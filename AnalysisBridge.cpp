#include "AnalysisBridge.h"
#include "Persistence.h"
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <ctime>
#include <fstream>
// custom library for json handling, should cite
using json = nlohmann::json;

// This file is for writing out information to a JSON file to then be given to python for analysis with pands

//  escape for Json conversion
static std::string escapeJsonString(const std::string &input)
{
    std::string escaped;

    for (char ch : input)
    {
        switch (ch)
        {
        case '"':
            escaped += "\\\"";
            break;
        case '\\':
            escaped += "\\\\";
            break;
        case '\n':
            escaped += "\\n";
            break;
        default:
            escaped += ch;
            break;
        }
    }

    return escaped;
}

static std::string taskKindToString(TaskKind kind)
{
    switch (kind)
    {
    case TaskKind::Standard:
        return "Standard";
    case TaskKind::Timed:
        return "Timed";
    }

    return "Standard";
}
// helper for getting filepaths better
static std::string quoteCommandArgument(const std::filesystem::path &path)
{
    return "\"" + path.string() + "\"";
}
// function for finding the analysis file, better than just hardcode path
static std::filesystem::path resolveSmartAnalysisScriptPath()
{
    const std::filesystem::path cwd = std::filesystem::current_path();

    const std::filesystem::path candidate1 = cwd / "smart_analysis.py";
    if (std::filesystem::exists(candidate1))
    {
        return candidate1;
    }

    const std::filesystem::path candidate2 = cwd / ".." / "smart_analysis.py";
    if (std::filesystem::exists(candidate2))
    {
        return candidate2;
    }

    const std::filesystem::path candidate3 = cwd / ".." / ".." / "smart_analysis.py";
    if (std::filesystem::exists(candidate3))
    {
        return candidate3;
    }

    return "smart_analysis.py";
}

// getting filepaths
std::filesystem::path getAnalysisInputPath()
{
    return ".m3oep_nlippiat/analysis_input.json";
}

std::filesystem::path getAnalysisOutputPath()
{
    return ".m3oep_nlippiat/analysis_output.json";
}

// Functionf for exporting projects to a JSON file to be analysized in python
bool exportProjectsForAnalysis(const std::vector<Project> &projects,
                               const std::filesystem::path &outputPath,
                               std::string &message)
{
    const std::filesystem::path directory = outputPath.parent_path();
    if (!directory.empty() && !std::filesystem::exists(directory))
    {
        std::filesystem::create_directories(directory);
    }

    std::ofstream outFile(outputPath);
    if (!outFile.is_open())
    {
        message = "Error: Failed to create analysis input file.";
        return false;
    }
    // starting to write the json file, creating an array of projects, array of tasks
    outFile << "{\n";
    outFile << "  \"projects\": [\n";
    // looping through each project
    for (std::size_t i = 0; i < projects.size(); i++)
    {
        const Project &project = projects[i];
        // project details
        outFile << "    {\n";
        outFile << "      \"name\": \"" << escapeJsonString(project.getProjectName()) << "\",\n";
        outFile << "      \"status\": \"" << projectStatusToString(project.getProjectState()) << "\",\n";
        outFile << "      \"priority\": " << project.getPriority() << ",\n";
        outFile << "      \"tasks\": [\n";

        // going through each task
        const int taskCount = project.getTaskCount();
        for (int taskIndex = 0; taskIndex < taskCount; taskIndex++)
        {
            const Task *task = project.getTask(static_cast<std::size_t>(taskIndex));
            const TaskKind kind = task->getKind();

            outFile << "        {\n";
            outFile << "          \"name\": \"" << escapeJsonString(task->taskName) << "\",\n";
            outFile << "          \"kind\": \"" << taskKindToString(kind) << "\",\n";
            outFile << "          \"state\": \"" << taskStateToString(task->taskState) << "\"";
            // handling timed tasks
            if (kind == TaskKind::Timed)
            {
                const TimedTask *timedTask = dynamic_cast<const TimedTask *>(task);
                const CountdownTimer &timer = timedTask->getTimer();

                outFile << ",\n";
                outFile << "          \"original_seconds\": " << timer.getOriginalTotalSeconds() << ",\n";
                outFile << "          \"remaining_seconds\": " << timer.getRemainingSeconds() << ",\n";
                outFile << "          \"running\": " << (timer.isRunning() ? "true" : "false") << "\n";
            }
            else
            {
                outFile << "\n";
            }
            // closing the task json object
            outFile << "        }";
            if (taskIndex + 1 < taskCount)
            {
                outFile << ",";
            }
            outFile << "\n";
        }
        // closing the tasks array and project json object
        outFile << "      ]\n";
        outFile << "    }";
        if (i + 1 < projects.size())
        {
            outFile << ",";
        }
        outFile << "\n";
    }

    outFile << "  ]\n";
    outFile << "}\n";

    message = "Analysis input exported to: " + outputPath.string();
    return true;
}
// building the command to run the py script
std::string buildSmartAnalysisCommand(const std::filesystem::path &inputPath,
                                      const std::filesystem::path &outputPath)
{
    const std::filesystem::path scriptPath = resolveSmartAnalysisScriptPath();

    return "python " + quoteCommandArgument(scriptPath) + " analyze " +
           quoteCommandArgument(inputPath) + " " +
           quoteCommandArgument(outputPath);
}
// runningpython script
bool runSmartAnalysisPython(const std::filesystem::path &inputPath,
                            const std::filesystem::path &outputPath,
                            std::string &message)
{
    const std::string command = buildSmartAnalysisCommand(inputPath, outputPath);
    const int exitCode = std::system(command.c_str());

    if (exitCode == 0)
    {
        message = "Python analysis completed successfully.";
        return true;
    }

    if (exitCode == -1)
    {
        message = "Error: Failed to launch Python command. Please verify Python is installed and available on PATH.";
        return false;
    }

    message = "Error: Python analysis failed with exit code " + std::to_string(exitCode) +
              ". Please verify Python, pandas, and script inputs.";
    return false;
}

