#include <hornet.h>

HornetPoll* hornet_poll_new( int32 poll_size ){
  HornetPoll* poll = (HornetPoll*)C_MEMORY_ALLOCATE( sizeof( HornetPoll ) );
  do{
    if ( null == poll ) break;
    
    poll->poll_size = poll_size;
#ifdef C_PLATFORM_APPLE
    poll->kqueue = kqueue();
    poll->kevents = (struct kevent*)C_MEMORY_ALLOCATE( sizeof( struct kevent ) * poll_size );
    
    if ( poll->kqueue < 0 ) break;
    if ( null == poll->kevents ) break;
#else
    // TODO
#endif
    
    return poll;
  }while ( 0 );
  hornet_poll_delete( poll );
  return null;
}

void hornet_poll_delete( HornetPoll* poll ){
  if ( null != poll ){
#ifdef C_PLATFORM_APPLE
    if ( 0 <= poll->kqueue ) close( poll->kqueue );
#else
    // TODO
#endif
    C_MEMORY_FREE( poll );
  }
}

bool hornet_poll_set( HornetPoll* poll, HornetIO* io, int32 events ){
  hornet_poll_unset( poll, io );
#ifdef C_PLATFORM_APPLE
  struct kevent event;
  if ( 0 != (events & HORNET_IO_EVENT_READABLE) ){
    EV_SET( &event, io->value, EVFILT_READ, EV_ADD, 0, 0, io );
    kevent( poll->kqueue, &event, 1, null, 0, null );
  }
  if ( 0 != (events & HORNET_IO_EVENT_WRITABLE) ){
    EV_SET( &event, io->value, EVFILT_WRITE, EV_ADD, 0, 0, io );
    kevent( poll->kqueue, &event, 1, null, 0, null );
  }
  return true;
#else
  return false; // TODO
#endif
}

void hornet_poll_unset( HornetPoll* poll, HornetIO* io ){
#ifdef C_PLATFORM_APPLE
  struct kevent event;
  EV_SET( &event, io->value, EVFILT_READ, EV_DELETE, 0, 0, null );
  kevent( poll->kqueue, &event, 1, null, 0, null );
  EV_SET( &event, io->value, EVFILT_WRITE, EV_DELETE, 0, 0, null );
  kevent( poll->kqueue, &event, 1, null, 0, null );
#else
  // TODO
#endif
}

bool hornet_poll_update( HornetPoll* poll ){
#ifdef C_PLATFORM_APPLE
  struct timespec timeout;
  int32 count;
  do{
    timeout.tv_sec  = 0;
    timeout.tv_nsec = 0;
    count = kevent( poll->kqueue, null, 0, poll->kevents, poll->poll_size, &timeout );
    for ( int32 i = 0; i < count; ++i ){
      struct kevent* event = &(poll->kevents[ i ]);
      HornetIO* io = (HornetIO*)event->udata;
      if ( event->filter == EVFILT_READ ){
        io->event_callback( io, HORNET_IO_EVENT_READABLE );
      }else if ( event->filter == EVFILT_WRITE ){
        io->event_callback( io, HORNET_IO_EVENT_WRITABLE );
      }
    }
  }while ( poll->poll_size == count );
  return ( 0 < count );
#else
  return false; // TODO
#endif
}
