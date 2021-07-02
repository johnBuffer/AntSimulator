# Get image from Docker Hub
FROM iorfanidi/ubuntu-20.04-gcc-cmake-git:latest

# Specify the working directory
WORKDIR /AntSimulator

# Copy the current folder which contains C++ source code to the Docker image
COPY . /AntSimulator

# Update apps on the base image
RUN apt-get update && \
    apt-get install -y \
    # Install additional libraries
    libsfml-dev \
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
    && \
    # AntSimulator Build
    cd ../../.. && cd AntSimulator/ && \
    rm -rf build && mkdir build && cd build && \
    cmake cmake -DCMAKE_PREFIX_PATH=/usr/local/cmake/SFML/ -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build .
