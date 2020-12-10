#include <hornet.hpp>
#include <vector>

typedef struct {
  double Timestamp;
  uint32 Id;
  int32  DataSize;
} Header;

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
  uint8*                     WriteBytes;
  HornetSocket*              ServerSocket;
  std::vector< Connection* > Connections;
};

static void on_stream_socket_standby_event( HornetSocket* socket, int32 events ){
  // TODO
}

static void on_datagram_socket_standby_event( HornetSocket* socket, int32 events ){
  // TODO
}

static void on_socket_standalone_event( HornetSocket* socket, int32 events ){
  // TODO
}

static void on_socket_connect_event( HornetSocket* socket, int32 events ){
  // TODO
}

static void on_address_resolve( void* _arg, HornetSocketAddressResolveResult* result ){
  Arg* arg = (Arg*)_arg;
  do{
    int32 addresses_count = hornet_socket_address_resolve_result_get_addresses_count( result );
    printf( "Resolved address: %d\n", addresses_count );
    if ( addresses_count <= 0 ){
      // TODO
      break;
    }
    
    hornet_string_clear( arg->LogString );
    for ( int32 i = 0; i < addresses_count; ++i ){
      // TODO
      printf( "[%d]: %s\n", i, hornet_string_get_string( arg->LogString ) );
    }
    
    HornetSocketAddress* address = hornet_socket_address_resolve_result_get_address( result, 0 );
    HornetSocketAddressType type = hornet_socket_address_get_type( address );
    if ( 0 == arg->ConnectionNumMax ){
      arg->ServerSocket = hornet_socket_new( arg->Poll, address );
      if ( null == arg->ServerSocket ) break;
      if ( ! hornet_socket_standby( arg->ServerSocket ) ) break;
      if ( HORNET_SOCKET_ADDRESS_TYPE_STREAM == type ){
        hornet_socket_set_event_callback( arg->ServerSocket, on_stream_socket_standby_event );
      }else{
        hornet_socket_set_event_callback( arg->ServerSocket, on_datagram_socket_standby_event );
      }
    }else if ( HORNET_SOCKET_ADDRESS_TYPE_DATAGRAM == type ){
      for ( int32 i = 0; i < arg->ConnectionNumMax; ++i ){
        HornetSocket* socket = hornet_socket_new( arg->Poll, address );
        if ( null == socket ) break;
        if ( ! hornet_socket_standalone( socket ) ) break;
        
        // TODO
        hornet_socket_set_event_callback( socket, on_socket_standalone_event );
        arg->Connections.push_back( new Connection( socket ) );
      }
    }else if ( HORNET_SOCKET_ADDRESS_TYPE_STREAM == type ){
      for ( int32 i = 0; i < arg->ConnectionNumMax; ++i ){
        HornetSocket* socket = hornet_socket_new( arg->Poll, address );
        if ( null == socket ) break;
        if ( ! hornet_socket_connect( socket, arg->ConnectTimeout ) ) break;
        
        hornet_socket_set_event_callback( socket, on_socket_connect_event );
        arg->Connections.push_back( new Connection( socket ) );
      }
    }
    if ( arg->ConnectionNumMax != arg->Connections.size() ) break;
    return;
  }while ( 0 );
  arg->IsUpdate = false;
}

static bool on_init( Arg* arg ){
  arg->MeasureCount = 10;
  arg->ConnectionNumMax = 0;
  arg->ConnectTimeout = 3.0f;
  arg->DataSize = 128;
  arg->SleepMsec = 1;
  arg->Url = "udp://127.0.0.1;80";
  
  arg->IsUpdate = false;
  arg->LogString = hornet_string_new( 256 );
  arg->Poll = hornet_poll_new();
  arg->AutoId = 0;
  arg->WriteBytes = (uint8*)C_MEMORY_ALLOCATE( sizeof( Header ) + arg->DataSize );
  arg->ServerSocket = null;
  
  if ( null == arg->LogString ) return false;
  if ( null == arg->Poll ) return false;
  if ( null == arg->WriteBytes ) return false;
  
  for ( int32 i = 0; i < arg->DataSize; ++i ){
    arg->WriteBytes[ sizeof( Header ) + i ] = 0xFF - (i % 0xFF);
  }
  
  hornet_socket_address_resolve( arg->Url, on_address_resolve, arg );
  arg->IsUpdate = true;
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
      for ( int32 i = 0; i < arg.MeasureCount; ++i ){
        if ( ! arg.IsUpdate ) break;
        on_update( &arg );
      }
    }
    on_exit( &arg );
  }
  hornet_exit();
  return 0;
}
