#!/bin/bash

choco install buckaroo

if [ $BUCKAROO_USE_BAZEL ]
then

choco install bazel

else

choco install buck

fi
