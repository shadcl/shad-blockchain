#ifndef SHAD_H_
#define SHAD_H_

#include <stddef.h>
#include <fcntl.h>

#define SHAD_HASH_ALGO MHASH_SHA256
#define SHAD_HASH_BLOCK_SIZE 32

#define SHAD_INPUT_BUFFER_SIZE 2048

#ifndef SHAD_DATA_ATOMICITY
#define SHAD_WRITE_FLAGS O_RDWR | O_APPEND | O_CREAT
#else
#define SHAD_WRITE_FLAGS O_RDWR | O_APPEND | O_SYNC | O_CREAT
#endif

#define SHAD_READ_FLAGS O_RDONLY
#define SHAD_OPEN_MODE S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP

typedef off_t shad_ordinal_t;
typedef long shad_nonce_t;

typedef union
{
  unsigned char bytes[sizeof(off_t)];
  off_t value;
} shad_block_key_t;

typedef union
{
  unsigned char bytes[sizeof(size_t)];
  size_t value;
} shad_nsize_t;

struct shad_block
{
  size_t len;
  unsigned char hash[SHAD_HASH_BLOCK_SIZE];
  unsigned char parent_hash[SHAD_HASH_BLOCK_SIZE];
  char *data;
  size_t data_len;
};
typedef struct shad_block shad_block_t;

struct shad_index_handle
{
  int fd;
  struct stat stat;
  shad_ordinal_t seq_head;
};

struct shad_collection_handle
{
  int fd;
  struct stat stat;
};

int
shad_collection_add_block (shad_block_t*, struct shad_collection_handle*,
			   shad_block_key_t*);

int
shad_index_add_block_key (shad_block_key_t, struct shad_index_handle*,
			  shad_ordinal_t*);

shad_block_t*
shad_fetch_block (shad_ordinal_t, struct shad_index_handle*,
		  struct shad_collection_handle*);

shad_block_t*
shad_block_create (char*, size_t);

void
shad_block_destroy (shad_block_t*);

shad_nonce_t
shad_get_nonce ();

extern int
shad_block_hash (shad_block_t*, shad_block_t*);

int
shad_commit_block (shad_block_t *, shad_block_t *,
      struct shad_index_handle *,
      struct shad_collection_handle *, shad_ordinal_t *);

shad_block_t*
shad_generate_root_block (struct shad_index_handle*,
			  struct shad_collection_handle*);

extern void
shad_print_block (shad_block_t*);

#endif /* SHAD_H_ */
