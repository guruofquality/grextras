#!/bin/sh
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/john/src/pre-cog/python
export PATH=/home/john/src/pre-cog/build/python:$PATH
export LD_LIBRARY_PATH=/home/john/src/pre-cog/build/lib:/home/john/src/pre-cog/build/lib:/home/john/src/pre-cog/build/lib:/home/john/src/pre-cog/build/lib:/home/john/src/pre-cog/build/lib:/home/john/src/pre-cog/build/lib:/home/john/src/pre-cog/build/lib:/home/john/src/pre-cog/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=/home/john/src/pre-cog/build/python/../swig:$PYTHONPATH
/usr/bin/python2.7 -B /home/john/src/pre-cog/python/qa_pmt_to_python.py 
