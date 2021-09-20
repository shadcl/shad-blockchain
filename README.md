# Shad blockchain
Tiny blockchain I developed to support my arguments in an ongoing debate I'm enduring with a few colleagues.

#### Get Started
Following commands will build from source, install the binary and run a few example operations.
```sh
./configure
make
sudo make install
echo -n "this string" | shad -c
echo -n "no more strings" | shad -c -n 100
shad -k 25
shad -k 73
```

#### Dependencies
mhash &mdash; http://mhash.sourceforge.net/
