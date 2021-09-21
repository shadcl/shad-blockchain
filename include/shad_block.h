#ifndef SHAD_BLOCK_H_
#define SHAD_BLOCK_H_

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>

#define SHAD_HASH_ALGO MHASH_SHA256
#define SHAD_HASH_BLOCK_SIZE 32

typedef union
{
  unsigned char bytes[sizeof(off_t)];
  off_t value;
} shad_block_key_t;

struct shad_block
{
  size_t len;
  unsigned char hash[SHAD_HASH_BLOCK_SIZE];
  unsigned char parent_hash[SHAD_HASH_BLOCK_SIZE];
  char *data;
  size_t data_len;
};
typedef struct shad_block shad_block_t;

#endif /* SHAD_BLOCK_H_ */
