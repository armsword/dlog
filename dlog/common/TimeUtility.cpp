#include "TimeUtility.h"

DLOG_BEGIN_NAMESPACE(common);

int64_t TimeUtility::currentTime() {
    struct timeval tval;
    gettimeofday(&tval, NULL);
    return (tval.tv_sec * 1000000LL + tval.tv_usec);
}

int64_t TimeUtility::currentTimeInNanoSeconds() {
    return currentTime() * 1000;
}

int64_t TimeUtility::currentTimeInSeconds() {
    return currentTime() / 1000000;
}

int64_t TimeUtility::currentTimeInMicroSeconds() {
    return currentTime();
}

int64_t TimeUtility::getTime(int64_t usecOffset) {
    return currentTime() + usecOffset;
}

timeval TimeUtility::getTimeval(int64_t usecOffset) {
    timeval tval;
    int64_t uTime = getTime(usecOffset);
    tval.tv_sec = uTime / 1000000;
    tval.tv_usec = uTime % 1000000;
    return tval;
}

timespec TimeUtility::getTimespec(int64_t usecOffset) {
    timespec tspec;
    int64_t uTime = getTime(usecOffset);
    tspec.tv_sec = uTime / 1000000;
    tspec.tv_nsec = (uTime % 1000000) * 1000;
    return tspec;
}

std::string TimeUtility::currentTimeString() {
    time_t lt = time(NULL);
    struct tm tim;
    localtime_r(&lt, &tim);
    char str[200];
    strftime(str, sizeof(str), "%Y%m%d%H%M%S", &tim);
    return std::string(str);
}

std::string TimeUtility::currentDay() {
    time_t lt = time(NULL);
    struct tm tim;
    localtime_r(&lt, &tim);
    char str[200];
    strftime(str, sizeof(str), "%Y%m%d", &tim);
    return std::string(str);
}

uint32_t TimeUtility::getCurTime(char cur[], int length) {
    struct timeval t;
    gettimeofday(&t, NULL);
    struct tm tim;
    ::localtime_r(&t.tv_sec, &tim);
    return snprintf(cur, length, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                    tim.tm_year + 1900, tim.tm_mon + 1, tim.tm_mday,
                    tim.tm_hour, tim.tm_min, tim.tm_sec, (int)t.tv_usec / 1000);
}

DLOG_END_NAMESPACE(common);
