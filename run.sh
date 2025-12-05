#!/bin/bash

clear
make clean
make
bin/inmemorydb_test "$@"
