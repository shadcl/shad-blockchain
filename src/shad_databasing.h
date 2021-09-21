#ifndef SHAD_DATABASING_H_
#define SHAD_DATABASING_H_

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

int
shad_collection_add_block (shad_block_t*, struct shad_collection_cursor*,
			   shad_block_key_t*);

int
shad_index_add_block_key (shad_block_key_t, struct shad_index_cursor*,
			  shad_ordinal_t*);

#endif /* SHAD_DATABASING_H_ */
