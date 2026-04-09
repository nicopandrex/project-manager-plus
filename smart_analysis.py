import json
import sys
from pathlib import Path
import pandas as pd

VALID_PROJECT_STATUS = {"Planned", "Active", "Paused", "Completed"}
VALID_TASK_KIND = {"Standard", "Timed"}
VALID_TASK_STATE = {"ToDo", "Doing", "Done"}

#function for deciding the reason of why a task is reccomended 
def build_recommendation_reason(row):
    parts = []

    if row["project_priority"] <= 2:
        parts.append("high-priority project")
    elif row["project_priority"] >= 4:
        parts.append("lower-priority project")

    if row["project_status"] == "Active":
        parts.append("project is active")
    elif row["project_status"] == "Planned":
        parts.append("project is planned")

    if row["task_state"] == "Doing":
        parts.append("task already in progress")
    elif row["task_state"] == "ToDo":
        parts.append("task is ready to start")

    if row["task_kind"] == "Timed":
        remaining = row.get("remaining_seconds_num", 10**9)
        if remaining <= 900:
            parts.append("timed task with less than 15 minutes left")
        elif remaining <= 1800:
            parts.append("timed task with 15-30 minutes left")
        else:
            parts.append("timed task")

    if not parts:
        return "ranked by project priority and task status"

    return "; ".join(parts)


def parse_args(argv):
    if len(argv) != 4:
        raise ValueError(
            "Please run this like: python smart_analysis.py analyze <input_json> <output_json>"
        )

    if argv[1] != "analyze":
        raise ValueError(f"Invalid command'{argv[1]}'. Please do: analyze")

    input_path = Path(argv[2])
    output_path = Path(argv[3])

    if not input_path.exists():
        raise ValueError(f"Input file not found: {input_path}")
    if not input_path.is_file():
        raise ValueError(f"Input path is not a file: {input_path}")

    if output_path.exists() and output_path.is_dir():
        raise ValueError(f"Output path is a directory, expected file: {output_path}")

    return input_path, output_path


def load_json(input_path):
    try:
        return json.loads(input_path.read_text(encoding="utf-8"))
    except OSError as error:
        raise ValueError(f"Failed reading input JSON: {error}")
    except json.JSONDecodeError as error:
        raise ValueError(f"Invalid JSON format: {error}")

#takes the input json and builds a pandas dataframe, creating one row per task, to analyze 
def build_task_table(payload):
    rows = []
    for project in payload["projects"]:
        for task in project["tasks"]:
            rows.append(
                {
                    "project": project["name"],
                    "project_status": project["status"],
                    "project_priority": project["priority"],
                    "task": task["name"],
                    "task_kind": task["kind"],
                    "task_state": task["state"],
                    "remaining_seconds": task.get("remaining_seconds"),
                    "running": task.get("running"),
                }
            )

    return pd.DataFrame(
        rows,
        columns=[
            "project",
            "project_status",
            "project_priority",
            "task",
            "task_kind",
            "task_state",
            "remaining_seconds",
            "running",
        ],
    )

#building the summar for the report, couting projects tasks etc
def build_summary(payload, tasks_df):
    if tasks_df.empty:
        done_count = 0
        timed_count = 0
    else:
        done_count = int((tasks_df["task_state"] == "Done").sum()) #using sum because true 1 false 0
        timed_count = int((tasks_df["task_kind"] == "Timed").sum())

    return {
        "project_count": int(len(payload["projects"])),
        "task_count": int(len(tasks_df)),
        "done_count": done_count,
        "timed_task_count": timed_count,
    }

#
def build_project_breakdown(payload, tasks_df):
    breakdown = []
    #go through projects, count tasks and find completion percentage
    for project in payload["projects"]:
        project_name = project["name"]
        project_rows = tasks_df[tasks_df["project"] == project_name] if not tasks_df.empty else pd.DataFrame()

        total_tasks = int(len(project_rows))
        done_tasks = int((project_rows["task_state"] == "Done").sum()) if total_tasks > 0 else 0
        completion_percent = round((done_tasks / total_tasks) * 100, 1) if total_tasks > 0 else 0.0
    #appending info to the list via a dict
        breakdown.append(
            {
                "project": project_name,
                "priority": int(project["priority"]),
                "status": project["status"],
                "total_tasks": total_tasks,
                "done_tasks": done_tasks,
                "completion_percent": float(completion_percent),
            }
        )

    return breakdown

#using pandas to reccomend next tasks via a scoring system
#the scoring system is based on project priority, project status, task state, and timer urgency
def build_recommendations(tasks_df):
    if tasks_df.empty:
        return []
    #filtering out tasks that are already done or in completed/paused projects
    candidates = tasks_df[
        (tasks_df["task_state"] != "Done")
        & (tasks_df["project_status"] != "Completed")
        & (tasks_df["project_status"] != "Paused")
    ].copy()
    
    if candidates.empty:
        return []
    #converting the remaining seconds to number for safer operations missing values become a large number to indivate lower urgency
    candidates["remaining_seconds_num"] = pd.to_numeric(
        candidates["remaining_seconds"], errors="coerce" #if conversion fails set to NaN
    ).fillna(10**9)
