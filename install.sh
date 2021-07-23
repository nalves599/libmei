#!/bin/bash

sudo apt install python python-pip cmake build-essential python-dev python-setuptools libboost-python-dev libboost-thread-dev uuid-dev -y

mkdir build
cd build

cmake ..
make
make install

cd ../python
python setup.py build
python setup.py install

echo "pymei package installed"
