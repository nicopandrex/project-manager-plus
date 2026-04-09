//
// Created by Nico on 2/20/2026.
//

#ifndef M3OEP_NLIPPIAT_COUNTDOWNTIMER_H
#define M3OEP_NLIPPIAT_COUNTDOWNTIMER_H

#include <ctime>

class CountdownTimer
{
private:
    int originalTotalSeconds;
    int totalSeconds;
    int remainingSeconds;
    bool running;
    std::time_t startedEpoch;

public:
    // Constructor
    CountdownTimer(int totalSeconds);

    // Getters
    int getOriginalTotalSeconds() const;
    int getTotalSeconds() const;
    int getRemainingSeconds() const;
    bool isRunning() const;
    std::time_t getStartedEpoch() const;

    // Setters (for loading from save file)
    void setOriginalTotalSeconds(int seconds);
    void setRemainingSeconds(int seconds);
    void setStartedEpoch(std::time_t epoch);
    void setRunning(bool isRunning);

    // Timer controls
    void start();
    void pause();
    void reset();

    // Sync timer based on current time
    void sync(std::time_t currentTime);

    // Check if timer has expired
    bool isExpired() const;
};

#endif // M3OEP_NLIPPIAT_COUNTDOWNTIMER_H
