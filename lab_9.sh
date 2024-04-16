#!/bin/bash
g++ -c lab_9_1.cpp
g++ -o lab_9_1 lab_9_1.o -lseccomp
g++ -c lab_9_2.cpp
g++ -o lab_9_2 lab_9_2.o -lseccomp
