#ifndef SHAD_COLLECTION_H_
#define SHAD_COLLECTION_H_

#include <sys/stat.h>

struct shad_collection_cursor
{
  int fd;
  struct stat stat;
};

#endif /* SHAD_COLLECTION_H_ */
