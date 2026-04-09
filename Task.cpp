//
// Created by Nico on 2/20/2026.
//

#include "Task.h"

// Overload for printing TaskState
std::ostream &operator<<(std::ostream &os, TaskState state)
{
    switch (state)
    {
    case TaskState::ToDo:
        os << "ToDo";
        break;
    case TaskState::Doing:
        os << "Doing";
        break;
    case TaskState::Done:
        os << "Done";
        break;
    }
    return os;
}

// Task base class

Task::Task(std::string name)
{
    this->taskName = name;
    this->taskState = TaskState::ToDo;
}

Task::~Task()
{
}

TaskKind Task::getKind() const
{
    return TaskKind::Standard;
}

std::unique_ptr<Task> Task::clone() const
{
    return std::make_unique<Task>(taskName);
}

void Task::startTimer()
{
    // irrelevant
}

void Task::pauseTimer()
{
    // irrelevant
}

void Task::resetTimer()
{
    // irrelevant
}

void Task::syncTimer(std::time_t currentTime)
{
    // irrelavant
}

bool Task::isTimerRunning() const
{
    return false;
}

bool Task::isTimerExpired() const
{
    return false;
}

int Task::getRemainingSeconds() const
{
    return 0;
}
// comparison operator for sorting tasks by state, then by name
bool Task::operator<(const Task &other) const
{
    if (taskState != other.taskState)
    {
        return static_cast<int>(taskState) < static_cast<int>(other.taskState);
    }
    return taskName < other.taskName;
}

// TimedTask implementations (child of Task)

TimedTask::TimedTask(std::string name, int totalSeconds)
    : Task(name), timer(totalSeconds)
{
    this->expiredPromptPending = false;
}

TimedTask::TimedTask(std::string name, int totalSeconds,
                     int remainingSeconds, bool running, std::time_t startedEpoch,
                     bool expiredPromptPending)
    : Task(name), timer(totalSeconds)
{
    this->expiredPromptPending = expiredPromptPending;
}

TaskKind TimedTask::getKind() const
{
    return TaskKind::Timed;
}

std::unique_ptr<Task> TimedTask::clone() const // Clone to create deep copies of tasks (should cite)
{
    auto cloned = std::make_unique<TimedTask>(taskName, timer.getTotalSeconds());
    cloned->taskState = this->taskState;
    cloned->expiredPromptPending = this->expiredPromptPending;
    return cloned;
}

void TimedTask::startTimer()
{
    timer.start();
}

void TimedTask::pauseTimer()
{
    timer.pause();
}

void TimedTask::resetTimer()
{
    timer.reset();
}

void TimedTask::syncTimer(std::time_t currentTime)
{
    timer.sync(currentTime);
    if (timer.isExpired() && !expiredPromptPending)
    {
        expiredPromptPending = true;
    }
}

bool TimedTask::isTimerRunning() const
{
    return timer.isRunning();
}

bool TimedTask::isTimerExpired() const
{
    return timer.isExpired();
}

int TimedTask::getRemainingSeconds() const
{
    return timer.getRemainingSeconds();
}

bool TimedTask::getExpiredPromptPending() const
{
    return expiredPromptPending;
}

void TimedTask::setExpiredPromptPending(bool pending)
{
    this->expiredPromptPending = pending;
}

CountdownTimer &TimedTask::getTimer()
{
    return timer;
}

const CountdownTimer &TimedTask::getTimer() const
{
    return timer;
}
