#!/bin/bash -e
set -e

if ! command -v cmake &> /dev/null
then
    echo "cmake is required for building AntSimulator, installing"
    sudo apt install cmake -y
    echo "cmake installed"
fi

if ! (dpkg -l | grep libsfml-dev &> /dev/null)
then
  echo "libsfml-dev is required for building AntSimulator, installing"
  sudo apt install libsfml-dev -y
  echo "libsfml-dev installed"
fi

mkdir -p build && cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make

echo -e "\nTo start AntSimulator run ./build/AntSimulator"
