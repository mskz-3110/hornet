#include <hornet.h>

typedef struct {
  bool      is_locked;
  int32     prev_index;
  int32     next_index;
  uint32    key;
  HornetList* group_list;
} HornetStoreHeader;

typedef struct {
  HornetStore* store;
  uint32     key;
  int32      index;
} HornetStoreFind;

#define HORNET_STORE_HEADER_VALUE_SIZE( _store )              (sizeof( HornetStoreHeader ) + (_store)->value_size)
#define HORNET_STORE_MEMORY_SIZE( _store, _count )            (HORNET_STORE_HEADER_VALUE_SIZE( _store ) * (_count))
#define HORNET_STORE_GET_HEADER_FROM_INDEX( _store, _index )  (HornetStoreHeader*)&((_store)->bytes[ HORNET_STORE_MEMORY_SIZE( _store, (_index) ) ])
#define HORNET_STORE_GET_HEADER_FROM_VALUE( _store, _value )  (HornetStoreHeader*)((uint8*)(_value) - sizeof( HornetStoreHeader ))
#define HORNET_STORE_GET_VALUE_FROM_HEADER( _header )         ((uint8*)(_header) + sizeof( HornetStoreHeader ))
#define HORNET_STORE_GET_INDEX_FROM_HEADER( _store, _header ) (int32)(((uint8*)(_header) - (_store)->bytes) / HORNET_STORE_HEADER_VALUE_SIZE( _store ))
#define HORNET_STORE_GET_INDEX_FROM_VALUE( _store, _value )   HORNET_STORE_GET_INDEX_FROM_HEADER( _store, HORNET_STORE_GET_HEADER_FROM_VALUE( _store, _value ) )

static int32 store_get_prev_index( HornetStore* store, int32 index ){
  HornetStoreHeader* header = HORNET_STORE_GET_HEADER_FROM_INDEX( store, index );
  return header->prev_index;
}

static int32 store_get_next_index( HornetStore* store, int32 index ){
  HornetStoreHeader* header = HORNET_STORE_GET_HEADER_FROM_INDEX( store, index );
  return header->next_index;
}

static bool on_group_list_find( HornetList* list, void* arg, HornetListNode* node, void* value ){
  HornetStoreFind* store_find = (HornetStoreFind*)arg;
  HornetStoreHeader* header = (HornetStoreHeader*)value;
  if ( store_find->key == header->key ){
    store_find->index = HORNET_STORE_GET_INDEX_FROM_HEADER( store_find->store, header );
    return false;
  }
  return true;
}

static int32 store_get_index( HornetStore* store, uint32 key ){
  HornetStoreFind store_find;
  store_find.store = store;
  store_find.key = key;
  store_find.index = -1;
  do{
    if ( store->count <= 0 ) break;
    
    HornetStoreHeader* group_list_header = HORNET_STORE_GET_HEADER_FROM_INDEX( store, key % store->allocated_count );
    if ( key == group_list_header->key ){
      store_find.index = HORNET_STORE_GET_INDEX_FROM_HEADER( store, group_list_header );
    }else if ( null != group_list_header->group_list ){
      hornet_list_each( group_list_header->group_list, on_group_list_find, &store_find );
    }
  }while ( 0 );
  return store_find.index;
}

static int32 store_get_empty_index_range( HornetStore* store, int32 start_index, int32 end_index ){
  HornetStoreHeader* header;
  int32 index = start_index;
  for ( ; index <= end_index; ++index ){
    header = HORNET_STORE_GET_HEADER_FROM_INDEX( store, index );
    if ( ! header->is_locked ) return index;
  }
  return -1;
}

static int32 store_get_empty_index( HornetStore* store, uint32 key ){
  int32 start_index = key % store->allocated_count;
  int32 empty_index = store_get_empty_index_range( store, start_index, store->allocated_count - 1 );
  if ( (empty_index < 0) && (0 < start_index) ){
    empty_index = store_get_empty_index_range( store, 0, start_index - 1 );
  }
  return empty_index;
}

