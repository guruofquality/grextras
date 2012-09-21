#!/bin/sh
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/john/src/grextras/python
export PATH=/home/john/src/grextras/cmake/python:$PATH
export LD_LIBRARY_PATH=/home/john/src/grextras/cmake/lib:/home/john/src/grextras/cmake/lib:/home/john/src/grextras/cmake/lib:/home/john/src/grextras/cmake/lib:/home/john/src/grextras/cmake/lib:/home/john/src/grextras/cmake/lib:/home/john/src/grextras/cmake/lib:/home/john/src/grextras/cmake/lib:$LD_LIBRARY_PATH
export PYTHONPATH=/home/john/src/grextras/cmake/python/../swig:$PYTHONPATH
/usr/bin/python2.7 -B /home/john/src/grextras/python/qa_pmt_to_python.py 
