#!/bin/bash

buckaroo install

if [ $BUCKAROO_USE_BAZEL ]
then

bazel build //:valuable

else

buck build -c ui.superconsole=DISABLED //:valuable
buck test -c ui.superconsole=DISABLED //...

fi
