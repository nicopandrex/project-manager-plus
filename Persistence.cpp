//
// Created by Nico on 2/16/2026.
//

#include "Persistence.h"
#include <fstream>
#include <cstdlib>
#include <ctime>

// Helper function for special chars (for when we are writing to file)
std::string escapeString(const std::string &input)
{
    std::string result = "";

    for (int i = 0; i < input.length(); i++)
    {
        char ch = input[i];
        if (ch == '\\')
        {
            result += "\\\\";
        }
        else if (ch == '|')
        {
            result += "\\|";
        }
        else if (ch == '\n')
        {
            result += "\\n";
        }
        else
        {
            result += ch;
        }
    }

    return result;
}

// Helper function to unescape special characters (for when we are loading from file)
std::string unescapeString(const std::string &input)
{
    std::string result = "";

    for (int i = 0; i < input.length(); i++)
    {
        if (input[i] == '\\' && i + 1 < input.length())
        {
            char nextChar = input[i + 1];
            if (nextChar == '\\')
            {
                result += '\\';
                i++;
            }
            else if (nextChar == '|')
            {
                result += '|';
                i++;
            }
            else if (nextChar == 'n')
            {
                result += '\n';
                i++;
            }
            else
            {
                result += input[i];
            }
        }
        else
        {
            result += input[i];
        }
    }

    return result;
}

// Helper function to convert string to ProjectStatus
bool stringToProjectStatus(const std::string &str, ProjectStatus &status)
{
    if (str == "Planned")
    {
        status = ProjectStatus::Planned;
        return true;
    }
    else if (str == "Active")
    {
        status = ProjectStatus::Active;
        return true;
    }
    else if (str == "Paused")
    {
        status = ProjectStatus::Paused;
        return true;
    }
    else if (str == "Completed")
    {
        status = ProjectStatus::Completed;
        return true;
    }
    return false;
}

// Helper function to convert string to TaskState
bool stringToTaskState(const std::string &str, TaskState &state)
{
    if (str == "ToDo")
    {
        state = TaskState::ToDo;
        return true;
    }
    else if (str == "Doing")
    {
        state = TaskState::Doing;
        return true;
    }
    else if (str == "Done")
    {
        state = TaskState::Done;
        return true;
    }
    return false;
}

// Helper function to split a string by delimiter (for when we are reading the files)
std::vector<std::string> splitString(const std::string &str, char delimiter)
{
    std::vector<std::string> parts;
    std::string current = "";

    for (int i = 0; i < str.length(); i++)
    {
        if (str[i] == '\\' && i + 1 < str.length())
        {
            current += str[i];
            current += str[i + 1];
            i++;
        }
        else if (str[i] == delimiter)
        {
            parts.push_back(current);
            current = "";
        }
        else
        {
            current += str[i];
        }
    }
    parts.push_back(current);

    return parts;
}

// Helper function to convert ProjectStatus to string
std::string projectStatusToString(ProjectStatus status)
{
    switch (status)
    {
    case ProjectStatus::Planned:
        return "Planned";
    case ProjectStatus::Active:
        return "Active";
    case ProjectStatus::Paused:
        return "Paused";
    case ProjectStatus::Completed:
        return "Completed";
    default:
        return "Planned";
    }
}

// Helper function to convert TaskState to string
std::string taskStateToString(TaskState state)
{
    switch (state)
    {
    case TaskState::ToDo:
        return "ToDo";
    case TaskState::Doing:
        return "Doing";
    case TaskState::Done:
        return "Done";
    default:
        return "ToDo";
    }
}

// learned abt filesystem library in C++, should cite
std::filesystem::path getDefaultSavePath()
{
    std::filesystem::path savePath = ".m3oep_nlippiat/projects.txt";
    return savePath;
}

bool saveProjects(const std::filesystem::path &path,
                  const std::vector<Project> &projects,
                  std::string &message)
{
    // Create directory if it doesn't exist
    std::filesystem::path directory = path.parent_path();
    if (!directory.empty() && !std::filesystem::exists(directory))
    {
        std::filesystem::create_directories(directory);
    }

    // Open file
    std::ofstream file(path);
    if (!file.is_open())
    {
        message = "Error: Failed to open file";
        return false;
    }

    // project count
    file << "PROJECTS|" << projects.size() << "\n";

    // Write each project
    for (int i = 0; i < projects.size(); i++)
    {
        const Project &project = projects[i];

        file << "PROJECT_BEGIN\n";

        // Writing the project details
        std::string escapedName = escapeString(project.getProjectName());
        file << "NAME|" << escapedName << "\n";

        std::string statusString = projectStatusToString(project.getProjectState());
        file << "STATUS|" << statusString << "\n";

        file << "PRIORITY|" << project.getPriority() << "\n";

        int taskCount = project.getTaskCount();
        file << "TASKS|" << taskCount << "\n";

        // Write each task
        for (int j = 0; j < taskCount; j++)
        {
            Task *task = project.getTask(j);

            std::string escapedTaskName = escapeString(task->taskName);
            std::string taskStateString = taskStateToString(task->taskState);

            if (task->getKind() == TaskKind::Standard)
            {
                // Standard task format
                file << "TASK|Standard|" << escapedTaskName
                     << "|" << taskStateString << "\n";
            }
            else if (task->getKind() == TaskKind::Timed)
            {
                // Timed task format
                TimedTask *timedTask = dynamic_cast<TimedTask *>(task);
                if (timedTask)
                {
                    CountdownTimer &timer = timedTask->getTimer();

                    file << "TASK|Timed|" << escapedTaskName
                         << "|" << taskStateString
                         << "|" << timer.getOriginalTotalSeconds()
                         << "|" << timer.getRemainingSeconds()
                         << "|" << (timer.isRunning() ? "1" : "0")
                         << "|" << timer.getStartedEpoch()
                         << "|" << (timedTask->getExpiredPromptPending() ? "1" : "0") << "\n";
                }
            }
        }

        file << "PROJECT_END\n";
    }

    file.close();

    message = "Projects saved successfully";
    return true;
}

