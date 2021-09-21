#ifndef SHAD_FORKING_H_
#define SHAD_FORKING_H_

#include "shad_types.h"
#include "shad_forklist.h"

int
shad_load_forklist (char*, struct shad_forklist_cursor*);

shad_ordinal_t
shad_forklist_next (struct shad_forklist_cursor*);

int
shad_bootstrap_forks (char*, struct shad_forklist_cursor*);

#endif /* SHAD_FORKING_H_ */
