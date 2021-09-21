#ifndef SHAD_TYPES_H_
#define SHAD_TYPES_H_

#include <stddef.h>
#include <sys/types.h>

typedef off_t shad_ordinal_t;
typedef long shad_nonce_t;

typedef union
{
  unsigned char bytes[sizeof(size_t)];
  size_t value;
} shad_nsize_t;

#endif /*SHAD_TYPES_H_ */
