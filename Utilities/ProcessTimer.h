
#include <string>
#include <chrono>

#ifndef KTRUSS_V1_PROCESSTIMER_H
#define KTRUSS_V1_PROCESSTIMER_H

class ProcessTimer {
public:
    enum STATUS {
        NOT_STARTED = 0, RUNNING = 1, PAUSED = 2, ENDED = 3,
    };
    typedef std::chrono::time_point <std::chrono::high_resolution_clock> TimePointType;
    typedef std::chrono::duration<double> TimeIntervalType;

    ProcessTimer();
    ProcessTimer(std::string processName);

    void start();
    void pause();
    void end();
    void reset();
    TimeIntervalType getDuration() const;
    STATUS getStatus() const;
    std::string getProcessName() const;
    void setProcessName(std::string);

private:
    void updateInterval();

    STATUS status;
    TimePointType startTime;
    TimePointType endTime;
    TimeIntervalType interval;
    std::string processName;
};

#endif //KTRUSS_V1_PROCESSTIMER_H
