#!/bin/bash

curl -L https://github.com/seccomp/libseccomp/releases/download/v2.5.5/libseccomp-2.5.5.tar.gz > seccomp.tar.gz
tar -xvzf seccomp.tar.gz
cd libseccomp-2.5.5
sudo apt-get update -y
sudo apt-get install -y gperf
./configure
make V=1
sudo make install
