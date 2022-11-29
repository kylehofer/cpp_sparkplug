
# cpp_sparkplug - A C++ Sparkplug library
This project is to solve a requirement for easy Sparkplug implementation on embedded systems. It is built upon the tahu c library and adds easy to implement c++ functionality. Currently the project is built based off a linux system, I hope to start adding more embedded support, for example adding support for the Arduino system.

## WIP
This project is a big work in progress. I have implemented enough of a base functionality to get it off the ground, however a lot more needs to be added. I'll be doing my best to constantly update this project and continue to add more features. The idea of the framework is to use a single core Sparkplug base framework built around the Node class, with support for other embedded systems coming from different SparkplugClient implementations.

## Software Goals
- [ ] Arduino Pub/Sub Client
- [ ] Comprehensive E2E Tests
- [ ] Expand Unit Tests
- [ ] Property Support
- [ ] Command Support
- [ ] Template Support
- [ ] DataSet Support

## Building
This library requires a few external open source projects which I have listed below. I have also included some basic instructions for getting these libraries install for Debian systems. This is only a rough guide and modifications might need to be made for your system.

### Required libraries for cpp_sparkplug
```
# Mosquitto for running E2E tests
sudo apt install -y mosquitto
```
### paho.mqtt.c - https://github.com/eclipse/paho.mqtt.c.git
```
sudo apt install -y git build-essential cmake libmosquitto-dev
git clone https://github.com/eclipse/paho.mqtt.c.git
# Pull the required branch
cd paho.mqtt.c && git checkout v1.3.8
# Prepare make files
cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON
# Install as package which we'll use to install
# See the paho readme for alternative methods of installation
cmake --build build/ --target package
# Extract the tar into the /usr/local/ directory which will add into /usr/local/lib and /usr/local/include
sudo tar -xf build/*.tar.gz -C /usr/local --strip-components=1
sudo ldconfig
```

### tahu c - https://github.com/eclipse/tahu.git
```
sudo apt install -y git build-essential libmosquitto-dev
git clone https://github.com/eclipse/tahu.git
# Move to the required directory
cd tahu/c/core
# Compile source
make
# Move libraries and headers into our /usr/local
sudo cp lib/* /usr/local/lib/
sudo cp include/* /usr/local/include/
sudo ldconfig
```

### Google Test (gtest and gmock) - https://github.com/google/googletest
```
sudo apt install -y libgtest-dev
# gtest
cd /usr/src/gtest
# Prepare Make
cmake CMakeLists.txt
# Compile
make
# Move libraries and headers into our /usr/local
sudo cp lib/*.a /usr/local/lib/
sudo cp -r include/* /usr/local/include/

# gmock
cd /usr/src/googletest/googlemock/
mkdir build
# Prepare Make
cmake ..
# Compile
make
# Move libraries and headers into our /usr/local
sudo cp lib/*.a /usr/local/lib/
sudo cp -r include/* /usr/local/include/
```