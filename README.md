# Shad blockchain
Shad-blockchain is a tiny blockchain I developed for academic purposes. Someday, I might extend it to a decentralized version.

#### What is a blockchain?

The term "blockchain" is often used interchangeably with "decentralized blockchain," which causes confusion. A blockchain, as its name suggests, is simply a growing list of blocks linked together using cryptography.

Shad-blockchain is not a decentralized blockchain, but a tiny blockchain that you can add blocks to from the command line.

#### Naming conventions
Source files ending with **-ing** to form a verbal noun contain functioning related code.

* src/shad_chain**ing**.c &mdash; blockchain function definitions
* src/shad_databas**ing**.c &mdash; database function definitions

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

##### Output block sample
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
If you want to use your own hashing algo and output format, here are sample definitions.
```c
#include <shad_chaining.h>

/* Define your hashing logic
 */
int
shad_block_hash (shad_block_t *block, shad_block_t *parent)
{
  int ret = -1; /* 0 (success) or -1 (failure) */
  /* TIGER HASH? */
  return ret;
}
EXPORT_SYMBOL(shad_block_hash);

void
shad_print_block (shad_block_t *block)
{
    /* Print JSON to stdout? */
}
EXPORT_SYMBOL(shad_print_block);

/* modify the Makefile */
```

#### Dependencies
mhash &mdash; http://mhash.sourceforge.net/

#### C89
Compile with c89 yourself, but it _should_ be compliant. It wasn't a requirement.
