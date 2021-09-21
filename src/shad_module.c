#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <mhash.h>
#include <sys/stat.h>

#include "shad_types.h"
#include "shad_block.h"
#include "shad_chaining.h"
#include "shad.h"

/* Hashes a block for a given parent
 * Assumes the block data and parent hash are ready for storage.
 */
int
shad_block_hash (shad_block_t *block, shad_block_t *parent)
{
  int ret = -1;
  shad_nonce_t nonce;
  char *buffer;
  size_t buffer_size;
  MHASH td;

  nonce = shad_get_nonce ();

  buffer_size = (sizeof(nonce) + sizeof(parent->hash) + block->data_len);
  buffer = malloc (buffer_size);
  if (buffer == NULL)
    return ret;

  memcpy (buffer, &(nonce), sizeof(nonce));
  memcpy ((buffer + sizeof(nonce)), parent->hash, sizeof(parent->hash));
  memcpy ((buffer + sizeof(nonce) + sizeof(parent->hash)), block->data,
	  block->data_len);

  td = mhash_init (SHAD_HASH_ALGO);
  if (td != MHASH_FAILED)
    {
      mhash (td, buffer, buffer_size);
      mhash_deinit (td, &(block->hash));

      memcpy (block->parent_hash, parent->hash, sizeof(block->hash));

      ret = 0;
    }

  free (buffer);

  return ret;
}

void
shad_print_block (shad_block_t *block)
{
  int i, j;

  printf (
      "hash ------------------------------------------------------------------------------------------\n");
  for (i = 0; i < sizeof(block->hash); i++)
    {
      printf ("%x", block->hash[i]);
    }
  printf (
      "\nparent hash -----------------------------------------------------------------------------------\n");
  for (i = 0; i < sizeof(block->parent_hash); i++)
    {
      printf ("%x", block->parent_hash[i]);
    }
  printf (
      "\ndata ------------------------------------------------------------------------------------------\n");
  for (i = 0; i < block->data_len; i += 32)
    {
      for (j = 0; j < 32; j++)
	{
	  if ((j + i) == block->data_len)
	    {
	      for (; j < 32; j++)
		{
		  printf ("%.2x ", 0);
		}
	      break;
	    }
	  printf ("%.2x ", block->data[(i + j)]);
	}
      printf ("\n");
    }
}
