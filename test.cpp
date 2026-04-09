//
// Test file for M3OEP_NLIPPIAT Project Manager
//

#include <iostream>
#include <vector>
#include <ctime>
#include <filesystem>
#include "Project.h"
#include "Task.h"
#include "CountdownTimer.h"
#include "Persistence.h"

using namespace std;

bool test_Project();
bool test_Task();
bool test_CountdownTimer();
bool test_Persistence();

int main()
{
    if (test_Project())
    {
        cout << "Passed all Project test cases" << endl;
    }
    if (test_Task())
    {
        cout << "Passed all Task test cases" << endl;
    }
    if (test_CountdownTimer())
    {
        cout << "Passed all CountdownTimer test cases" << endl;
    }
    if (test_Persistence())
    {
        cout << "Passed all Persistence test cases" << endl;
    }
    return 0;
}

bool test_Project()
{
    bool passed = true;

    // Test default constructor
    Project p("Test Project");
    if (p.getProjectName() != "Test Project")
    {
        passed = false;
        cout << "FAILED project name test" << endl;
    }
    if (p.getProjectState() != ProjectStatus::Planned)
    {
        passed = false;
        cout << "FAILED default state test" << endl;
    }
    if (p.getPriority() != 5)
    {
        passed = false;
        cout << "FAILED default priority test" << endl;
    }
    if (p.getTaskCount() != 0)
    {
        passed = false;
        cout << "FAILED default task count test" << endl;
    }

    // Test setters
    p.setName("Updated Project");
    if (p.getProjectName() != "Updated Project")
    {
        passed = false;
        cout << "FAILED setName test" << endl;
    }

    p.setState(ProjectStatus::Completed);
    if (p.getProjectState() != ProjectStatus::Completed)
    {
        passed = false;
        cout << "FAILED setState test" << endl;
    }

    p.setPriority(5);
    if (p.getPriority() != 5)
    {
        passed = false;
        cout << "FAILED setPriority test" << endl;
    }

    p.setState(ProjectStatus::Completed);
    if (p.getProjectState() != ProjectStatus::Completed)
    {
        passed = false;
        cout << "FAILED setState test" << endl;
    }

    // Test adding standard tasks
    p.addTask("Task 1");
    p.addTask("Task 2");
    if (p.getTaskCount() != 2)
    {
        passed = false;
        cout << "FAILED addTask test" << endl;
    }

    // Test getting task
    Task *task = p.getTask(0);
    if (task == nullptr || task->taskName != "Task 1")
    {
        passed = false;
        cout << "FAILED getTask test" << endl;
    }

    // Test changing task state
    p.changeTaskState(0, TaskState::Done);
    if (task->taskState != TaskState::Done)
    {
        passed = false;
        cout << "FAILED changeTaskState test" << endl;
    }

    // Test removing task
    p.removeTask(0);
    if (p.getTaskCount() != 1)
    {
        passed = false;
        cout << "FAILED removeTask count test" << endl;
    }

    // Test adding timed task
    p.addTimedTask("Timed Task", 60);
    if (p.getTaskCount() != 2)
    {
        passed = false;
        cout << "FAILED addTimedTask test" << endl;
    }

    Task *timedTask = p.getTask(1);
    if (timedTask == nullptr || timedTask->getKind() != TaskKind::Timed)
    {
        passed = false;
        cout << "FAILED timed task kind test" << endl;
    }

    // Test timer controls
    p.startTaskTimer(1);
    if (!timedTask->isTimerRunning())
    {
        passed = false;
        cout << "FAILED startTaskTimer test" << endl;
    }

    p.pauseTaskTimer(1);
    if (timedTask->isTimerRunning())
    {
        passed = false;
        cout << "FAILED pauseTaskTimer test" << endl;
    }

    p.resetTaskTimer(1);
    if (timedTask->getRemainingSeconds() != 60)
    {
        passed = false;
        cout << "FAILED resetTaskTimer test" << endl;
    }

    return passed;
}

bool test_Task()
{
    bool passed = true;

    // Test standard task creation
    Task task("Standard Task");
    if (task.taskName != "Standard Task")
    {
        passed = false;
        cout << "FAILED task name test" << endl;
    }
    if (task.taskState != TaskState::ToDo)
    {
        passed = false;
        cout << "FAILED default task state test" << endl;
    }
    if (task.getKind() != TaskKind::Standard)
    {
        passed = false;
        cout << "FAILED task kind test" << endl;
    }

    // Test task state changes
    task.taskState = TaskState::Doing;
    if (task.taskState != TaskState::Doing)
    {
        passed = false;
        cout << "FAILED task state change test" << endl;
    }

    // Test timed task creation
    TimedTask timedTask("Timed Task", 120);
    if (timedTask.taskName != "Timed Task")
    {
        passed = false;
        cout << "FAILED timed task name test" << endl;
    }
    if (timedTask.getKind() != TaskKind::Timed)
    {
        passed = false;
        cout << "FAILED timed task kind test" << endl;
    }
    if (timedTask.getRemainingSeconds() != 120)
    {
        passed = false;
        cout << "FAILED timed task initial seconds test" << endl;
    }
    if (timedTask.isTimerRunning())
    {
        passed = false;
        cout << "FAILED timed task not running initially test" << endl;
    }
    if (timedTask.isTimerExpired())
    {
        passed = false;
        cout << "FAILED timed task not expired initially test" << endl;
    }

    // Test timer controls
    timedTask.startTimer();
    if (!timedTask.isTimerRunning())
    {
        passed = false;
        cout << "FAILED startTimer test" << endl;
    }

    timedTask.pauseTimer();
    if (timedTask.isTimerRunning())
    {
        passed = false;
        cout << "FAILED pauseTimer test" << endl;
    }

    timedTask.resetTimer();
    if (timedTask.getRemainingSeconds() != 120)
    {
        passed = false;
        cout << "FAILED resetTimer test" << endl;
    }

    return passed;
}

