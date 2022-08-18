#!/usr/bin/env bash

a=model.*cl_function_gcda.tar

b=$(echo $a | sed 's,\*,\.,g; s,\.,-,g')
echo $b

