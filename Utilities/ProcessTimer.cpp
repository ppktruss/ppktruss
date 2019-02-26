#include "ProcessTimer.h"
#include <chrono>

using namespace std;

ProcessTimer::ProcessTimer() {
    reset();
}

ProcessTimer::ProcessTimer(string processName) {
    ProcessTimer();
    this->processName = processName;
}

void ProcessTimer::reset() {
    interval = TimeIntervalType::zero();
    status = STATUS::NOT_STARTED;
}

void ProcessTimer::start() {
    startTime = chrono::high_resolution_clock::now();
    status = STATUS::RUNNING;
}

void ProcessTimer::pause() {
    if (status != STATUS::RUNNING)
        return;
    updateInterval();
    status = STATUS::PAUSED;
}

void ProcessTimer::end() {
    if (status == STATUS::RUNNING)
        updateInterval();
    status = STATUS::ENDED;
}

void ProcessTimer::updateInterval() {
    endTime = chrono::high_resolution_clock::now();
    TimeIntervalType elapsed = endTime - startTime;
    interval += elapsed;
}

ProcessTimer::STATUS ProcessTimer::getStatus() const {
    return status;
}

ProcessTimer::TimeIntervalType ProcessTimer::getDuration() const {
    return interval;
}

string ProcessTimer::getProcessName() const {
    return processName;
}

void ProcessTimer::setProcessName(string processName) {
    this->processName = processName;
}