bool loadAnalysisReport(const std::filesystem::path &outputPath,
                        AnalysisReport &report,
                        std::string &message)
{
    const std::string malformedOutputMessage =
        "Error: Analysis output is missing or broken. Please rerun Smart Analysis.";

    std::ifstream input(outputPath);
    if (!input.is_open())
    {
        message = "Error: Could not open analysis output file.";
        return false;
    }

    try
    { // parsin json output from python script
        json root;
        input >> root;
        // creating the report struct to hold the data
        AnalysisReport parsedReport;

        const json &summary = root.at("summary");
        parsedReport.summary.projectCount = summary.at("project_count").get<int>();
        parsedReport.summary.taskCount = summary.at("task_count").get<int>();
        parsedReport.summary.doneCount = summary.at("done_count").get<int>();
        parsedReport.summary.timedTaskCount = summary.at("timed_task_count").get<int>();
        // looping through the project items and recommendations and adding them to the report struct
        for (const auto &item : root.at("project_breakdown"))
        {
            ProjectBreakdownItem parsedItem;
            parsedItem.project = item.at("project").get<std::string>();
            parsedItem.priority = item.at("priority").get<int>();
            parsedItem.status = item.at("status").get<std::string>();
            parsedItem.totalTasks = item.at("total_tasks").get<int>();
            parsedItem.doneTasks = item.at("done_tasks").get<int>();
            parsedItem.completionPercent = item.at("completion_percent").get<double>();
            parsedReport.projectBreakdown.push_back(parsedItem);
        }

        for (const auto &item : root.at("recommendations"))
        {
            RecommendationItem parsedItem;
            parsedItem.project = item.at("project").get<std::string>();
            parsedItem.task = item.at("task").get<std::string>();
            parsedItem.reason = item.at("reason").get<std::string>();
            parsedItem.score = item.at("score").get<int>();
            parsedReport.recommendations.push_back(parsedItem);
        }

        report = parsedReport;
    } // errors in parsing
    catch (...)
    {
        message = malformedOutputMessage;
        return false;
    }

    message = "Analysis output loaded successfully.";
    return true;
}

// printing the report

void printAnalysisReport(const AnalysisReport &report, std::ostream &out)
{
    out << "\nSummary\n";
    out << "-------\n";
    out << "Projects: " << report.summary.projectCount << "\n";
    out << "Tasks: " << report.summary.taskCount << "\n";
    out << "Done: " << report.summary.doneCount << "\n";
    out << "Timed tasks: " << report.summary.timedTaskCount << "\n";

    out << "\nProject Breakdown\n";
    out << "-----------------\n";
    if (report.projectBreakdown.empty())
    {
        out << "none found\n";
    }
    else
    {
        for (const auto &item : report.projectBreakdown)
        {
            out << "- " << item.project
                << " | Priority " << item.priority
                << " | " << item.status
                << " | " << item.doneTasks << "/" << item.totalTasks
                << " done (" << item.completionPercent << "%)\n";
        }
    }
    // pritning the recommendations
    out << "\nTop Recommended Tasks\n";
    out << "---------------------\n";
    if (report.recommendations.empty())
    {
        out << "none found\n";
    }
    else
    {
        for (const auto &item : report.recommendations)
        {
            out << "- [Score " << item.score << "] "
                << item.project << " :: " << item.task
                << "\n  Reason: " << item.reason << "\n";
        }
    }
}
