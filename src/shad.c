#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "shad_types.h"
#include "shad_block.h"
#include "collection.h"
#include "shad_index.h"
#include "shad_chaining.h"
#include "shad_databasing.h"
#include "shad.h"

#define SHAD_INPUT_BUFFER_SIZE 2048

#ifndef SHAD_DATA_ATOMICITY
#define SHAD_WRITE_FLAGS O_RDWR | O_APPEND | O_CREAT
#else
#define SHAD_WRITE_FLAGS O_RDWR | O_APPEND | O_SYNC | O_CREAT
#endif

#define SHAD_READ_FLAGS O_RDONLY
#define SHAD_OPEN_MODE S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP

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
  struct shad_index_cursor index;
  struct shad_collection_cursor coll;
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
			      if (shad_commit_block (block, parent, &(index),
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
