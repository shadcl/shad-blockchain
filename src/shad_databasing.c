#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "shad_types.h"
#include "shad_block.h"
#include "shad_collection.h"
#include "shad_index.h"
#include "shad_databasing.h"

/* Adds a block to a collection
 * Assumes the block has all its members ready for storage.
 */
int
shad_collection_add_block (shad_block_t *block,
			   struct shad_collection_cursor *coll,
			   shad_block_key_t *key)
{
  off_t pos;
  shad_nsize_t size;

  pos = lseek (coll->fd, 0, SEEK_CUR);
  if (pos == -1)
    return -1;

  size.value = (block->data_len + sizeof(block->hash)
      + sizeof(block->parent_hash));

  /* size marker */
  if (write (coll->fd, &(size), sizeof(size)) == -1)
    return -1;
  /* hash header */
  if (write (coll->fd, &(block->hash), sizeof(block->hash)) == -1)
    return -1;
  /* parent hash header */
  if (write (coll->fd, &(block->parent_hash), sizeof(block->parent_hash)) == -1)
    return -1;
  /* body */
  if (write (coll->fd, block->data, block->data_len) == -1)
    return -1;

  key->value = pos;

  return 0;
}

/* Adds a block key to the index.
 */
int
shad_index_add_block_key (shad_block_key_t key, struct shad_index_cursor *index,
			  shad_ordinal_t *ordinal)
{
  if (write (index->fd, &(key), sizeof(key)) == -1)
    return -1;

  *(ordinal) = index->seq_head;

  index->seq_head++;

  return 0;
}
