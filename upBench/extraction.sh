#!/bin/bash

grep "are running" full/out | grep -oE '[0-9]+' > out_of_full

grep "are running" single/out | grep -oE '[0-9]+' > out_of_single

python3 genererGrapque.py out_of_full out_of_single
