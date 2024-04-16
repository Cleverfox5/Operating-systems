#!/bin/bash
g++ -c lab_7_1.cpp
g++ -o lab_7_1 lab_7_1.o -lpthread
sudo setcap cap_sys_resource=eip lab_7_1
g++ -c lab_7_2.cpp
g++ -o lab_7_2 lab_7_2.o -lpthread
sudo setcap cap_sys_resource=eip lab_7_2
