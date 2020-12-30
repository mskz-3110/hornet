#include <hornet.hpp>
#include <vector>

typedef struct {
  double Timestamp;
  uint32 Id;
  int32  DataSize;
} WriteHeader;

class Connection {
public:
  class Stats {
  private:
    double Rtt;
    
    int32  WriteSize;
    int32  WriteCount;
    
    int32  ReadSize;
    int32  ReadCount;
    
  public:
    Stats(){
      Clear();
    }
    
    void Clear(){
      Rtt = 0.0;
      WriteSize = 0;
      WriteCount = 0;
      ReadSize = 0;
      ReadCount = 0;
    }
  };
  
  HornetSocket* Socket;
  Stats         Stats;
  
  Connection( HornetSocket* socket ){
    Socket = socket;
  }
};

class Arg {
public:
  int32                      MeasureCount;
  int32                      ConnectionNumMax;
  float                      ConnectTimeout;
  int32                      DataSize;
  int32                      SleepMsec;
  string                     Url;
  
  bool                       IsUpdate;
  HornetString*              LogString;
  HornetPoll*                Poll;
  uint32                     AutoId;
  int32                      WriteSize;
  WriteHeader*               WriteHeader;
  uint8*                     WriteBytes;
  HornetSocket*              ServerSocket;
  std::vector< Connection* > Connections;
};

static void on_server_socket_event( HornetSocket* socket, int32 events ){
  // TODO
  printf( "on_server_socket_event events=0x%X\n", events );
  if ( 0 != ( HORNET_SOCKET_EVENT_READABLE & events ) ){
    char data[ 65536 ];
    while ( 1 ){
      int32 data_size = hornet_socket_read( socket, data, sizeof( data ) );
      if ( data_size <= 0 ) break;
      if ( ! hornet_socket_write( socket, data, data_size ) ) break;
    }
  }
}

static void on_client_socket_event( HornetSocket* socket, int32 events ){
  // TODO
  printf( "on_client_socket_event events=0x%X\n", events );
  if ( 0 != ( HORNET_SOCKET_EVENT_READABLE & events ) ){
    char data[ 65536 ];
    double read_timestamp = hornet_time_now();
    while ( 1 ){
      int32 data_size = hornet_socket_read( socket, data, sizeof( data ) );
      if ( data_size <= 0 ) break;
      
      WriteHeader* write_header = (WriteHeader*)data;
      printf( "%d %f %d\n", data_size, read_timestamp - write_header->Timestamp, write_header->Id );
    }
  }
}

static void on_address_resolve( void* _arg, HornetSocketAddressResolveResult* result ){
  Arg* arg = (Arg*)_arg;
  do{
    int32 addresses_count = hornet_socket_address_resolve_result_get_addresses_count( result );
    printf( "Resolved address: %d\n", addresses_count );
    if ( addresses_count <= 0 ) break;
    
    for ( int32 i = 0; i < addresses_count; ++i ){
      hornet_string_clear( arg->LogString );
      hornet_socket_address_to_string( arg->LogString, hornet_socket_address_resolve_result_get_address( result, i ) );
      printf( "[%d]: %s\n", i, hornet_string_get_string( arg->LogString ) );
    }
    
    HornetSocketAddress* address = hornet_socket_address_resolve_result_get_address( result, 0 );
    HornetSocketAddressType type = hornet_socket_address_get_type( address );
    if ( 0 == arg->ConnectionNumMax ){
      arg->ServerSocket = hornet_socket_new( arg->Poll, address );
      if ( null == arg->ServerSocket ) break;
      if ( ! hornet_socket_standby( arg->ServerSocket ) ){
        printf( "%s\n", hornet_socket_get_errmsg( arg->ServerSocket ) );
        break;
      }
      hornet_socket_set_event_callback( arg->ServerSocket, on_server_socket_event );
      hornet_string_clear( arg->LogString );
      hornet_socket_to_string( arg->LogString, arg->ServerSocket );
      printf( "standby: %s\n", hornet_string_get_string( arg->LogString ) );
    }else if ( HORNET_SOCKET_ADDRESS_TYPE_UDP == type ){
      for ( int32 i = 0; i < arg->ConnectionNumMax; ++i ){
        HornetSocket* socket = hornet_socket_new( arg->Poll, address );
        if ( null == socket ) break;
        if ( ! hornet_socket_standalone( socket ) ) break;
        
        arg->WriteHeader->Timestamp = hornet_time_now();
        arg->WriteHeader->Id = ++arg->AutoId;
        if ( ! hornet_socket_write( socket, arg->WriteBytes, arg->WriteSize ) ) break;
        
        hornet_socket_set_event_callback( socket, on_client_socket_event );
        arg->Connections.push_back( new Connection( socket ) );
        hornet_string_clear( arg->LogString );
        hornet_socket_to_string( arg->LogString, socket );
        printf( "standalone[%d]: %s\n", i, hornet_string_get_string( arg->LogString ) );
      }
    }else if ( HORNET_SOCKET_ADDRESS_TYPE_TCP == type ){
      for ( int32 i = 0; i < arg->ConnectionNumMax; ++i ){
        HornetSocket* socket = hornet_socket_new( arg->Poll, address );
        if ( null == socket ) break;
        if ( ! hornet_socket_connect( socket, arg->ConnectTimeout ) ) break;
        
        hornet_socket_set_event_callback( socket, on_client_socket_event );
        arg->Connections.push_back( new Connection( socket ) );
      }
    }
    if ( arg->ConnectionNumMax != arg->Connections.size() ) break;
    return;
  }while ( 0 );
  arg->IsUpdate = false;
}