bool loadProjects(const std::filesystem::path &path,
                  std::vector<Project> &outProjects,
                  std::string &message)
{
    outProjects.clear();

    // start fresh if no file
    if (!std::filesystem::exists(path))
    {
        message = "No save file found, starting fresh";
        return true;
    }

    std::ifstream file(path);
    if (!file.is_open())
    {
        message = "Error: Cannot open save file";
        return false;
    }

    try
    {
        std::string line;

        //  project count
        std::getline(file, line);
        std::vector<std::string> countParts = splitString(line, '|'); // puting into a vector split by the delimeter
        int projectCount = std::stoi(countParts[1]);                  // getting the count from the second part of the vector

        // Read each project
        for (int i = 0; i < projectCount; i++)
        {
            std::getline(file, line); // PROJECT_BEGIN

            // Read project data

            // project name
            std::getline(file, line);
            std::vector<std::string> nameParts = splitString(line, '|');
            std::string projectName = unescapeString(nameParts[1]);
            // project status
            std::getline(file, line);
            std::vector<std::string> statusParts = splitString(line, '|');
            ProjectStatus projectStatus;
            stringToProjectStatus(statusParts[1], projectStatus);
            // project priority
            std::getline(file, line);
            std::vector<std::string> priorityParts = splitString(line, '|');
            int priority = std::stoi(priorityParts[1]);
            // task count
            std::getline(file, line);
            std::vector<std::string> taskCountParts = splitString(line, '|');
            int taskCount = std::stoi(taskCountParts[1]);

            // Creates a project from the loaded data
            Project project(projectName);
            project.setState(projectStatus);
            project.setPriority(priority);

            // Reads tasks from the loadedd data
            for (int j = 0; j < taskCount; j++)
            {
                std::getline(file, line);
                std::vector<std::string> taskParts = splitString(line, '|');

                std::string taskKind = taskParts[1];

                if (taskKind == "Standard")
                {
                    // Standard task: TASK|Standard|name|state
                    std::string taskName = unescapeString(taskParts[2]);
                    TaskState taskState;
                    stringToTaskState(taskParts[3], taskState);

                    project.addTask(taskName);
                    project.changeTaskState(j, taskState);
                }
                else if (taskKind == "Timed")
                {
                    // Timed task: TASK|Timed|name|state|originalTotalSec|remainingSec|running|startedEpoch|expiredPrompt
                    std::string taskName = unescapeString(taskParts[2]);
                    TaskState taskState;
                    stringToTaskState(taskParts[3], taskState);
                    int originalTotalSeconds = std::stoi(taskParts[4]);
                    int remainingSeconds = std::stoi(taskParts[5]);
                    bool running = (taskParts[6] == "1");
                    std::time_t startedEpoch = std::stoll(taskParts[7]);
                    bool expiredPrompt = (taskParts[8] == "1");

                    project.addTimedTask(taskName, originalTotalSeconds);
                    project.changeTaskState(j, taskState);

                    // Set timer state
                    Task *task = project.getTask(j);
                    if (task && task->getKind() == TaskKind::Timed)
                    {
                        TimedTask *timedTask = dynamic_cast<TimedTask *>(task);
                        if (timedTask)
                        {
                            CountdownTimer &timer = timedTask->getTimer();
                            timer.setRemainingSeconds(remainingSeconds);
                            if (running)
                            {
                                timer.setStartedEpoch(startedEpoch);
                                timer.setRunning(running);
                            }
                            timedTask->setExpiredPromptPending(expiredPrompt);
                        }
                    }
                }
            }

            std::getline(file, line); // PROJECT_END
            // adds the project to the vector of projects
            outProjects.push_back(std::move(project));
        }

        file.close();
        message = "Projects loaded successfully";
        return true;
    }
    catch (...)
    {
        file.close();
        message = "Warning: Corrupted save file, starting fresh";
        outProjects.clear();
        return true;
    }
}
