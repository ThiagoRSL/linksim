#define  RND_IMPLEMENTATION

#include "rnd.h"

void rnd_pcg_seed( rnd_pcg_t* pcg, RND_U32 seed )
{
    RND_U64 value = ( ( (RND_U64) seed ) << 1ULL ) | 1ULL;
    value = rnd_internal_murmur3_avalanche64( value );
    pcg->state[ 0 ] = 0U;
    pcg->state[ 1 ] = ( value << 1ULL ) | 1ULL;
    rnd_pcg_next( pcg );
    pcg->state[ 0 ] += rnd_internal_murmur3_avalanche64( value );
    rnd_pcg_next( pcg );
}

RND_U32 rnd_pcg_next( rnd_pcg_t* pcg )
{
    RND_U64 oldstate = pcg->state[ 0 ];
    pcg->state[ 0 ] = oldstate * 0x5851f42d4c957f2dULL + pcg->state[ 1 ];
    RND_U32 xorshifted = (RND_U32)( ( ( oldstate >> 18ULL)  ^ oldstate ) >> 27ULL );
    RND_U32 rot = (RND_U32)( oldstate >> 59ULL );
    return ( xorshifted >> rot ) | ( xorshifted << ( ( -(int) rot ) & 31 ) );
}

float rnd_pcg_nextf( rnd_pcg_t* pcg )
{
    return rnd_internal_float_normalized_from_u32( rnd_pcg_next( pcg ) );
}

int rnd_pcg_range( rnd_pcg_t* pcg, int min, int max )
{
    int const range = ( max - min ) + 1;
    if( range <= 0 ) return min;
    int const value = (int) ( rnd_pcg_nextf( pcg ) * range );
    return min + value; 
}