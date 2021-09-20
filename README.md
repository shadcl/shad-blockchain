# Shad blockchain
Tiny blockchain I developed to support my arguments in an ongoing debate I'm enduring with a few colleagues.

#### Get Started
Following commands will build from source, install the binary and run a few example operations.
```sh
autoreconf -fi
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

#### Dependencies
mhash &mdash; http://mhash.sourceforge.net/
