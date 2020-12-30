#include <hornet.h>
#include <netdb.h>
#include <arpa/inet.h>

static void socket_address_resolve_result_delete( HornetSocketAddressResolveResult* result ){
  if ( null != result ){
    hornet_string_delete( result->protocol );
    hornet_string_delete( result->host );
    hornet_string_delete( result->port );
    hornet_string_delete( result->path );
    hornet_string_delete( result->errmsg );
    if ( null != result->addresses ){
      for ( int32 i = 0; i < result->addresses_count; ++i ){
        hornet_socket_address_delete( result->addresses[ i ] );
      }
      C_MEMORY_FREE( result->addresses );
    }
    C_MEMORY_FREE( result );
  }
}

static HornetSocketAddressResolveResult* socket_address_resolve_result_new( string url ){
  HornetSocketAddressResolveResult* result = (HornetSocketAddressResolveResult*)C_MEMORY_ALLOCATE( sizeof( HornetSocketAddressResolveResult ) );
  do{
    if ( null == result ) break;
    
    result->protocol = hornet_string_new( 0 );
    result->host = hornet_string_new( 0 );
    result->port = hornet_string_new( 0 );
    result->path = hornet_string_new( 0 );
    result->errmsg = hornet_string_new( 0 );
    result->addresses_count = 0;
    result->addresses = null;
    
    if ( null == result->protocol ) break;
    if ( null == result->host ) break;
    if ( null == result->port ) break;
    if ( null == result->path ) break;
    if ( null == result->errmsg ) break;
    
    char format[ 8 ];
    HornetStringParseResult parse_result;
    parse_result = hornet_string_split_once( url, "://" );
    snprintf( format, sizeof( format ), "%%.%ds", parse_result.left_length );
    hornet_string_add_string( result->protocol, format, parse_result.left_string );
    hornet_string_set_lower( result->protocol, 0, parse_result.left_length );
    parse_result = hornet_string_split_once( parse_result.right_string, ";" );
    snprintf( format, sizeof( format ), "%%.%ds", parse_result.left_length );
    hornet_string_add_string( result->host, format, parse_result.left_string );
    parse_result = hornet_string_split_once( parse_result.right_string, "/" );
    snprintf( format, sizeof( format ), "%%.%ds", parse_result.left_length );
    hornet_string_add_string( result->port, format, parse_result.left_string );
    snprintf( format, sizeof( format ), "%%.%ds", parse_result.right_length );
    hornet_string_add_string( result->path, format, parse_result.right_string );
    
    return result;
  }while ( 0 );
  socket_address_resolve_result_delete( result );
  return null;
}

HornetSocketAddress* hornet_socket_address_new( struct addrinfo* info ){
  HornetSocketAddress* address = (HornetSocketAddress*)C_MEMORY_ALLOCATE( sizeof( HornetSocketAddress ) );
  do{
    if ( null == address ) break;
    
    if ( null != info ){
      address->type = info->ai_socktype;
      address->address_size = info->ai_addrlen;
      memcpy( &(address->address.base), info->ai_addr, address->address_size );
    }else{
      address->type = 0;
      address->address_size = sizeof( address->address );
    }
    address->string = hornet_string_new( 0 );
    
    if ( null == address->string ) break;
    
    return address;
  }while ( 0 );
  hornet_socket_address_delete( address );
  return null;
}

HornetSocketAddress* hornet_socket_address_clone( HornetSocketAddress* address ){
  if ( null == address ) return null;
  HornetSocketAddress* cloned_address = (HornetSocketAddress*)C_MEMORY_ALLOCATE( sizeof( HornetSocketAddress ) );
  do {
    if ( null == cloned_address ) break;
    
    cloned_address->type = address->type;
    memcpy( &(cloned_address->address), &(address->address), address->address_size );
    cloned_address->address_size = address->address_size;
    cloned_address->string = hornet_string_clone( address->string );
    
    return cloned_address;
  }while ( 0 );
  hornet_socket_address_delete( cloned_address );
  return null;
}

void hornet_socket_address_delete( HornetSocketAddress* address ){
  if ( null != address ){
    hornet_string_delete( address->string );
    C_MEMORY_FREE( address );
  }
}

void hornet_socket_address_set( HornetSocketAddress* _address, HornetSocketAddressType type, void* address, int32 address_size ){
  switch ( type ){
  case HORNET_SOCKET_ADDRESS_TYPE_UDP:{ _address->type = SOCK_DGRAM; }break;
  case HORNET_SOCKET_ADDRESS_TYPE_TCP:{ _address->type = SOCK_STREAM; }break;
  default:{ _address->type = 0; }break;
  }
  if ( sizeof( _address->address ) < address_size ){
    address_size = sizeof( _address->address );
  }else if ( address_size < 0 ){
    address_size = 0;
  }
  if ( 0 < address_size ) memcpy( &(_address->address), address, address_size );
}

HornetSocketAddressType hornet_socket_address_get_type( HornetSocketAddress* address ){
  switch ( address->type ){
  case SOCK_DGRAM: return HORNET_SOCKET_ADDRESS_TYPE_UDP;
  case SOCK_STREAM: return HORNET_SOCKET_ADDRESS_TYPE_TCP;
  default: return HORNET_SOCKET_ADDRESS_TYPE_NONE;
  }
}

