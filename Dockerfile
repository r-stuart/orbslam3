FROM opencv AS build
WORKDIR /

# setup prereqs
RUN \
        apt -y update && \
        DEBIAN_FRONTEND=noninteractive apt install -y --reinstall --no-install-recommends \
        ca-certificates cmake gcc git g++ libboost-dev libboost-serialization-dev libssl-dev make sudo

# setup pangolin
RUN \
        mkdir -p pangolin && \
        cd pangolin && \
        git clone --recursive https://github.com/stevenlovegrove/Pangolin.git && \
        yes | ./Pangolin/scripts/install_prerequisites.sh recommended && \
        mkdir -p build && \
        cd build && \
        cmake ../Pangolin && \
        cmake --build . --target install /usr/local

COPY mono_loop.cc .

# setup orbslam
RUN \
        git clone https://github.com/UZ-SLAMLab/ORB_SLAM3.git ORB_SLAM3 && \
        cd ORB_SLAM3

WORKDIR /ORB_SLAM3

COPY mono_loop.cc .
 
RUN \
        sed -i 's/++11/++14/g' CMakeLists.txt && \
        sed -i '140,390d' CMakeLists.txt && \
        echo 'set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})' >> CMakeLists.txt && \
        echo 'add_executable(mono_loop mono_loop.cc)' >> CMakeLists.txt && \
        echo 'target_link_libraries(mono_loop ${PROJECT_NAME})' >> CMakeLists.txt && \
        chmod +x build.sh && \
        ./build.sh && \
        sudo ldconfig

RUN mkdir -p /copylibs/
WORKDIR /copylibs/

RUN \
        ldd /ORB_SLAM3/mono_loop | grep '=>' | tr -s " " | cut -d' ' -f 3 | xargs -I '{}' cp --parents -H '{}' /copylibs/ && \
        cp --parents /ORB_SLAM3/mono_loop /copylibs && \
        cp --parents /ORB_SLAM3/Examples/Monocular/EuRoC.yaml /copylibs && \
        cp --parents /ORB_SLAM3/Vocabulary/ORBvoc.txt /copylibs
        

FROM ubuntu:20.04
WORKDIR /

COPY --from=build . ./

ENTRYPOINT ["/ORB_SLAM3/mono_loop", "/ORB_SLAM3/Vocabulary/ORBvoc.txt", "/ORB_SLAM3/Examples/Monocular/EuRoC.yaml"]
