#ifndef SHAD_FORKLIST_H_
#define SHAD_FORKLIST_H_

#include "shad_types.h"

struct shad_forklist_cursor
{
  int fd;
  shad_ordinal_t seq_current;
};

#endif /* SHAD_FORKLIST_H_ */