void* hornet_socket_address_get_address( HornetSocketAddress* address ){
  return &(address->address);
}

int32 hornet_socket_address_get_address_size( HornetSocketAddress* address ){
  return address->address_size;
}

void hornet_socket_address_to_string( HornetString* string, HornetSocketAddress* address ){
  if ( 0 == hornet_string_get_length( address->string ) ){
    switch ( address->type ){
    case SOCK_DGRAM:{
      hornet_string_add_string( address->string, "%s://", "udp" );
    }break;
    case SOCK_STREAM:{
      hornet_string_add_string( address->string, "%s://", "tcp" );
    }break;
    default: return;
    }
    
    switch ( address->address.base.sa_family ){
    case PF_INET:{
      char ip[ INET_ADDRSTRLEN ];
      inet_ntop( address->address.base.sa_family, &(address->address.ipv4.sin_addr), ip, sizeof( ip ) );
      hornet_string_add_string( address->string, "%s;", ip );
      hornet_string_add_int32( address->string, "%d", ntohs( address->address.ipv4.sin_port ) );
    }break;
    
    case PF_INET6:{
      char ip[ INET6_ADDRSTRLEN ];
      inet_ntop( address->address.base.sa_family, &(address->address.ipv6.sin6_addr), ip, sizeof( ip ) );
      hornet_string_add_string( address->string, "%s;", ip );
      hornet_string_add_int32( address->string, "%d", ntohs( address->address.ipv6.sin6_port ) );
    }break;
    }
  }
  hornet_string_add_string( string, "%s", hornet_string_get_string( address->string ) );
}

void hornet_socket_address_resolve( string url, HornetSocketAddressResolveResultCallback result_callback, void* data ){
  HornetSocketAddressResolveResult* result = socket_address_resolve_result_new( url );
  struct addrinfo* response = null;
  do{
    if ( null == result ) break;
    
    struct addrinfo request;
    memset( &request, 0, sizeof( request ) );
    request.ai_family    = PF_UNSPEC;
    request.ai_socktype  = 0;
    request.ai_flags     = AI_V4MAPPED | AI_ALL;
    request.ai_protocol  = 0;
    request.ai_canonname = null;
    request.ai_addr      = null;
    request.ai_next      = null;
    if ( 0 == strcmp( "udp", hornet_string_get_string( result->protocol ) ) ){
      request.ai_socktype = SOCK_DGRAM;
    }else{
      request.ai_socktype = SOCK_STREAM;
    }
    
    string host = hornet_string_get_string( result->host );
    string port = hornet_string_get_string( result->port );
    int32 errcode = getaddrinfo( host, port, &request, &response );
    if ( EAI_BADFLAGS == errcode ){
      request.ai_flags = 0;
      errcode = getaddrinfo( host, port, &request, &response );
    }
    if ( 0 == errcode ){
      int32 addresses_count = 0;
      for ( struct addrinfo* info = response; null != info; info = info->ai_next ){
        ++addresses_count;
      }
      if ( 0 == addresses_count ) break;
      
      result->addresses = (HornetSocketAddress**)C_MEMORY_ALLOCATE( sizeof( HornetSocketAddress* ) * addresses_count );
      if ( null == result->addresses ) break;
      int32 addresses_index = 0;
      for ( struct addrinfo* info = response; null != info; info = info->ai_next ){
        result->addresses[ addresses_index++ ] = hornet_socket_address_new( info );
        if ( null == result->addresses[ addresses_index - 1 ] ) break;
        
        ++result->addresses_count;
      }
    }else{
      hornet_string_add_string( result->errmsg, "%s", gai_strerror( errcode ) );
    }
  }while ( 0 );
  result_callback( data, result );
  socket_address_resolve_result_delete( result );
  if ( null != response ) freeaddrinfo( response );
}

string hornet_socket_address_resolve_result_get_protocol( HornetSocketAddressResolveResult* result ){
  return hornet_string_get_string( result->protocol );
}

string hornet_socket_address_resolve_result_get_host( HornetSocketAddressResolveResult* result ){
  return hornet_string_get_string( result->host );
}

string hornet_socket_address_resolve_result_get_port( HornetSocketAddressResolveResult* result ){
  return hornet_string_get_string( result->port );
}

string hornet_socket_address_resolve_result_get_path( HornetSocketAddressResolveResult* result ){
  return hornet_string_get_string( result->path );
}

int32 hornet_socket_address_resolve_result_get_addresses_count( HornetSocketAddressResolveResult* result ){
  return result->addresses_count;
}

HornetSocketAddress* hornet_socket_address_resolve_result_get_address( HornetSocketAddressResolveResult* result, int32 index ){
  return ( index < result->addresses_count ) ? result->addresses[ index ] : null;
}

string hornet_socket_address_resolve_result_get_errmsg( HornetSocketAddressResolveResult* result ){
  return hornet_string_get_string( result->errmsg );
}
