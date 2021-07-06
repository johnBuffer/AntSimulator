# Get image from Docker Hub
FROM ubuntu:18.04

# Specify the working directory
WORKDIR /AntSimulator

# Copy the current folder which contains C++ source code to the Docker image
COPY . /AntSimulator

# Update apps on the base image
RUN apt-get update && \
    apt-get install -y \
    # Install GCC, CMake, Git
    gcc cmake build-essential git \
    # Install additional libraries
    libfreetype6-dev \
    libjpeg-dev \
    xorg-dev \
    libxrandr-dev \
    xcb \
    libx11-xcb-dev \
    libxcb-randr0-dev \
    libxcb-image0-dev \
    libgl1-mesa-dev \
    libflac-dev \
    libogg-dev \
    libvorbis-dev \
    libopenal-dev \
    libpthread-stubs0-dev \
    libudev-dev \
    libxcursor-dev \
    libxinerama-dev \
    libxi-dev &&  \
    # SFML static build
    git clone https://github.com/SFML/SFML.git && \
    cd SFML && \
    rm -rf build && mkdir build && cd build && \
    cmake -G "Unix Makefiles" -DBUILD_SHARED_LIBS=FALSE -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . && \
    make install && \
    cd ../../.. && \
    # AntSimulator build
    cd AntSimulator/ && \
    rm -rf build && mkdir build && cd build && \
    cmake -DCMAKE_PREFIX_PATH=/usr/local/cmake/SFML/ -DSFML_STATIC_LIBRARIES=TRUE -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build .
