# A simple Dockerfile that might be used to compile the project
# for running isolated tests

FROM debian:latest as builder

RUN apt update && apt install -y build-essential git cmake libmosquitto-dev libgtest-dev mosquitto psmisc valgrind libssl-dev

RUN mkdir -p /install/usr/include/ /install/usr/lib/ && \
    git clone https://github.com/eclipse/paho.mqtt.c.git && \
    cd paho.mqtt.c && git checkout v1.3.8 && \
    cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON && \
    cmake --build build/ --target package && \
    tar -xf build/*.tar.gz -C /install/usr --strip-components=1

RUN cd / && git clone https://github.com/eclipse/tahu.git && cd tahu/c/core && make -j4 && \
    cp lib/* /install/usr/lib/ && cp include/* /install/usr/include/

RUN cd /usr/src/gtest && cmake CMakeLists.txt && make && cp lib/*.a /install/usr/lib/ && cp -r include/* /install/usr/include/ && \
    cd /usr/src/googletest/googlemock/ &&  mkdir build && cmake .. && make && cp lib/*.a /usr/lib/ && cp -r include/* /install/usr/include/


RUN cp -r /install/* /

ENV LD_LIBRARY_PATH=/usr/local/lib:/cpp_sparkplug/build/lib

COPY ./ /cpp_sparkplug/

WORKDIR /cpp_sparkplug

ENTRYPOINT ["/bin/bash"]
