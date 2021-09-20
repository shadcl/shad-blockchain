# Shad blockchain
A tiny blockchain I developed for reference in an ongoing debate with a few colleagues regarding embedded blockchain viability.

#### Get Started
Following commands will build from source, install the binary and run a few example operations.
```sh
autoreconf -i
./configure
make
sudo make install
echo -n "this string" | shad -c
echo -n "no more strings" | shad -c -n 100
shad -k 25
shad -k 73
```

##### Sample output
```
hash ------------------------------------------------------------------------------------------
6f239f22a146370d041d250b949f129214181ca1eee93a60b4871528f5a
parent hash -----------------------------------------------------------------------------------
1e4e6386eda03fce36f7fb73eb62ea2fbddceb8ae613aa3d8346b63791802d
data ------------------------------------------------------------------------------------------
6e 6f 20 6d 6f 72 65 20 73 74 72 69 6e 67 73 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
```

#### Compile with atomicity for compliance with Two-phase commit protocol standards.
Committing to the blockchain will be slower, but data integrity will be greater.
```sh
-DSHAD_DATA_ATOMICITY
```

#### Using your own hashing algorithm and output format
If you want to use your own hashing algo and output format, here are example definitions.
```c
#include <shad.h>

/* Define your hashing logic
 */
int
shad_block_hash (shad_block_t *block, shad_block_t *parent)
{
  int ret = -1; /* 0 (success) or -1 (failure) */
  /* TIGER HASH? */
  return ret;
}

void
shad_print_block (shad_block_t *block)
{
    /* Print JSON to stdout? */
}
```

#### Dependencies
mhash &mdash; http://mhash.sourceforge.net/

#### C89
Compile with c89 yourself, but it _should_ be compatible. It wasn't a requirement.
