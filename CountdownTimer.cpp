//
// Created by Nico on 2/20/2026.
//

#include "CountdownTimer.h"

// Constructor
CountdownTimer::CountdownTimer(int totalSeconds)
{
    this->originalTotalSeconds = totalSeconds;
    this->totalSeconds = totalSeconds;
    this->remainingSeconds = totalSeconds;
    this->running = false;
    this->startedEpoch = 0;
}

// Getters
int CountdownTimer::getOriginalTotalSeconds() const
{
    return this->originalTotalSeconds;
}

int CountdownTimer::getTotalSeconds() const
{
    return this->totalSeconds;
}

int CountdownTimer::getRemainingSeconds() const
{
    return this->remainingSeconds;
}

bool CountdownTimer::isRunning() const
{
    return this->running;
}

std::time_t CountdownTimer::getStartedEpoch() const
{
    return this->startedEpoch;
}

// Start the timer
void CountdownTimer::start()
{
    if (!this->running && this->remainingSeconds > 0)
    {
        this->running = true;
        this->startedEpoch = std::time(nullptr);
    }
}

// Pause the timer
void CountdownTimer::pause()
{
    if (this->running)
    {
        // Sync before pausing to update remainingSeconds
        sync(std::time(nullptr));
        this->running = false;
        this->startedEpoch = 0;
    }
}

// Reset the timer to original duration
void CountdownTimer::reset()
{
    this->totalSeconds = this->originalTotalSeconds;
    this->remainingSeconds = this->originalTotalSeconds;
    this->running = false;
    this->startedEpoch = 0;
}

// Sync timer based on current time
void CountdownTimer::sync(std::time_t currentTime)
{
    if (this->running && this->startedEpoch > 0)
    {
        int elapsed = static_cast<int>(currentTime - this->startedEpoch);
        this->remainingSeconds = this->totalSeconds - elapsed;

        // If timer expired, stop it
        if (this->remainingSeconds <= 0)
        {
            this->remainingSeconds = 0;
            this->running = false;
        }

        // Update startedEpoch to current time for next sync
        this->startedEpoch = currentTime;
        this->totalSeconds = this->remainingSeconds;
    }
}

// Check if timer has expired
bool CountdownTimer::isExpired() const
{
    return this->remainingSeconds <= 0;
}

// Setters (for loading from save file)
void CountdownTimer::setRemainingSeconds(int seconds)
{
    this->remainingSeconds = seconds;
    this->totalSeconds = seconds;
}

void CountdownTimer::setOriginalTotalSeconds(int seconds)
{
    this->originalTotalSeconds = seconds;
}

void CountdownTimer::setStartedEpoch(std::time_t epoch)
{
    this->startedEpoch = epoch;
}

void CountdownTimer::setRunning(bool isRunning)
{
    this->running = isRunning;
}
