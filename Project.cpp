//
// Created by Nico on 1/26/2026.
//
#include <iostream>
#include <string>
#include <vector>
#include <ostream>
#include <algorithm>
#include "Project.h"
using namespace std;
using std::string;
using std::vector;

// Overload for printing ProjectStatus
std::ostream &operator<<(std::ostream &os, ProjectStatus state)
{
    switch (state)
    {
    case ProjectStatus::Planned:
        os << "Planned";
        break;
    case ProjectStatus::Active:
        os << "Active";
        break;
    case ProjectStatus::Paused:
        os << "Paused";
        break;
    case ProjectStatus::Completed:
        os << "Completed";
        break;
    }
    return os;
}

// Constructors
Project::Project(std::string projectName)
{
    this->projectName = projectName;
    this->projectState = ProjectStatus::Planned;
    this->priority = 5;
}

// Project Getters
string Project::getProjectName() const
{
    return this->projectName;
}

ProjectStatus Project::getProjectState() const
{
    return this->projectState;
}

int Project::getPriority() const
{
    return this->priority;
}

int Project::getTaskCount() const
{
    return static_cast<int>(this->tasks.size());
}

// seters
void Project::setName(std::string name)
{
    this->projectName = name;
}

void Project::setState(ProjectStatus state)
{
    this->projectState = state;
}

void Project::setPriority(int priority)
{
    if (priority > 0 && priority < 6)
    {
        this->priority = priority;
    }
}

// Custom Project Functions

void Project::addTask(std::string taskName)
{
    std::unique_ptr<Task> task(new Task(taskName));
    this->tasks.push_back(std::move(task));
}

void Project::addTimedTask(std::string taskName, int totalSeconds)
{
    std::unique_ptr<TimedTask> task(new TimedTask(taskName, totalSeconds));
    this->tasks.push_back(std::move(task));
}
// learned task erase online (need to cite)
void Project::removeTask(std::size_t index)
{
    if (index < this->tasks.size())
    {
        this->tasks.erase(this->tasks.begin() + index);
    }
}

void Project::changeTaskState(std::size_t index, TaskState newState)
{
    if (index < this->tasks.size())
    {
        this->tasks[index]->taskState = newState;
    }
}

void Project::printTasksWithIndices(std::ostream &out) const
{
    if (tasks.empty())
    {
        out << "No tasks yet.\n";
        return;
    }

    for (std::size_t i = 0; i < tasks.size(); i++)
    {
        out << "  [" << (i + 1) << "] "
            << tasks[i]->taskName << " | "
            << tasks[i]->taskState;

        // Show timer info for timed tasks
        if (tasks[i]->getKind() == TaskKind::Timed)
        {
            out << " | Timer: " << tasks[i]->getRemainingSeconds() << "s";
            if (tasks[i]->isTimerRunning())
            {
                out << " (running)";
            }
        }

        out << "\n";
    }
}

Task *Project::getTask(std::size_t index) const
{
    if (index < this->tasks.size())
    {
        return this->tasks[index].get();
    }
    return nullptr;
}

// Timed task controls
void Project::startTaskTimer(std::size_t index)
{
    if (index < this->tasks.size())
    {
        this->tasks[index]->startTimer();
    }
}

void Project::pauseTaskTimer(std::size_t index)
{
    if (index < this->tasks.size())
    {
        this->tasks[index]->pauseTimer();
    }
}

void Project::resetTaskTimer(std::size_t index)
{
    if (index < this->tasks.size())
    {
        this->tasks[index]->resetTimer();
    }
}

void Project::syncTimers(std::time_t currentTime)
{
    for (auto &task : this->tasks)
    {
        task->syncTimer(currentTime);
    }
}
