#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "shad_types.h"
#include "shad_filing.h"
#include "shad_forklist.h"
#include "shad_forking.h"

int
shad_load_forklist (char *path, struct shad_forklist_cursor *forks)
{
  forks->fd = open (path, O_RDWR | O_APPEND | O_CREAT,
  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
  forks->seq_current = 0;

  return 0;
}

shad_ordinal_t
shad_forklist_next (struct shad_forklist_cursor *forks)
{
  shad_ordinal_t ordinal;

  if (read (forks->fd, &(ordinal), sizeof(ordinal)) != sizeof(ordinal))
    return -1;

  return ordinal;
}

int
shad_bootstrap_forks (char *base_path, struct shad_forklist_cursor *forks)
{
  shad_ordinal_t ordinal;
  char ordinal_str[(255 + 1)];
  char *fork_dir, *fork_path;
  char *_base_path;
  pid_t pid;

  ordinal = shad_forklist_next (forks);
  while (ordinal != -1)
    {
      sprintf (ordinal_str, "%d", ordinal);
      fork_dir = shad_path_join (2, base_path, ordinal_str);
      pid = fork ();

      if (pid == 0)
	{
	  _base_path = base_path;
	  base_path = fork_dir;
	  shad_path_free (base_path);
	  free (base_path);
	  close (forks->fd);
	  shad_path_free (fork_path);
	  fork_path = shad_path_join (2, base_path, "f");
	  shad_load_forklist (fork_path, forks);
	}
      shad_path_free (fork_path);
      ordinal = shad_forklist_next (forks);
    }
  shad_path_free (base_path);

  return 0;
}
