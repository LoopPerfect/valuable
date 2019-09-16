#!/bin/bash

buckaroo install

if [ $BUCKAROO_USE_BAZEL ]
then

bazel build //:valuable

else

./bin/buck build -c ui.superconsole=DISABLED //:valuable
./bin/buck test -c ui.superconsole=DISABLED //...

fi
