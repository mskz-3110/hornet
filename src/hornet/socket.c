#include <hornet.h>

static uint32 s_socket_auto_key = 0;

HornetSocket* hornet_socket_new( HornetPoll* poll, HornetSocketAddress* address ){
  if ( null == address ) return null;
  HornetSocket* socket = (HornetSocket*)C_MEMORY_ALLOCATE( sizeof( HornetSocket ) );
  do{
    if ( null == socket ) break;
    
    socket->poll = poll;
    socket->self_address = address;
    socket->peer_address = null;
    hornet_lock();
    socket->key = ++s_socket_auto_key;
    hornet_unlock();
    socket->data = null;
    
    // TODO
    hornet_io_set_event_callback( &(socket->io), null );
    hornet_io_set_data( &(socket->io), null );
    
    return socket;
  }while ( 0 );
  hornet_socket_delete( socket );
  return null;
}

void hornet_socket_delete( HornetSocket* socket ){
  if ( null != socket ){
    if ( null != socket->self_address ){
      hornet_socket_address_delete( socket->self_address );
      socket->self_address = null;
    }
    
    if ( null != socket->peer_address ){
      hornet_socket_address_delete( socket->peer_address );
      socket->peer_address = null;
    }
    
    C_MEMORY_FREE( socket );
  }
}

void hornet_socket_set_peer_address( HornetSocket* socket, HornetSocketAddress* address ){
  socket->peer_address = address;
}

HornetSocketAddress* hornet_socket_get_peer_address( HornetSocket* socket ){
  return socket->peer_address;
}

HornetIO* hornet_socket_get_io( HornetSocket* socket ){
  return &(socket->io);
}

uint32 hornet_socket_get_key( HornetSocket* socket ){
  return socket->key;
}

void hornet_socket_set_event_callback( HornetSocket* socket, HornetSocketEventCallback event_callback ){
  socket->event_callback = event_callback;
}

void hornet_socket_call_event( HornetSocket* socket, int32 events ){
  if ( null != socket->event_callback ) socket->event_callback( socket, events );
}

void hornet_socket_set_data( HornetSocket* socket, void* data ){
  socket->data = data;
}

void* hornet_socket_get_data( HornetSocket* socket ){
  return socket->data;
}

bool hornet_socket_standalone( HornetSocket* socket ){
  // TODO
  return false;
}

bool hornet_socket_standby( HornetSocket* socket ){
  // TODO
  return false;
}

bool hornet_socket_connect( HornetSocket* socket, float timeout ){
  // TODO
  return false;
}

HornetSocket* hornet_socket_accept( HornetSocket* socket ){
  // TODO
  return null;
}

bool hornet_socket_set_write_buffer_size( HornetSocket* socket, int32 write_buffer_size ){
  // TODO
  return false;
}

int32 hornet_socket_get_write_buffer_size( HornetSocket* socket ){
  // TODO
  return 0;
}

bool hornet_socket_set_read_buffer_size( HornetSocket* socket, int32 read_buffer_size ){
  // TODO
  return false;
}

int32 hornet_socket_get_read_buffer_size( HornetSocket* socket ){
  // TODO
  return 0;
}

bool hornet_socket_set_is_broadcast( HornetSocket* socket, bool is_broadcast ){
  // TODO
  return false;
}

bool hornet_socket_get_is_broadcast( HornetSocket* socket ){
  // TODO
  return false;
}

bool hornet_socket_write( HornetSocket* socket, void* data, int32 data_size ){
  // TODO
  return false;
}

int32 hornet_socket_read( HornetSocket* socket, void* data, int32 data_size ){
  // TODO
  return -1;
}
