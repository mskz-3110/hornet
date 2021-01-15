#include <hornet.h>

typedef struct {
  bool  is_locked;
  int32 prev_id;
  int32 next_id;
} HornetAristHeader;

#define HORNET_ARIST_HEADER_VALUE_SIZE( _arist )             (sizeof( HornetAristHeader ) + (_arist)->value_size)
#define HORNET_ARIST_MEMORY_SIZE( _arist, _count )           (HORNET_ARIST_HEADER_VALUE_SIZE( _arist ) * (_count))
#define HORNET_ARIST_GET_HEADER_FROM_ID( _arist, _id )       (HornetAristHeader*)&((_arist)->bytes[ HORNET_ARIST_MEMORY_SIZE( _arist, (_id) - 1 ) ])
#define HORNET_ARIST_GET_HEADER_FROM_VALUE( _arist, _value ) (HornetAristHeader*)((uint8*)(_value) - sizeof( HornetAristHeader ))
#define HORNET_ARIST_GET_VALUE_FROM_HEADER( _header )        ((uint8*)(_header) + sizeof( HornetAristHeader ))
#define HORNET_ARIST_GET_ID_FROM_HEADER( _arist, _header )   (int32)(((uint8*)(_header) - (_arist)->bytes) / HORNET_ARIST_HEADER_VALUE_SIZE( _arist ) + 1)
#define HORNET_ARIST_GET_ID_FROM_VALUE( _arist, _value )     HORNET_ARIST_GET_ID_FROM_HEADER( _arist, HORNET_ARIST_GET_HEADER_FROM_VALUE( _arist, _value ) )

static bool arist_lock_check( HornetArist* arist, int32 count ){
  int32 max_count = arist->count + count;
  if ( arist->allocated_count < max_count ){
    max_count = hornet_memory_aligned_size( max_count, arist->alignment_count );
    int32 new_size = HORNET_ARIST_MEMORY_SIZE( arist, max_count );
    uint8* reallocated_bytes = (uint8*)C_MEMORY_REALLOCATE( arist->bytes, new_size );
    if ( null == reallocated_bytes ) return false;
    
    int32 old_size = HORNET_ARIST_MEMORY_SIZE( arist, arist->allocated_count );
    memset( &(reallocated_bytes[ old_size ]), 0, new_size - old_size );
    
    arist->bytes = reallocated_bytes;
    arist->allocated_count = max_count;
  }
  return true;
}

HornetArist* hornet_arist_new( int32 alignment_count, int32 value_size ){
  HornetArist* arist = (HornetArist*)C_MEMORY_ALLOCATE( sizeof( HornetArist ) );
  do{
    if ( null == arist ) break;
    
    if ( value_size <= 0 ) break;
    
    arist->alignment_count = alignment_count;
    arist->value_size      = value_size;
    arist->allocated_count = 0;
    arist->count           = 0;
    arist->empty_id        = 1;
    arist->head_id         = 1;
    arist->tail_id         = 1;
    arist->bytes           = null;
    
    return arist;
  }while ( 0 );
  hornet_arist_delete( arist );
  return null;
}

void hornet_arist_delete( HornetArist* arist ){
  if ( null != arist ){
    if ( null != arist->bytes ) C_MEMORY_FREE( arist->bytes );
    C_MEMORY_FREE( arist );
  }
}

int32 hornet_arist_get_count( HornetArist* arist ){
  return arist->count;
}

int32 hornet_arist_get_head_id( HornetArist* arist ){
  return arist->head_id;
}

int32 hornet_arist_get_tail_id( HornetArist* arist ){
  return arist->tail_id;
}

void* hornet_arist_get_value( HornetArist* arist, int32 id ){
  do{
    if ( 0 == id ) break;
    if ( arist->allocated_count < id ) break;
    
    HornetAristHeader* header = HORNET_ARIST_GET_HEADER_FROM_ID( arist, id );
    if ( ! header->is_locked ) break;
    
    return HORNET_ARIST_GET_VALUE_FROM_HEADER( header );
  }while ( 0 );
  return null;
}

