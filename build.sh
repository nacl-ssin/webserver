#!/usr/bin/bash

g++ -o cgi/cgi_proc cgi/cgi_proc.cc -std=c++11

cp cgi/cgi_proc static

make clean;
make
