FROM ubuntu:20.04
WORKDIR /

# apt requirements
RUN \
        apt -y update && \
        DEBIAN_FRONTEND=noninteractive apt install -y --reinstall --no-install-recommends \
        ca-certificates cmake gcc git g++ make

# setup opencv
RUN \
        mkdir -p opencv && \
        cd opencv && \
        git clone --depth 1 --branch 4.4.0 https://github.com/opencv/opencv.git && \
        mkdir -p build && \
        cd build && \
        cmake ../opencv && \
        cmake --build . --target install /usr/local
