#include <hornet.h>

int32 hornet_memory_aligned_size( int32 target_size, int32 alignment_size ){
  if ( alignment_size <= 1 ) return target_size;
  
  return ( ( target_size / alignment_size ) + ( 0 < target_size % alignment_size ) ) * alignment_size;
}
