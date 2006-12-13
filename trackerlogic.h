#ifndef __TRACKERLOGIC_H__
#define __TRACKERLOGIC_H__

#include <sys/types.h>
#include <sys/time.h>

/* Should be called BYTE, WORD, DWORD - but some OSs already have that and there's no #iftypedef */
/* They mark memory used as data instead of integer or human readable string -
   they should be cast before used as integer/text */
typedef unsigned char  ot_byte;
typedef unsigned short ot_word;
typedef unsigned long  ot_dword;

typedef ot_byte        ot_hash[20];
typedef ot_dword       ot_ip;
typedef time_t         ot_time;

#define MEMMOVE              memmove
#define BZERO                bzero
#define FORMAT_FIXED_STRING  sprintf
#define FORMAT_FORMAT_STRING sprintf
#define BINARY_FIND          binary_search

// We maintain a list of 256 pointers to sorted list of ot_torrent structs
// Sort key is, of course, its hash

// This list points to 9 pools of peers each grouped in five-minute-intervals
// thus achieving a timeout of 2700s or 45 minutes
// These pools are sorted by its binary content

#define OT_POOLS_COUNT   9
#define OT_POOLS_TIMEOUT 300
#define NOW              (time(NULL)/OT_POOLS_TIMEOUT)

typedef struct ot_vector {
  void   *data;
  size_t  size;
  size_t  space;   
} *ot_vector;

typedef struct ot_peer {
  ot_ip    ip;
  ot_dword port_flags;
} *ot_peer;
static const ot_byte PEER_FLAG_SEEDING   = 0x80;

typedef struct ot_peerlist {
  ot_time          base;
  unsigned long    seed_count[ OT_POOLS_COUNT ];
  struct ot_vector peers[ OT_POOLS_COUNT ];
} *ot_peerlist;

typedef struct ot_torrent {
  ot_hash       hash;
  ot_peerlist   peer_list;
} *ot_torrent;

void *map_file( char *file_name, size_t map_size );
void  unmap_file( char *file_name, void *map, size_t mapped_size, unsigned long real_size );

// This behaves quite like bsearch but allows to find
// the insertion point for inserts after unsuccessful searches
// in this case exactmatch is 0 on exit
//
void *binary_search( const void *key, const void *base,
                     const unsigned long member_count, const unsigned long member_size,
                     int (*compar) (const void *, const void *),
                     int *exactmatch );

//
// Exported functions
//

int  init_logic( char *chdir_directory );
void deinit_logic( );

ot_torrent add_peer_to_torrent( ot_hash *hash, ot_peer peer );
size_t return_peers_for_torrent( ot_torrent torrent, unsigned long amount, char *reply );

#endif
