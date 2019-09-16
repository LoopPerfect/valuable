#!/bin/bash

if [ $BUCKAROO_USE_BAZEL ]
then

./bin/buckaroo install
bazel build //:valuable

else

./bin/buckaroo install
./bin/buck build -c ui.superconsole=DISABLED //:valuable
./bin/buck test -c ui.superconsole=DISABLED //...

fi
