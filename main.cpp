//
// Created by Nico on 1/26/2026.
//

#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include "Project.h"
#include "Persistence.h"
#include "AnalysisBridge.h"
using std::string;
using std::vector;
// Print menus

void printMainMenu()
{
    std::cout << "\nMain Menu\n";
    std::cout << "__________\n";
    std::cout << "1. Create Project\n";
    std::cout << "2. View Projects\n";
    std::cout << "3. Open a Project\n";
    std::cout << "4. Smart Analysis Report\n";
    std::cout << "5. Exit\n";
    std::cout << "__________\n";
    std::cout << "Enter your choice (1-5): ";
}

void printProjectMenu(const Project &p)
{
    std::cout << "\nProject: " << p.getProjectName() << "\n";
    std::cout << "Status: " << p.getProjectState()
              << " | Priority: " << p.getPriority()
              << "\n";
    std::cout << "__________\n";
    std::cout << "1. List tasks\n";
    std::cout << "2. Add task\n";
    std::cout << "3. Change task state\n";
    std::cout << "4. Remove task\n";
    std::cout << "5. Rename project\n";
    std::cout << "6. Change project status\n";
    std::cout << "7. Timed task controls\n";
    std::cout << "8. Back\n";
    std::cout << "__________\n";
    std::cout << "Enter your choice (1-8): ";
}

// Input validators
int getIntInRange(int low, int high)
{
    int value;

    while (true)
    {
        std::cin >> value;

        bool valid = !std::cin.fail() && value >= low && value <= high;
        // clearing / handling invalid input need to cite
        std::cin.clear();
        std::cin.ignore(10000, '\n');

        if (valid)
        {
            return value;
        }

        std::cout << "Invalid input. Please enter a number between "
                  << low << " and " << high << ".\n";
        std::cout << "Try again: ";
    }
}

int getPositiveInt(const string &prompt)
{
    while (true)
    {
        std::cout << prompt;
        int value;
        std::cin >> value;

        bool valid = !std::cin.fail() && value > 0;
        // handling invalid input need to cite
        std::cin.clear();
        std::cin.ignore(10000, '\n');

        if (valid)
            return value;

        std::cout << "Invalid input. Please enter a positive number.\n";
    }
}

string getNonEmptyLine(const string &prompt)
{
    while (true)
    {
        std::cout << prompt;
        string line;
        std::getline(std::cin, line);

        if (!line.empty())
        {
            return line;
        }

        std::cout << "Input cannot be empty. Try again.\n";
    }
}

// Main Menu

void createProject(vector<Project> &projects)
{
    std::cout << "\nCreate Project\n";
    string name = getNonEmptyLine("Project name: ");

    std::cout << "Priority (1-5): ";
    int priority = getIntInRange(1, 5);

    Project p(name);
    p.setPriority(priority);

    projects.push_back(std::move(p)); // move operator in cpp should cite
    std::cout << "Project created.\n";
}
// analyssi report
void runSmartAnalysisReport(const vector<Project> &projects)
{
    std::cout << "\nSmart Analysis Report\n";
    std::cout << "_____________________\n";

    if (projects.empty())
    {
        std::cout << "No projects available to analyze.\n";
        return;
    }

    std::cout << "Projects currently loaded: " << projects.size() << "\n";

    std::string exportMessage;
    const std::filesystem::path analysisInputPath = getAnalysisInputPath();
    const std::filesystem::path analysisOutputPath = getAnalysisOutputPath();
    const bool exported = exportProjectsForAnalysis(projects, analysisInputPath, exportMessage);

    if (!exported)
    {
        std::cout << exportMessage << "\n";
        return;
    }

    std::cout << exportMessage << "\n";

    std::string runMessage;
    // runing the python script
    const bool ranPython = runSmartAnalysisPython(analysisInputPath, analysisOutputPath, runMessage);
    std::cout << runMessage << "\n";

    if (ranPython)
    { // loading the output
        std::cout << "Analysis output file: " << analysisOutputPath.string() << "\n";

        AnalysisReport report;
        std::string loadMessage;
        const bool loaded = loadAnalysisReport(analysisOutputPath, report, loadMessage);

        if (!loaded)
        {
            std::cout << loadMessage << "\n";
            return;
        }

        printAnalysisReport(report, std::cout);
    }
}