static bool on_init( Arg* arg ){
  char url[ 256 ] = "udp://127.0.0.1;1234";
  
  arg->MeasureCount = -1;
  arg->ConnectionNumMax = 0;
  arg->ConnectTimeout = 3.0f;
  arg->DataSize = 128;
  arg->SleepMsec = 1;
  arg->Url = url;
  
  HornetIO stdin_io;
  stdin_io.value = fileno( stdin );
  hornet_io_set_flags( &stdin_io, hornet_io_get_flags( &stdin_io ) | O_NONBLOCK | O_CLOEXEC );
  
  char line[ 256 ];
  while ( null != fgets( line, sizeof( line ), stdin ) ){
    line[ strlen( line ) - 1 ] = '\0';
    HornetStringParseResult result = hornet_string_split_once( line, "\t" );
    if ( 0 == strncmp( "MeasureCount", result.left_string, result.left_length ) ){
      arg->MeasureCount = hornet_string_to_int32( result.right_string );
    }else if ( 0 == strncmp( "ConnectionNumMax", result.left_string, result.left_length ) ){
      arg->ConnectionNumMax = hornet_string_to_int32( result.right_string );
    }else if ( 0 == strncmp( "ConnectTimeout", result.left_string, result.left_length ) ){
      arg->ConnectTimeout = hornet_string_to_double( result.right_string );
    }else if ( 0 == strncmp( "DataSize", result.left_string, result.left_length ) ){
      arg->DataSize = hornet_string_to_int32( result.right_string );
    }else if ( 0 == strncmp( "SleepMsec", result.left_string, result.left_length ) ){
      arg->SleepMsec = hornet_string_to_int32( result.right_string );
    }else if ( 0 == strncmp( "Url", result.left_string, result.left_length ) ){
      strcpy( url, result.right_string );
    }
  }
  
  arg->IsUpdate = false;
  arg->LogString = hornet_string_new( 256 );
  arg->Poll = hornet_poll_new( 10000 + arg->ConnectionNumMax );
  arg->AutoId = 0;
  arg->WriteSize = sizeof( WriteHeader ) + arg->DataSize;
  arg->WriteBytes = (uint8*)C_MEMORY_ALLOCATE( arg->WriteSize );
  arg->ServerSocket = null;
  
  if ( null == arg->LogString ) return false;
  if ( null == arg->Poll ) return false;
  if ( null == arg->WriteBytes ) return false;
  
  arg->WriteHeader = (WriteHeader*)arg->WriteBytes;
  arg->WriteHeader->DataSize = arg->DataSize;
  for ( int32 i = 0; i < arg->DataSize; ++i ){
    arg->WriteBytes[ sizeof( WriteHeader ) + i ] = 0xFF - (i % 0xFF);
  }
  
  arg->IsUpdate = true;
  hornet_socket_address_resolve( arg->Url, on_address_resolve, arg );
  return true;
}

static void on_update( Arg* arg ){
  // TODO
  
  double start_time = hornet_time_now();
  while ( 1 ){
    while ( hornet_poll_update( arg->Poll ) ){}
    
    double end_time = hornet_time_now();
    if ( 1.0 <= end_time - start_time ){
      // TODO
      hornet_string_clear( arg->LogString );
      hornet_string_add_time( arg->LogString, end_time );
      printf( "%s\n", hornet_string_get_string( arg->LogString ) );
      break;
    }
    
    hornet_sleep( arg->SleepMsec );
  }
}

static void on_exit( Arg* arg ){
  if ( null != arg->WriteBytes ){
    C_MEMORY_FREE( arg->WriteBytes );
    arg->WriteBytes = null;
  }
  
  if ( null != arg->ServerSocket ){
    hornet_socket_delete( arg->ServerSocket );
    arg->ServerSocket = null;
  }
  
  if ( 0 < arg->Connections.size() ){
    for ( std::vector< Connection* >::iterator itr = arg->Connections.begin(); arg->Connections.end() != itr; ++itr ){
      Connection* connection = *itr;
      hornet_socket_delete( connection->Socket );
      delete connection;
    }
    arg->Connections.clear();
  }
}

int32 main(){
  Arg arg;
  if ( hornet_init() ){
    if ( on_init( &arg ) ){
      int32 update_count = 0;
      while ( 1 ){
        if ( ! arg.IsUpdate ) break;
        on_update( &arg );
        if ( 0 < arg.MeasureCount ){
          if ( arg.MeasureCount <= ++update_count ) break;
        }
      }
    }
    on_exit( &arg );
  }
  hornet_exit();
  return 0;
}
