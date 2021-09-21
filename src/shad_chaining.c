#include <stdlib.h>
#include <stddef.h>
#include <fcntl.h>
#include <time.h>

#include "shad_types.h"
#include "shad_block.h"
#include "shad_collection.h"
#include "shad_index.h"
#include "shad_chaining.h"

/* Fetches a block for a given ordinal position.
 * Ordinal positions are written to STDOUT when new blocks are committed from
 * the command line.
 */
shad_block_t*
shad_fetch_block (shad_ordinal_t pos, struct shad_index_cursor *index,
		  struct shad_collection_cursor *coll)
{
  shad_nsize_t size;
  shad_block_key_t key;
  shad_block_t *block = NULL;

  /* move index cursor to ordinal position */
  if (lseek (index->fd, ((pos - 1) * sizeof(shad_ordinal_t)), SEEK_SET) == -1)
    return NULL;

  /* get collection key from the index */
  if (read (index->fd, &(key), sizeof(key)) != sizeof(key))
    return NULL;

  /* move collection cursor to key */
  if (lseek (coll->fd, key.value, SEEK_SET) == -1)
    return NULL;

  /* read the size */
  if (read (coll->fd, &(size), sizeof(size)) != sizeof(size))
    return NULL;

  block = shad_block_create (NULL, 0);
  if (block == NULL)
    return NULL;

  /* parse hash and parent hash headers */
  if (read (coll->fd, block->hash, sizeof(block->hash)) == sizeof(block->hash)
      && read (coll->fd, block->parent_hash, sizeof(block->parent_hash))
	  == sizeof(block->parent_hash))
    {
      /* parse the body */
      block->data_len = (size.value
	  - (sizeof(block->hash) + sizeof(block->parent_hash)));
      block->data = malloc (block->data_len);
      if ((block->data != NULL)
	  && read (coll->fd, block->data, block->data_len) == block->data_len)
	{
	  block->len = size.value;
	  return block;
	}
    }

  if (block->data != NULL)
    {
      free (block->data);
    }
  shad_block_destroy (block);

  return NULL;
}

/* Create a block in memory
 * The block is initialized with passed data and data length.
 */
shad_block_t*
shad_block_create (char *data, size_t data_len)
{
  shad_block_t *block;

  block = calloc (1, sizeof(*block));
  if (block == NULL)
    return NULL;

  block->data = data;
  block->data_len = data_len;

  return block;
}

/* Destroys a block
 * The block data is dynamic, so it is not freed during the destruction. Block
 * data must be freed in calling logic.
 */
void
shad_block_destroy (shad_block_t *block)
{
  free (block);
}

/* Returns a nonce
 * Just used nano time from POSIX timespec
 */
shad_nonce_t
shad_get_nonce ()
{
  struct timespec time;

  clock_gettime (CLOCK_REALTIME, &time);

  return time.tv_nsec;
}

/* Commits a block to the blockchain
 * This function and all subsequent instructions are atomic.
 */
int
shad_commit_block (shad_block_t *block, shad_block_t *parent,
		   struct shad_index_cursor *index,
		   struct shad_collection_cursor *coll, shad_ordinal_t *ordinal)
{
  int ret = -1;
  shad_block_key_t key;

  if (shad_block_hash (block, parent) == 0
      && shad_collection_add_block (block, coll, &(key)) == 0
      && shad_index_add_block_key (key, index, ordinal) == 0)
    {
      ret = 0;
    }

  return ret;
}

/* Generates the root block of the blockchain
 * The root is used for block ordinals > 0
 */
shad_block_t*
shad_generate_root_block (struct shad_index_cursor *index,
			  struct shad_collection_cursor *coll)
{
  char data[] = "What doesn't bend, breaks.";
  size_t data_len = strlen (data);
  shad_block_t *block;
  shad_block_t *parent;
  shad_ordinal_t ordinal;

  block = shad_block_create (data, data_len);
  parent = shad_block_create ("", 0);

  if (shad_commit_block (block, parent, index, coll, &(ordinal)) == -1)
    {
      shad_block_destroy (block);
      block = NULL;
    }

  shad_block_destroy (parent);

  return block;
}
