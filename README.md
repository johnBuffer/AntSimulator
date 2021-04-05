# AntSimulator

Very simple ants simulator.

# Installation

## Prerequisites

In order to compile this project you will need to:
 - have [SFML](https://www.sfml-dev.org/index.php) installed on your system. If you don't know how to do it see [this link](https://www.sfml-dev.org/tutorials/2.5/#getting-started).
 - have [CMake](https://cmake.org/) installed

## Compilation

### On Linux with `make`
 - Go in the repo folder

`cd the/repo/location`
- Create a `build` directory

`mkdir build`
 - Go into build

`cd build`
 - Execute command

`cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..`
 - Execute command

`make`

### On windows with CMake GUI and Visual Studio
 - Install the right SFML version or compile it see [this](https://www.sfml-dev.org/tutorials/2.5/start-vc.php)
 - Run CMake
 - Select the repo location
 
![Cmake 1](https://github.com/johnBuffer/AntSimulator/blob/master/img/cmake_1.PNG)
 - Click on `Configure`, if you have installed the `x64` version of SFML, in the pop up window select `x64` in the `Optionnal platform for generator` drop down

![Cmake 2](https://github.com/johnBuffer/AntSimulator/blob/master/img/cmake_2.PNG)
 - Click on `Finish`
 - Click on `Generate`

![Cmake 3](https://github.com/johnBuffer/AntSimulator/blob/master/img/cmake_3.PNG)
 - You can now open the generated project and build it.

More information here [here](https://preshing.com/20170511/how-to-build-a-cmake-based-project/)

# Commands

|Command|Action|
|---|---|
|**E**|Pause/Unpause the simulation|
|**A**|Toggle markers drawing|
|**S**|Toggle max speed mode|
|**Right click**|Add food|
|**Left click**|Move view|
|**Wheel**|Zoom|
