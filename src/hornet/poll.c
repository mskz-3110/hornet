#include <hornet.h>

HornetPoll* hornet_poll_new(){
  HornetPoll* poll = (HornetPoll*)C_MEMORY_ALLOCATE( sizeof( HornetPoll ) );
  do{
    if ( null == poll ) break;
    
    // TODO
    
    return poll;
  }while ( 0 );
  hornet_poll_delete( poll );
  return null;
}

void hornet_poll_delete( HornetPoll* poll ){
  if ( null != poll ){
    C_MEMORY_FREE( poll );
  }
}

bool hornet_poll_set( HornetPoll* poll, HornetIO* io ){
  // TODO
  return false;
}

void hornet_poll_unset( HornetPoll* poll, HornetIO* io ){
  // TODO
}

bool hornet_poll_update( HornetPoll* poll ){
  // TODO
  return false;
}