static bool store_reallocate( HornetStore* store, int32 allocated_count ){
  int32 allocated_size = HORNET_STORE_MEMORY_SIZE( store, allocated_count );
  uint8* allocated_bytes = (uint8*)C_MEMORY_ALLOCATE( allocated_size );
  HornetStore* new_store = hornet_store_new( store->alignment_count, store->value_size, null );
  do{
    if ( null == allocated_bytes ) return false;
    if ( null == new_store ) break;
    
    memset( allocated_bytes, 0, allocated_size );
    new_store->allocated_count = allocated_count;
    new_store->bytes = allocated_bytes;
    HornetStoreHeader* header;
    HornetStoreHeader* new_store_tail;
    int32 count = store->count;
    int32 index = store->head_index;
    for ( ; 0 < count; --count, index = store_get_next_index( store, index ) ){
      header = HORNET_STORE_GET_HEADER_FROM_INDEX( store, index );
      if ( ! hornet_store_lock( new_store, header->key, false ) ) break;
      
      new_store_tail = HORNET_STORE_GET_HEADER_FROM_INDEX( new_store, new_store->tail_index );
      memcpy( HORNET_STORE_GET_VALUE_FROM_HEADER( new_store_tail ), HORNET_STORE_GET_VALUE_FROM_HEADER( header ), store->value_size );
    }
    if ( store->count != new_store->count ) break;
    
    if ( null != store->bytes ) C_MEMORY_FREE( store->bytes );
    store->bytes = allocated_bytes;
    store->allocated_count = allocated_count;
    store->head_index = new_store->head_index;
    store->tail_index = new_store->tail_index;
    
    new_store->count = 0;
    new_store->bytes = null;
    hornet_store_delete( new_store );
    
    return true;
  }while ( 0 );
  if ( null != allocated_bytes ) C_MEMORY_FREE( allocated_bytes );
  if ( null != new_store ) hornet_store_delete( new_store );
  return false;
}

static bool store_lock_check( HornetStore* store, int32 count ){
  int32 max_count = store->count + count;
  if ( store->allocated_count < max_count ){
    return store_reallocate( store, hornet_memory_aligned_size( max_count, store->alignment_count ) );
  }
  return true;
}

static bool on_group_list_remove( HornetList* list, void* arg, HornetListNode* node, void* value ){
  if ( arg == value ){
    hornet_list_remove( list, node );
    return false;
  }
  return true;
}

static void store_unlock( HornetStore* store, int32 index ){
  HornetStoreHeader* header = HORNET_STORE_GET_HEADER_FROM_INDEX( store, index );
  if ( ! header->is_locked ) return;
  
  HornetStoreHeader* group_list_header = HORNET_STORE_GET_HEADER_FROM_INDEX( store, header->key % store->allocated_count );
  if ( null != group_list_header->group_list ){
    hornet_list_each( group_list_header->group_list, on_group_list_remove, header );
    if ( 0 == hornet_list_get_count( group_list_header->group_list ) ){
      hornet_list_delete( group_list_header->group_list );
      group_list_header->group_list = null;
    }
  }
  
  header->is_locked = false;
  if ( store->head_index == index ) store->head_index = header->next_index;
  if ( store->tail_index == index ) store->tail_index = header->prev_index;
  
  HornetStoreHeader* prev = HORNET_STORE_GET_HEADER_FROM_INDEX( store, header->prev_index );
  HornetStoreHeader* next = HORNET_STORE_GET_HEADER_FROM_INDEX( store, header->next_index );
  prev->next_index = header->next_index;
  next->prev_index = header->prev_index;
  --(store->count);
  if ( 0 == store->count ){
    store->head_index = store->tail_index = -1;
  }
  
  if ( null != store->delete_value_callback ) store->delete_value_callback( HORNET_STORE_GET_VALUE_FROM_HEADER( header ) );
}

