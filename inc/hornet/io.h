#ifndef __HORNET_IO_H__
#define __HORNET_IO_H__

struct HornetIO;

typedef enum {
  HORNET_IO_EVENT_NONE     = 0,
  HORNET_IO_EVENT_READABLE = 1,
  HORNET_IO_EVENT_WRITABLE = 2,
} HornetIOEvent;

typedef void (*HornetIOEventCallback)( struct HornetIO* io, int32 events );

typedef struct HornetIO {
#ifdef C_PLATFORM_WINDOWS
  
#else
  int32 value;
#endif
  HornetIOEventCallback event_callback;
  void*                 data;
} HornetIO;

C_API_BEGIN
  C_API void hornet_io_set_event_callback( HornetIO* io, HornetIOEventCallback event_callback );
  
  C_API void hornet_io_call_event( HornetIO* io, int32 events );
  
  C_API void hornet_io_set_data( HornetIO* io, void* data );
  
  C_API void* hornet_io_get_data( HornetIO* io );
C_API_END

#endif
