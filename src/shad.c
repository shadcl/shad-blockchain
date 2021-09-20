#include "shad.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <mhash.h>
#include <sys/stat.h>

/* Adds a block to a collection
 * Assumes the block has all its members ready for storage.
 */
int
shad_collection_add_block (shad_block_t *block,
			   struct shad_collection_handle *coll,
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
shad_index_add_block_key (shad_block_key_t key, struct shad_index_handle *index,
			  shad_ordinal_t *ordinal)
{
  if (write (index->fd, &(key), sizeof(key)) == -1)
    return -1;

  *(ordinal) = index->seq_head;

  index->seq_head++;

  return 0;
}

/* Fetches a block for a given ordinal position.
 * Ordinal positions are written to STDOUT when new blocks are committed from
 * the command line.
 */
shad_block_t*
shad_fetch_block (shad_ordinal_t pos, struct shad_index_handle *index,
		  struct shad_collection_handle *coll)
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
shad_block_commit (shad_block_t *block, shad_block_t *parent,
		   struct shad_index_handle *index,
		   struct shad_collection_handle *coll, shad_ordinal_t *ordinal)
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
shad_generate_root_block (struct shad_index_handle *index,
			  struct shad_collection_handle *coll)
{
  char data[] = "What doesn't bend, breaks.";
  size_t data_len = strlen (data);
  shad_block_t *block;
  shad_block_t *parent;
  shad_ordinal_t ordinal;

  block = shad_block_create (data, data_len);
  parent = shad_block_create ("", 0);

  if (shad_block_commit (block, parent, index, coll, &(ordinal)) == -1)
    {
      shad_block_destroy (block);
      block = NULL;
    }

  shad_block_destroy (parent);

  return block;
}

/* Usage function
 */
static void
shad_usage (const char *program)
{
  printf ("Usage: %s [...]\n", program);
  printf (" Examples:\n");
  printf ("  Commit JSON data 100 times:\n");
  printf ("    echo -n '{\"key\":\"value\"}' | %s -c -n 100\n", program);
  printf ("  Fetch block information:\n");
  printf ("    %s -k 20\n", program);
}

int
main (int argc, char **argv)
{
  int cflag = 0;
  char *kopt = NULL;
  char *nopt = NULL;
  int c;
  char path[(255 + 2)] =
    { '\0' };
  int path_len;
  int empty_coll;
  struct shad_index_handle index;
  struct shad_collection_handle coll;
  shad_block_t *root = NULL;
  char *buffer = NULL;
  size_t input_len;
  shad_block_t *block = NULL;
  shad_block_t *parent = NULL;
  shad_ordinal_t ordinal;
  size_t copy_it, copy_count = 1;

  if (argc < 2)
    {
      shad_usage (argv[0]);
      exit (EXIT_FAILURE);
    }

  while ((c = getopt (argc, argv, "ck:n:")) != -1)
    {
      switch (c)
	{
	case 'c':
	  cflag = 1;
	  break;

	case 'k':
	  kopt = optarg;
	  break;

	case 'n':
	  nopt = optarg;
	  break;

	case '?':
	  if (optopt == 'c')
	    {
	      fprintf (stderr, "Option -%c requires an ordinal identifier\n",
		       optopt);
	    }
	  else
	    {
	      fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
	    }
	  break;

	default:
	  shad_usage (argv[0]);
	  exit (EXIT_FAILURE);
	}
    }

  /* current directory is where the data files are stored */
  if (getcwd (path, 255) == NULL)
    exit (EXIT_FAILURE);
  path_len = strlen (path);
  path[path_len] = '/';

  /* c - commit */
  if (cflag == 1)
    {
      if (nopt != NULL && sscanf (nopt, "%zu", &(copy_count)) == -1)
	exit (EXIT_FAILURE);

      path[(path_len + 1)] = 'i';

      /* load the index */
      index.fd = open (path, SHAD_WRITE_FLAGS, SHAD_OPEN_MODE);
      if (index.fd == -1)
	exit (EXIT_FAILURE);
      if (fstat (index.fd, &(index.stat)) != -1)
	{
	  index.seq_head = (index.stat.st_size / sizeof(index.seq_head));

	  path[(path_len + 1)] = 'c';

	  /* to check later for creating the root block */
	  empty_coll = stat (path, &(index.stat));
	  if (!empty_coll && index.stat.st_size == 1)
	    {
	      empty_coll = 1;
	    }

	  /* load the collection */
	  coll.fd = open (path, SHAD_WRITE_FLAGS, SHAD_OPEN_MODE);
	  if (coll.fd != -1)
	    {
	      /* generate the root block, if the logs is empty */
	      if (empty_coll)
		{
		  root = shad_generate_root_block (&(index), &(coll));
		}

	      if (!empty_coll || (empty_coll && root != NULL))
		{
		  buffer = malloc (SHAD_INPUT_BUFFER_SIZE);
		  if (buffer != NULL)
		    {
		      input_len = fread (buffer, 1, SHAD_INPUT_BUFFER_SIZE,
					 stdin);
		      block = shad_block_create (buffer, input_len);
		    }
		  if (block != NULL)
		    {
		      for (copy_it = 0; copy_it < copy_count; copy_it++)
			{
			  parent = shad_fetch_block (index.seq_head, &(index),
						     &(coll));
			  if (parent != NULL)
			    {
			      if (shad_block_commit (block, parent, &(index),
						     &(coll), &(ordinal)) == 0)
				{
				  printf ("%zu\n", ordinal);
				}
			      free (parent->data);
			      shad_block_destroy (parent);
			    }
			}
		    }
		}
	    }
	}
    }

  /* i - ordinal identifier */
  else if (kopt != NULL)
    {
      path[(path_len + 1)] = 'i';

      /* load the index */
      index.fd = open (path, SHAD_READ_FLAGS, SHAD_OPEN_MODE);
      if (index.fd == -1)
	exit (EXIT_FAILURE);
      if (fstat (index.fd, &(index.stat)) != -1)
	{
	  index.seq_head = (index.stat.st_size / sizeof(index.seq_head));

	  path[(path_len + 1)] = 'c';

	  /* load the collection */
	  coll.fd = open (path, SHAD_READ_FLAGS, SHAD_OPEN_MODE);
	  if (coll.fd != -1)
	    {
	      /* cast the ordinal identifier */
	      if (sscanf (kopt, "%zu", &(ordinal)))
		{
		  block = shad_fetch_block (ordinal, &(index), &(coll));
		  if (block != NULL)
		    {
		      shad_print_block (block);
		    }
		}
	    }
	}
    }

  if (buffer != NULL)
    {
      free (buffer);
    }
  if (root != NULL)
    {
      shad_block_destroy (root);
    }
  if (block != NULL)
    {
      shad_block_destroy (block);
    }
  close (index.fd);
  close (coll.fd);

  return EXIT_SUCCESS;
}