void viewProjects(const vector<Project> &projects)
{
    std::cout << "\nProjects\n";

    if (projects.empty())
    {
        std::cout << "No projects yet.\n";
        return;
    }

    for (std::size_t i = 0; i < projects.size(); i++)
    {
        std::cout << "  [" << (i + 1) << "] "
                  << projects[i].getProjectName()
                  << " | Priority: " << projects[i].getPriority()
                  << " | Status: " << projects[i].getProjectState()
                  << " | Tasks: " << projects[i].getTaskCount()
                  << "\n";
    }
}

int chooseProjectIndex(const vector<Project> &projects)
{
    if (projects.empty())
    {
        std::cout << "No projects to open.\n";
        return -1;
    }

    viewProjects(projects);
    std::cout << "\nChoose a project number (1-" << projects.size() << "): ";
    int choice = getIntInRange(1, static_cast<int>(projects.size()));
    return choice - 1;
}

// Task functions and menus

TaskState chooseTaskState()
{
    std::cout << "New state:\n";
    std::cout << "  1. ToDo\n";
    std::cout << "  2. Doing\n";
    std::cout << "  3. Done\n";
    std::cout << "Choose (1-3): ";
    int c = getIntInRange(1, 3);

    if (c == 1)
    {
        return TaskState::ToDo;
    }
    if (c == 2)
    {
        return TaskState::Doing;
    }
    return TaskState::Done;
}

ProjectStatus chooseProjectStatus()
{
    std::cout << "New status:\n";
    std::cout << "  1. Planned\n";
    std::cout << "  2. Active\n";
    std::cout << "  3. Paused\n";
    std::cout << "  4. Completed\n";
    std::cout << "Choose (1-4): ";
    int c = getIntInRange(1, 4);

    if (c == 1)
    {
        return ProjectStatus::Planned;
    }
    if (c == 2)
    {
        return ProjectStatus::Active;
    }
    if (c == 3)
    {
        return ProjectStatus::Paused;
    }
    return ProjectStatus::Completed;
}

int chooseTaskIndex(Project &p)
{
    if (p.getTaskCount() == 0)
    {
        std::cout << "No tasks.\n";
        return -1;
    }

    // Sync timers before displaying
    p.syncTimers(std::time(nullptr));

    p.printTasksWithIndices(std::cout);

    std::cout << "Choose a task number (1-" << p.getTaskCount() << "): ";
    int choice = getIntInRange(1, p.getTaskCount());
    return choice - 1; // minus one to make it actually lineup with the index
}

void listTasks(Project &p)
{
    // Sync all timers before displaying
    p.syncTimers(std::time(nullptr));

    // Check for expired timers and prompt user
    for (int i = 0; i < p.getTaskCount(); i++)
    {
        Task *task = p.getTask(i);
        if (task->getKind() == TaskKind::Timed && task->taskState != TaskState::Done)
        {
            TimedTask *timedTask = dynamic_cast<TimedTask *>(task);
            if (timedTask && timedTask->getExpiredPromptPending())
            {
                std::cout << "\nTimer expired for task: " << task->taskName << "\n";
                std::cout << "Mark as Done? (y/n): ";

                string response;
                std::getline(std::cin, response);

                if (response == "y" || response == "Y")
                {
                    p.changeTaskState(i, TaskState::Done);
                }

                timedTask->setExpiredPromptPending(false);
            }
        }
    }

    std::cout << "\nTasks:\n";
    p.printTasksWithIndices(std::cout);
}

void addTaskToProject(Project &p)
{
    std::cout << "\nAdd Task\n";
    std::cout << "Task type:\n";
    std::cout << "1. Standard\n";
    std::cout << "2. Timed\n";
    std::cout << "Enter choice (1-2): ";

    int taskType = getIntInRange(1, 2);

    string taskName = getNonEmptyLine("Task name: ");

    if (taskType == 1)
    {
        p.addTask(taskName);
        std::cout << "Standard task added.\n";
    }
    else
    {
        int totalSeconds = getPositiveInt("Timer duration (seconds): ");
        p.addTimedTask(taskName, totalSeconds);
        std::cout << "Timed task added.\n";
    }
}

void changeTaskState(Project &p)
{
    std::cout << "\nChange Task State\n";
    int idx = chooseTaskIndex(p);
    if (idx == -1)
        return;

    TaskState newState = chooseTaskState();
    p.changeTaskState(static_cast<std::size_t>(idx), newState);

    std::cout << "Task updated.\n";
}