#scoring system, higher score higher reccomendation
    candidates["score"] = 6 - candidates["project_priority"] #if priority 1 then 5 points etc
    candidates["score"] += (candidates["task_state"] == "Doing") * 5 #prioritize tasks actively doing
    candidates["score"] += (candidates["task_state"] == "ToDo") * 3 #then tasks not started
    candidates["score"] += (candidates["project_status"] == "Active") * 4 #active projects more important than planned ones
    candidates["score"] += (candidates["task_kind"] == "Timed") * 2 #time tasks have greater urgency
    candidates["score"] += (
        (candidates["task_kind"] == "Timed")
        & (candidates["remaining_seconds_num"] <= 900) #if less than 15 minutes left on timer then very urgent
    ) * 3
    candidates["score"] += (
        (candidates["task_kind"] == "Timed")
        & (candidates["remaining_seconds_num"] > 900)
        & (candidates["remaining_seconds_num"] <= 1800)
    ) * 1 #between 15 and 30 min
    
    ordered = candidates.sort_values(
        by=["score", "project_priority", "project", "task"],
        ascending=[False, True, True, True],
    ).head(3) #get the top 3 reccomendations for next tasks

    recommendations = []
    
    #builds out the reccomendations
    for _, row in ordered.iterrows(): 
        recommendations.append(
            {
                "project": row["project"],
                "task": row["task"],
                "reason": build_recommendation_reason(row),
                "score": int(row["score"]),
            }
        )

    return recommendations

#identifies timed 
def build_report(payload):
    tasks_df = build_task_table(payload)

    return {
        "summary": build_summary(payload, tasks_df),
        "project_breakdown": build_project_breakdown(payload, tasks_df),
        "recommendations": build_recommendations(tasks_df),
    }


def write_output_json(output_path, report):
    try:
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(
            json.dumps(report, separators=(",", ":"), ensure_ascii=False),
            encoding="utf-8",
        )
    except OSError as error:
        raise ValueError(f"Failed writing output JSON: {error}")


def validate_payload(payload):
    #checking that its a dictionary {}
    if not isinstance(payload, dict):
        raise ValueError("Invalid type: expected dict")
    #Checking for projects
    if "projects" not in payload:
        raise ValueError("Missing key 'projects' in root object")
    #making sure it is a list 
    projects = payload["projects"]
    if not isinstance(projects, list):
        raise ValueError("Invalid type for 'projects': expected list")

    for i, project in enumerate(projects):
        if not isinstance(project, dict):
            raise ValueError(f"Invalid type for 'projects[{i}]': expected dict")
        #making sure the project has the right components like thhe name, status, priority and tasks
        for key in ("name", "status", "priority", "tasks"):
            if key not in project:
                raise ValueError(f"Missing key '{key}' in projects[{i}]")
        #type checking for the project components
        if not isinstance(project["name"], str):
            raise ValueError(f"Invalid type for 'projects[{i}].name': expected str")
        if not isinstance(project["status"], str):
            raise ValueError(f"Invalid type for 'projects[{i}].status': expected str")
        if not isinstance(project["priority"], int):
            raise ValueError(f"Invalid type for 'projects[{i}].priority': expected int")
        if not isinstance(project["tasks"], list):
            raise ValueError(f"Invalid type for 'projects[{i}].tasks': expected list")

        if project["status"] not in VALID_PROJECT_STATUS:
            raise ValueError(
                f"Invalid project status '{project['status']}' in projects[{i}]; "
                f"expected one of {sorted(VALID_PROJECT_STATUS)}"
            )
        if project["priority"] < 1 or project["priority"] > 5:
            raise ValueError(
                f"Invalid priority '{project['priority']}' in projects[{i}]; expected 1-5"
            )
    #type checking for the tasks in the project
        for j, task in enumerate(project["tasks"]):
            prefix = f"projects[{i}].tasks[{j}]"

            if not isinstance(task, dict):
                raise ValueError(f"Invalid type for '{prefix}': expected dict")

            for key in ("name", "kind", "state"):
                if key not in task:
                    raise ValueError(f"Missing key '{key}' in {prefix}")

            if not isinstance(task["name"], str):
                raise ValueError(f"Invalid type for '{prefix}.name': expected str")
            if not isinstance(task["kind"], str):
                raise ValueError(f"Invalid type for '{prefix}.kind': expected str")
            if not isinstance(task["state"], str):
                raise ValueError(f"Invalid type for '{prefix}.state': expected str")

            if task["kind"] not in VALID_TASK_KIND:
                raise ValueError(
                    f"Invalid task kind '{task['kind']}' in {prefix}; "
                    f"expected one of {sorted(VALID_TASK_KIND)}"
                )
            if task["state"] not in VALID_TASK_STATE:
                raise ValueError(
                    f"Invalid task state '{task['state']}' in {prefix}; "
                    f"expected one of {sorted(VALID_TASK_STATE)}"
                )
            #if timed task checking for the right components and types
            if task["kind"] == "Timed":
                for key in ("original_seconds", "remaining_seconds", "running"):
                    if key not in task:
                        raise ValueError(f"Missing key '{key}' in timed task {prefix}")

                if not isinstance(task["original_seconds"], int):
                    raise ValueError(f"Invalid type for '{prefix}.original_seconds': expected int")
                if not isinstance(task["remaining_seconds"], int):
                    raise ValueError(f"Invalid type for '{prefix}.remaining_seconds': expected int")
                if not isinstance(task["running"], bool):
                    raise ValueError(f"Invalid type for '{prefix}.running': expected bool")

    return payload


def main(argv):
    try:
        input_path, output_path = parse_args(argv)
        payload = load_json(input_path)
        validate_payload(payload)
        report = build_report(payload)
        write_output_json(output_path, report)
    except ImportError as exc:
        print(f"Missing dependency: {exc}", file=sys.stderr)
        return 2
    except ValueError as exc:
        print(f"Validation error: {exc}", file=sys.stderr)
        return 2

    print(f"Analysis complete. Output written to: {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
