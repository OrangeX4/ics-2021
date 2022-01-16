#ifndef __TIME_H__
#define __TIME_H__

struct timeval {
    long tv_sec;
    long tv_usec;
};

struct timezone {
    int tz_minuteswest;
    int tz_dsttime;  
};

int my_gettimeofday(struct timeval *tv, struct timezone *tz);

#endif