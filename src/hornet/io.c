#include <hornet.h>

void hornet_io_set_event_callback( HornetIO* io, HornetIOEventCallback callback ){
  io->event_callback = callback;
}

void hornet_io_call_event( HornetIO* io, int32 events ){
  if ( null != io->event_callback ) io->event_callback( io, events );
}

void hornet_io_set_data( HornetIO* io, void* data ){
  io->data = data;
}

void* hornet_io_get_data( HornetIO* io ){
  return io->data;
}

int32 hornet_io_get_flags( HornetIO* io ){
  return fcntl( io->value, F_GETFL, 0 );
}

bool hornet_io_set_flags( HornetIO* io, int32 flags ){
  return ( 0 <= fcntl( io->value, F_SETFL, flags ) );
}
