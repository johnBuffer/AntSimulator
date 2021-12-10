#!/bin/sh

docker build -t ant-simulator:latest .
docker create -it --name dummy ant-simulator:latest bash
docker cp dummy:/AntSimulator/build/AntSimulator .
rm -rf build/
mkdir build/
mv AntSimulator build/
cp -r res/ build/
docker rm -f dummy
