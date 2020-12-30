// [cc:namespace] Hornet
// [cc:class_name] Poll
#ifndef __HORNET_POLL_H__
#define __HORNET_POLL_H__

#include <hornet/io.h>

#ifdef C_PLATFORM_APPLE
  #include <sys/event.h>
#endif

typedef struct {
  int32          poll_size;
#ifdef C_PLATFORM_APPLE
  int32          kqueue;
  struct kevent* kevents;
#endif
} HornetPoll;

C_API_BEGIN
  // [cc:api] IntPtr hornet_poll_new( int poll_size )
  C_API HornetPoll* hornet_poll_new( int32 poll_size );
  
  // [cc:api] void hornet_poll_delete( IntPtr poll )
  C_API void hornet_poll_delete( HornetPoll* poll );
  
  // [cc:api] bool hornet_poll_set( IntPtr poll, IntPtr io, int events )
  C_API bool hornet_poll_set( HornetPoll* poll, HornetIO* io, int32 events );
  
  // [cc:api] void hornet_poll_unset( IntPtr poll, IntPtr io )
  C_API void hornet_poll_unset( HornetPoll* poll, HornetIO* io );
  
  // [cc:api] bool hornet_poll_update( IntPtr poll )
  C_API bool hornet_poll_update( HornetPoll* poll );
C_API_END

#endif
