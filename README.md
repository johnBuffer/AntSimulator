# AntSimulator

Very simple ants simulator.

# Installation

## Prerequisites

In order to compile this project you will need to:
 - have [SFML](https://www.sfml-dev.org/index.php) installed on your system. If you don't know how to do it see [this link](https://www.sfml-dev.org/tutorials/2.5/#getting-started).
 - have [CMake](https://cmake.org/) installed

## Compilation

Detailed explanation [here](https://preshing.com/20170511/how-to-build-a-cmake-based-project/)

### On Linux with `install.sh`
- Go in the repo folder

`cd the/repo/location`

- Execute `install.sh` script

`./install.sh`

##### On Ubuntu from Docker
 - Install Docker (see [this](https://docs.docker.com/engine/install/))
 - Execute `install_from_docker.sh` script
```sh
docker build -t myimages/ant-simulator:latest .
docker create -it --name dummy myimages/ant-simulator:latest bash
docker cp dummy:/AntSimulator/build/AntSimulator .
rm -rf build/
mkdir build/
mv AntSimulator build/
cp -r res/ build/
docker rm -f dummy
```
Executable files will be located in the build folder.

### On Windows with CMake GUI and Visual Studio
 - Install the right SFML version or compile it (see [this](https://www.sfml-dev.org/tutorials/2.5/start-vc.php))
 - Run CMake
 - Select the repo location
 
![Cmake 1](https://github.com/johnBuffer/AntSimulator/blob/master/img/cmake_1.PNG)
 - Click on `Configure`, if you have installed the `x64` version of SFML, in the pop up window select `x64` in the `Optionnal platform for generator` drop down

![Cmake 2](https://github.com/johnBuffer/AntSimulator/blob/master/img/cmake_2.PNG)
 - Click on `Finish`
 - Click on `Generate`

![Cmake 3](https://github.com/johnBuffer/AntSimulator/blob/master/img/cmake_3.PNG)
 - You can now open the generated project and build it.

# Commands

|Command|Action|
|---|---|
|**P**|Pause/Unpause the simulation|
|**M**|Toggle markers drawing|
|**A**|Toggle ants drawing|
|**S**|Toggle max speed mode|
|**W**|Toggle Wall mode|
|**E**|Toggle Wall erase mode|
|**Right click**|Add food|
|**Left click**|Move view|
|**Wheel**|Zoom|
