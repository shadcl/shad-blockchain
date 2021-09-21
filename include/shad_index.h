#ifndef SHAD_INDEX_H_
#define SHAD_INDEX_H_

#include "shad_types.h"

#include <sys/stat.h>

struct shad_index_cursor
{
  int fd;
  struct stat stat;
  shad_ordinal_t seq_head;
};

#endif /* SHAD_INDEX_H_ */
