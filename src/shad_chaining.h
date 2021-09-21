#ifndef SHAD_CHAINING_H_
#define SHAD_CHAINING_H_

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>

#include "shad_types.h"
#include "shad_block.h"
#include "shad_collection.h"
#include "shad_index.h"

shad_block_t*
shad_block_create (char*, size_t);

void
shad_block_destroy (shad_block_t*);

shad_nonce_t
shad_get_nonce ();

extern int
shad_block_hash (shad_block_t*, shad_block_t*);

int
shad_commit_block (shad_block_t*, shad_block_t*, struct shad_index_cursor*,
		   struct shad_collection_cursor*, shad_ordinal_t*);

shad_block_t*
shad_generate_root_block (struct shad_index_cursor*,
			  struct shad_collection_cursor*);

shad_block_t*
shad_fetch_block (shad_ordinal_t, struct shad_index_cursor*,
		  struct shad_collection_cursor*);

extern void
shad_print_block (shad_block_t*);

#endif /* SHAD_CHAINING_H_ */