bool test_CountdownTimer()
{
    bool passed = true;

    // Test construction
    CountdownTimer timer(30);
    if (timer.getOriginalTotalSeconds() != 30)
    {
        passed = false;
        cout << "FAILED original total seconds test" << endl;
    }
    if (timer.getTotalSeconds() != 30)
    {
        passed = false;
        cout << "FAILED total seconds test" << endl;
    }
    if (timer.getRemainingSeconds() != 30)
    {
        passed = false;
        cout << "FAILED initial remaining seconds test" << endl;
    }
    if (timer.isRunning())
    {
        passed = false;
        cout << "FAILED timer not running initially test" << endl;
    }
    if (timer.isExpired())
    {
        passed = false;
        cout << "FAILED timer not expired initially test" << endl;
    }

    // Test start
    timer.start();
    if (!timer.isRunning())
    {
        passed = false;
        cout << "FAILED start timer test" << endl;
    }
    if (timer.getStartedEpoch() == 0)
    {
        passed = false;
        cout << "FAILED started epoch test" << endl;
    }

    // Test sync
    time_t current = time(nullptr);
    timer.sync(current + 5);
    if (timer.getRemainingSeconds() > 25)
    {
        passed = false;
        cout << "FAILED sync reduces time test" << endl;
    }

    // Test pause
    timer.pause();
    if (timer.isRunning())
    {
        passed = false;
        cout << "FAILED pause test" << endl;
    }

    // Test reset
    timer.reset();
    if (timer.getRemainingSeconds() != 30)
    {
        passed = false;
        cout << "FAILED reset remaining test" << endl;
    }
    if (timer.isRunning())
    {
        passed = false;
        cout << "FAILED reset stops timer test" << endl;
    }

    // Test expiration
    timer.start();
    timer.sync(current + 100);
    if (!timer.isExpired())
    {
        passed = false;
        cout << "FAILED expiration test" << endl;
    }
    if (timer.getRemainingSeconds() != 0)
    {
        passed = false;
        cout << "FAILED expired remaining is 0 test" << endl;
    }
    if (timer.isRunning())
    {
        passed = false;
        cout << "FAILED expired stops timer test" << endl;
    }

    return passed;
}

bool test_Persistence()
{
    bool passed = true;

    filesystem::path testPath = "test_save.txt";
    vector<Project> projects;
    string message;

    // Create test projects
    Project p1("Project 1");
    p1.setPriority(5);
    p1.setState(ProjectStatus::Active);
    p1.addTask("Task 1");
    p1.addTask("Task 2");
    p1.changeTaskState(0, TaskState::Done);

    Project p2("Project 2");
    p2.addTimedTask("Timed Task", 120);

    projects.push_back(std::move(p1));
    projects.push_back(std::move(p2));

    // Test saving
    if (!saveProjects(testPath, projects, message))
    {
        passed = false;
        cout << "FAILED save projects test" << endl;
    }
    if (!filesystem::exists(testPath))
    {
        passed = false;
        cout << "FAILED save file exists test" << endl;
    }

    // Test loading
    vector<Project> loadedProjects;
    if (!loadProjects(testPath, loadedProjects, message))
    {
        passed = false;
        cout << "FAILED load projects test" << endl;
    }
    if (loadedProjects.size() != 2)
    {
        passed = false;
        cout << "FAILED loaded project count test" << endl;
    }

    // Verify loaded data
    if (loadedProjects.size() >= 2)
    {
        if (loadedProjects[0].getProjectName() != "Project 1")
        {
            passed = false;
            cout << "FAILED project 1 name test" << endl;
        }
        if (loadedProjects[0].getPriority() != 5)
        {
            passed = false;
            cout << "FAILED project 1 priority test" << endl;
        }
        if (loadedProjects[0].getTaskCount() != 2)
        {
            passed = false;
            cout << "FAILED project 1 task count test" << endl;
        }

        Task *task1 = loadedProjects[0].getTask(0);
        if (task1->taskState != TaskState::Done)
        {
            passed = false;
            cout << "FAILED task state persistence test" << endl;
        }

        if (loadedProjects[1].getTaskCount() != 1)
        {
            passed = false;
            cout << "FAILED project 2 task count test" << endl;
        }

        Task *timedTask = loadedProjects[1].getTask(0);
        if (timedTask->getKind() != TaskKind::Timed)
        {
            passed = false;
            cout << "FAILED timed task kind persistence test" << endl;
        }
        if (timedTask->getRemainingSeconds() != 120)
        {
            passed = false;
            cout << "FAILED timed task seconds persistence test" << endl;
        }
    }

    // Cleanup
    filesystem::remove(testPath);

    return passed;
}