void removeTask(Project &p)
{
    std::cout << "\nRemove Task\n";
    int idx = chooseTaskIndex(p);
    if (idx == -1)
        return;

    p.removeTask(static_cast<std::size_t>(idx));
    std::cout << "Task removed.\n";
}

void renameProject(Project &p)
{
    std::cout << "\nRename Project\n";
    string newName = getNonEmptyLine("New project name: ");
    p.setName(newName);
    std::cout << "Project renamed.\n";
}

void changeProjectStatus(Project &p)
{
    std::cout << "\nChange Project Status\n";
    p.setState(chooseProjectStatus());
    std::cout << "Status updated.\n";
}

void timedTaskControls(Project &p)
{
    std::cout << "\nTimed Task Controls\n";
    int idx = chooseTaskIndex(p);
    if (idx == -1)
        return;

    Task *task = p.getTask(idx);
    if (task->getKind() != TaskKind::Timed)
    {
        std::cout << "This is not a timed task.\n";
        return;
    }

    std::cout << "\nTimer Controls for: " << task->taskName << "\n";
    std::cout << "1. Start timer\n";
    std::cout << "2. Pause timer\n";
    std::cout << "3. Reset timer\n";
    std::cout << "4. Cancel\n";
    std::cout << "Enter choice (1-4): ";

    int choice = getIntInRange(1, 4);

    switch (choice)
    {
    case 1:
        // Sync timer before checking if expired
        p.syncTimers(std::time(nullptr));

        // Check if timer is expired or at 0 before starting
        if (task->getRemainingSeconds() <= 0)
        {
            std::cout << "Timer has expired (0s remaining).\n";
            std::cout << "Would you like to reset the timer? (y/n): ";
            string response;
            std::getline(std::cin, response);

            if (response == "y" || response == "Y")
            {
                p.resetTaskTimer(idx);
                std::cout << "Timer reset.\n";
            }
            else
            {
                std::cout << "Timer not started.\n";
            }
        }
        else
        {
            p.startTaskTimer(idx);
            std::cout << "Timer started.\n";
        }
        break;
    case 2:
        p.pauseTaskTimer(idx);
        std::cout << "Timer paused.\n";
        break;
    case 3:
        p.resetTaskTimer(idx);
        std::cout << "Timer reset.\n";
        break;
    case 4:
        return;
    }
}

void projectMenu(Project &p)
{
    while (true)
    {
        printProjectMenu(p);
        int choice = getIntInRange(1, 8);

        switch (choice)
        {
        case 1:
            listTasks(p);
            break;
        case 2:
            addTaskToProject(p);
            break;
        case 3:
            changeTaskState(p);
            break;
        case 4:
            removeTask(p);
            break;
        case 5:
            renameProject(p);
            break;
        case 6:
            changeProjectStatus(p);
            break;
        case 7:
            timedTaskControls(p);
            break;
        case 8:
            return;
        }
    }
}
// main
int main()
{
    vector<Project> projects;

    // Load projects from file on startup
    std::filesystem::path savePath = getDefaultSavePath();
    std::string loadMessage;
    loadProjects(savePath, projects, loadMessage);
    std::cout << loadMessage << "\n";

    // Sync all timers after loading to account for elapsed time
    std::time_t currentTime = std::time(nullptr);
    for (auto &project : projects)
    {
        project.syncTimers(currentTime);
    }

    while (true)
    {
        printMainMenu();
        int choice = getIntInRange(1, 5);

        switch (choice)
        {
        case 1:
            createProject(projects);
            break;
        case 2:
            viewProjects(projects);
            break;
        case 3:
        {
            int idx = chooseProjectIndex(projects);
            if (idx != -1)
            {
                projectMenu(projects[idx]);
            }
            break;
        }
        case 4:
            runSmartAnalysisReport(projects);
            break;
        case 5:
            // Sync and save projects before exit
            currentTime = std::time(nullptr);
            for (auto &project : projects)
            {
                project.syncTimers(currentTime);
            }

            std::string saveMessage;
            saveProjects(savePath, projects, saveMessage);
            std::cout << saveMessage << "\n";
            std::cout << "Goodbye!\n";
            return 0;
        }
    }
}
