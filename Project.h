//
// Created by Nico on 1/26/2026.
//

#ifndef M3OEP_NLIPPIAT_PROJECT_H
#define M3OEP_NLIPPIAT_PROJECT_H
#include <string>
#include <vector>
#include <ostream>
#include <memory>
#include "Task.h"

enum class ProjectStatus
{
    Planned,
    Active,
    Paused,
    Completed
};

std::ostream &operator<<(std::ostream &os, ProjectStatus state);

class Project
{
private:
    std::string projectName;
    std::vector<std::unique_ptr<Task>> tasks;
    ProjectStatus projectState;
    int priority;

public:
    // Constructors
    Project(std::string projectName);

    // Getters
    std::string getProjectName() const;
    ProjectStatus getProjectState() const;
    int getPriority() const;
    int getTaskCount() const;

    // Setters
    void setName(std::string name);
    void setState(ProjectStatus state);
    void setPriority(int priority);

    // custom functions
    void addTask(std::string taskName);
    void addTimedTask(std::string taskName, int totalSeconds);
    void removeTask(std::size_t index);
    void changeTaskState(std::size_t index, TaskState newState);
    void printTasksWithIndices(std::ostream &out) const;
    Task *getTask(std::size_t index) const;

    // Timed task controls
    void startTaskTimer(std::size_t index);
    void pauseTaskTimer(std::size_t index);
    void resetTaskTimer(std::size_t index);
    void syncTimers(std::time_t currentTime);
};

#endif // M3OEP_NLIPPIAT_PROJECT_H