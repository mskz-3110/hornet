#include <hornet.h>
#include <sys/time.h>

double hornet_time_now(){
  int64 seconds;
  int32 usec;
#ifdef C_PLATFORM_WINDOWS
  struct timespec ts;
  timespec_get( &ts, TIME_UTC );
  seconds = (int64)ts.tv_sec;
  usec = (int32)(ts.tv_nsec / 1000);
#else
  struct timeval tv;
  gettimeofday( &tv, null );
  seconds = (int64)tv.tv_sec;
  usec = (int32)tv.tv_usec;
#endif
  return (double)seconds + (double)usec / 1000000;
}

void hornet_sleep( int32 msec ){
#ifdef C_PLATFORM_WINDOWS
  Sleep( msec );
#else
  struct timespec req;
  req.tv_sec = msec / 1000;
  req.tv_nsec = (msec % 1000) * 1000000;
  nanosleep( &req, null );
#endif
}
