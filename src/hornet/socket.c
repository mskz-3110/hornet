#include <hornet.h>
#include <netdb.h>
#include <sys/types.h>

extern HornetSocketAddress* hornet_socket_address_new( struct addrinfo* info );

static void on_socket_event( HornetIO* io, int32 events ){
  HornetSocket* socket = (HornetSocket*)io->data;
  if ( null != socket->event_callback ) socket->event_callback( socket, events );
}

HornetSocket* hornet_socket_new( HornetPoll* poll, HornetSocketAddress* address ){
  if ( null == address ) return null;
  HornetSocket* _socket = (HornetSocket*)C_MEMORY_ALLOCATE( sizeof( HornetSocket ) );
  do{
    if ( null == _socket ) break;
    
    _socket->poll = poll;
    _socket->address = hornet_socket_address_clone( address );
    _socket->peer_address = hornet_socket_address_clone( address );
    _socket->errmsg = hornet_string_new( 0 );
    _socket->data = null;
#ifdef C_PLATFORM_WINDOWS
    // TODO
#else
    _socket->io.value = -1;
#endif
    
    if ( null == _socket->peer_address ) break;
    if ( null == _socket->errmsg ) break;
    
#ifdef C_PLATFORM_WINDOWS
    // TODO
#else
    _socket->io.value = socket( address->address.base.sa_family, address->type, 0 );
    if ( _socket->io.value < 0 ){
      hornet_string_add_errmsg( _socket->errmsg );
      break;
    }
    
    if ( ! hornet_io_set_flags( &(_socket->io), hornet_io_get_flags( &(_socket->io) ) | O_NONBLOCK | O_CLOEXEC ) ) break;
    
#ifdef C_PLATFORM_APPLE
    int32 flags = 1;
    if ( 0 != setsockopt( _socket->io.value, SOL_SOCKET, SO_NOSIGPIPE, &flags, sizeof( flags ) ) ) break;
#endif
#endif
    
    hornet_io_set_event_callback( &(_socket->io), null );
    hornet_io_set_data( &(_socket->io), _socket );
    
    return _socket;
  }while ( 0 );
  hornet_socket_delete( _socket );
  return null;
}

void hornet_socket_delete( HornetSocket* socket ){
  if ( null != socket ){
    hornet_socket_address_delete( socket->address );
    hornet_socket_address_delete( socket->peer_address );
    hornet_string_delete( socket->errmsg );
#ifdef C_PLATFORM_WINDOWS
    // TODO
#else
    if ( 0 <= socket->io.value ){
      close( socket->io.value );
    }
#endif
    C_MEMORY_FREE( socket );
  }
}

void hornet_socket_set_peer_address( HornetSocket* socket, HornetSocketAddress* address ){
  socket->peer_address = address;
}

HornetSocketAddress* hornet_socket_get_peer_address( HornetSocket* socket ){
  return socket->peer_address;
}

string hornet_socket_get_errmsg( HornetSocket* socket ){
  return hornet_string_get_string( socket->errmsg );
}

HornetIO* hornet_socket_get_io( HornetSocket* socket ){
  return &(socket->io);
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
  hornet_io_set_event_callback( &(socket->io), on_socket_event );
  return hornet_poll_set( socket->poll, &(socket->io), HORNET_IO_EVENT_READABLE );
}

bool hornet_socket_standby( HornetSocket* socket ){
  do{
#ifdef C_PLATFORM_WINDOWS
    break; // TODO
#else
    if ( bind( socket->io.value, &(socket->address->address.base), socket->address->address_size ) < 0 ){
      hornet_string_clear( socket->errmsg );
      hornet_string_add_errmsg( socket->errmsg );
      break;
    }
#endif
    
    return hornet_socket_standalone( socket );
  }while ( 0 );
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
  return ( 0 == setsockopt( socket->io.value, SOL_SOCKET, SO_SNDBUF, &write_buffer_size, sizeof( write_buffer_size ) ) );
}

int32 hornet_socket_get_write_buffer_size( HornetSocket* socket ){
  int32 value = -1;
  socklen_t value_size = sizeof( value );
  getsockopt( socket->io.value, SOL_SOCKET, SO_SNDBUF, &value, &value_size );
  return value;
}

bool hornet_socket_set_read_buffer_size( HornetSocket* socket, int32 read_buffer_size ){
  return ( 0 == setsockopt( socket->io.value, SOL_SOCKET, SO_RCVBUF, &read_buffer_size, sizeof( read_buffer_size ) ) );
}

int32 hornet_socket_get_read_buffer_size( HornetSocket* socket ){
  int32 value = -1;
  socklen_t value_size = sizeof( value );
  getsockopt( socket->io.value, SOL_SOCKET, SO_RCVBUF, &value, &value_size );
  return value;
}

bool hornet_socket_set_is_broadcast( HornetSocket* socket, bool is_broadcast ){
  int32 value = is_broadcast;
  return ( 0 == setsockopt( socket->io.value, SOL_SOCKET, SO_BROADCAST, &value, sizeof( value ) ) );
}

bool hornet_socket_get_is_broadcast( HornetSocket* socket ){
  int32 value = 0;
  socklen_t value_size = sizeof( value );
  getsockopt( socket->io.value, SOL_SOCKET, SO_BROADCAST, &value, &value_size );
  return ( 0 != value );
}

bool hornet_socket_write( HornetSocket* socket, void* data, int32 data_size ){
  switch ( socket->address->type ){
  case SOCK_DGRAM:{
    return ( data_size == sendto( socket->io.value, data, data_size, 0, &(socket->peer_address->address.base), socket->peer_address->address_size ) );
  }
  case SOCK_STREAM:{
    int32 flags = 0;
#ifdef C_PLATFORM_LINUX
    flags = MSG_NOSIGNAL;
#endif
    // TODO
    return ( data_size == send( socket->io.value, data, data_size, flags ) );
  }
  default: return false;
  }
}

int32 hornet_socket_read( HornetSocket* socket, void* data, int32 data_size ){
  switch ( socket->address->type ){
  case SOCK_DGRAM:{
    socket->peer_address->address_size = sizeof( socket->peer_address->address );
    return recvfrom( socket->io.value, data, data_size, 0, &(socket->peer_address->address.base), (socklen_t*)&socket->peer_address->address_size );
  }
  case SOCK_STREAM:{
    return recv( socket->io.value, data, data_size, 0 );
  }
  default: return -1;
  }
}

void hornet_socket_to_string( HornetString* string, HornetSocket* socket ){
  hornet_socket_address_to_string( string, socket->address );
  hornet_string_add_string( string, " -> ", null );
  hornet_socket_address_to_string( string, socket->peer_address );
  hornet_string_add_int32( string, " w=%d", hornet_socket_get_write_buffer_size( socket ) );
  hornet_string_add_int32( string, " r=%d", hornet_socket_get_read_buffer_size( socket ) );
  switch ( socket->address->type ){
  case SOCK_DGRAM:{
    hornet_string_add_int32( string, " b=%d", hornet_socket_get_is_broadcast( socket ) );
  }break;
  case SOCK_STREAM:{
    // TODO
  }break;
  default: break;
  }
}
