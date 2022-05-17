#!/usr/bin/bash

rm -f wwwroot/cgi_proc
g++ -o wwwroot/cgi_proc cgi/cgi_proc.cc -std=c++11
make clean;
make
