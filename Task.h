//
// Created by Nico on 2/20/2026.
//

#ifndef M3OEP_NLIPPIAT_TASK_H
#define M3OEP_NLIPPIAT_TASK_H

#include <string>
#include <ostream>
#include <memory>
#include "CountdownTimer.h"

enum class TaskState
{
    ToDo,
    Doing,
    Done
};
enum class TaskKind
{
    Standard,
    Timed
};

std::ostream &operator<<(std::ostream &os, TaskState state);

class Task
{
public:
    std::string taskName;
    TaskState taskState;

    // Constructor
    Task(std::string name);

    // destructor
    virtual ~Task();

    // Virtual methods for polymorphism
    virtual TaskKind getKind() const;
    virtual std::unique_ptr<Task> clone() const;

    // Timer-related virtual methods (only for timed tasks)
    virtual void startTimer();
    virtual void pauseTimer();
    virtual void resetTimer();
    virtual void syncTimer(std::time_t currentTime);
    virtual bool isTimerRunning() const;
    virtual bool isTimerExpired() const;
    virtual int getRemainingSeconds() const;

    // Comparison operator
    bool operator<(const Task &other) const;
};

class TimedTask : public Task
{
private:
    CountdownTimer timer;
    bool expiredPromptPending;

public:
    // Constructor
    TimedTask(std::string name, int totalSeconds);

    // Constructor for loading from file
    TimedTask(std::string name, int totalSeconds,
              int remainingSeconds, bool running, std::time_t startedEpoch,
              bool expiredPromptPending);

    // Override virtual methods
    TaskKind getKind() const override;
    std::unique_ptr<Task> clone() const override;

    // Timer controls
    void startTimer() override;
    void pauseTimer() override;
    void resetTimer() override;
    void syncTimer(std::time_t currentTime) override;
    bool isTimerRunning() const override;
    bool isTimerExpired() const override;
    int getRemainingSeconds() const override;

    // Timed task specific methods
    bool getExpiredPromptPending() const;
    void setExpiredPromptPending(bool pending);
    CountdownTimer &getTimer();
    const CountdownTimer &getTimer() const;
};

#endif // M3OEP_NLIPPIAT_TASK_H