HornetStore* hornet_store_new( int32 alignment_count, int32 value_size, HornetStoreDeleteValueCallbackType delete_value_callback ){
  HornetStore* store = (HornetStore*)C_MEMORY_ALLOCATE( sizeof( HornetStore ) );
  do{
    if ( null == store ) break;
    
    if ( value_size <= 0 ) break;
    
    store->alignment_count = alignment_count;
    store->value_size = value_size;
    store->allocated_count = 0;
    store->count = 0;
    store->head_index = -1;
    store->tail_index = -1;
    store->bytes = null;
    store->delete_value_callback = delete_value_callback;
    
    return store;
  }while ( 0 );
  hornet_store_delete( store );
  return null;
}

void hornet_store_delete( HornetStore* store ){
  if ( null != store ){
    hornet_store_clear( store );
    if ( null != store->bytes ) C_MEMORY_FREE( store->bytes );
    C_MEMORY_FREE( store );
  }
}

int32 hornet_store_get_count( HornetStore* store ){
  return store->count;
}

void* hornet_store_get_value( HornetStore* store, uint32 key ){
  do{
    int32 index = store_get_index( store, key );
    if ( index < 0 ) break;
    
    return HORNET_STORE_GET_VALUE_FROM_HEADER( HORNET_STORE_GET_HEADER_FROM_INDEX( store, index ) );
  }while ( 0 );
  return null;
}

void hornet_store_clear( HornetStore* store ){
  int32 count = store->count;
  int32 index = store->head_index;
  for ( ; 0 < count; --count, index = store_get_next_index( store, index ) ){
    store_unlock( store, index );
  }
}

bool hornet_store_lock( HornetStore* store, uint32 key, bool is_unlock_check ){
  do{
    if ( is_unlock_check ) hornet_store_unlock( store, key );
    
    if ( ! store_lock_check( store, 1 ) ) break;
    
    int32 index = store_get_empty_index( store, key );
    if ( index < 0 ) break;
    
    HornetStoreHeader* empty = HORNET_STORE_GET_HEADER_FROM_INDEX( store, index );
    empty->is_locked = true;
    empty->key = key;
    HornetStoreHeader* group_list_header = HORNET_STORE_GET_HEADER_FROM_INDEX( store, key % store->allocated_count );
    if ( empty != group_list_header ){
      if ( null == group_list_header->group_list ){
        group_list_header->group_list = hornet_list_new( null );
        if ( null == group_list_header->group_list ) break;
      }
      if ( null == hornet_list_tail_push( group_list_header->group_list, empty ) ) break;
    }
    if ( 0 == store->count ){
      store->head_index = store->tail_index = index;
    }
    HornetStoreHeader* head = HORNET_STORE_GET_HEADER_FROM_INDEX( store, store->head_index );
    HornetStoreHeader* tail = HORNET_STORE_GET_HEADER_FROM_INDEX( store, store->tail_index );
    empty->prev_index = store->tail_index;
    empty->next_index = store->head_index;
    tail->next_index = head->prev_index = store->tail_index = index;
    ++(store->count);
    
    return true;
  }while ( 0 );
  return false;
}

void hornet_store_unlock( HornetStore* store, uint32 key ){
  int32 index = store_get_index( store, key );
  if ( 0 <= index ) store_unlock( store, index );
}

void hornet_store_each( HornetStore* store, HornetStoreEachCallbackType callback, void* arg ){
  HornetStoreHeader* header;
  int32 count = store->count;
  int32 index = store->head_index;
  for ( ; 0 < count; --count, index = store_get_next_index( store, index ) ){
    header = HORNET_STORE_GET_HEADER_FROM_INDEX( store, index );
    if ( ! callback( store, arg, header->key, HORNET_STORE_GET_VALUE_FROM_HEADER( header ) ) ) break;
  }
}

void hornet_store_reach( HornetStore* store, HornetStoreEachCallbackType callback, void* arg ){
  HornetStoreHeader* header;
  int32 count = store->count;
  int32 index = store->tail_index;
  for ( ; 0 < count; --count, index = store_get_prev_index( store, index ) ){
    header = HORNET_STORE_GET_HEADER_FROM_INDEX( store, index );
    if ( ! callback( store, arg, header->key, HORNET_STORE_GET_VALUE_FROM_HEADER( header ) ) ) break;
  }
}