int32 hornet_arist_get_id( HornetArist* arist, void* value ){
  if ( null == value ) return 0;
  
  return HORNET_ARIST_GET_ID_FROM_VALUE( arist, value );
}

int32 hornet_arist_get_prev_id( HornetArist* arist, int32 id ){
  HornetAristHeader* header = HORNET_ARIST_GET_HEADER_FROM_ID( arist, id );
  return header->prev_id;
}

int32 hornet_arist_get_next_id( HornetArist* arist, int32 id ){
  HornetAristHeader* header = HORNET_ARIST_GET_HEADER_FROM_ID( arist, id );
  return header->next_id;
}

int32 hornet_arist_lock( HornetArist* arist ){
  int32 id = 0;
  do{
    if ( ! arist_lock_check( arist, 1 ) ) break;
    
    HornetAristHeader* empty = HORNET_ARIST_GET_HEADER_FROM_ID( arist, arist->empty_id );
    HornetAristHeader* head = HORNET_ARIST_GET_HEADER_FROM_ID( arist, arist->head_id );
    HornetAristHeader* tail = HORNET_ARIST_GET_HEADER_FROM_ID( arist, arist->tail_id );
    empty->is_locked = true;
    empty->prev_id = arist->tail_id;
    empty->next_id = arist->head_id;
    id = tail->next_id = head->prev_id = arist->tail_id = arist->empty_id;
    ++(arist->count);
    for ( ++(arist->empty_id); arist->empty_id <= arist->allocated_count; ++(arist->empty_id) ){
      if ( null == hornet_arist_get_value( arist, arist->empty_id ) ) break;
    }
  }while ( 0 );
  return id;
}

void hornet_arist_unlock( HornetArist* arist, int32 id ){
  void* value = hornet_arist_get_value( arist, id );
  if ( null == value ) return;
  
  HornetAristHeader* header = HORNET_ARIST_GET_HEADER_FROM_VALUE( arist, value );
  header->is_locked = false;
  if ( arist->head_id == id ) arist->head_id = header->next_id;
  if ( arist->tail_id == id ) arist->tail_id = header->prev_id;
  if ( id < arist->empty_id ) arist->empty_id = id;
  
  HornetAristHeader* prev = HORNET_ARIST_GET_HEADER_FROM_ID( arist, header->prev_id );
  HornetAristHeader* next = HORNET_ARIST_GET_HEADER_FROM_ID( arist, header->next_id );
  prev->next_id = header->next_id;
  next->prev_id = header->prev_id;
  --(arist->count);
  if ( 0 == arist->count ){
    arist->head_id = arist->tail_id = 1;
  }
}

void hornet_arist_each( HornetArist* arist, HornetAristEachCallbackType callback, void* arg ){
  HornetAristHeader* header;
  int32 count = arist->count;
  int32 id = arist->head_id;
  for ( ; 0 < count; --count, id = hornet_arist_get_next_id( arist, id ) ){
    header = HORNET_ARIST_GET_HEADER_FROM_ID( arist, id );
    if ( ! callback( arist, arg, id, HORNET_ARIST_GET_VALUE_FROM_HEADER( header ) ) ) break;
  }
}

void hornet_arist_reach( HornetArist* arist, HornetAristEachCallbackType callback, void* arg ){
  HornetAristHeader* header;
  int32 count = arist->count;
  int32 id = arist->tail_id;
  for ( ; 0 < count; --count, id = hornet_arist_get_prev_id( arist, id ) ){
    header = HORNET_ARIST_GET_HEADER_FROM_ID( arist, id );
    if ( ! callback( arist, arg, id, HORNET_ARIST_GET_VALUE_FROM_HEADER( header ) ) ) break;
  }
}
