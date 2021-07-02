#!/bin/bash

docker build -t myimages/ant-simulator:latest .
docker create -it --name dummy myimages/ant-simulator:latest bash
docker cp dummy:/AntSimulator/build/AntSimulator .
rm -rf build/
mkdir build/
mv AntSimulator build/
cp -r res/ build/
docker rm -f dummy
