
# cpp_sparkplug - A C++ Sparkplug library
This project is to solve a requirement for easy Sparkplug implementation on embedded systems. It is built upon the tahu c library and adds easy to implement c++ functionality. Currently the project is built with linux and pico support.

## WIP
This project is a big work in progress. I have implemented enough of a base functionality to get it off the ground, however a lot more needs to be added. I'll be doing my best to constantly update this project and continue to add more features. The idea of the framework is to use a single core Sparkplug base framework built around the Node class, with support for other embedded systems coming from different SparkplugClient implementations.

## Software Goals
- [ ] Arduino Pub/Sub Client
- [X] Cpp Controller Client
- [X] Comprehensive E2E Tests
- [X] Expand Unit Tests
- [X] Property Support
- [X] Command Support
- [ ] Template Support
- [ ] DataSet Support

## Building
This library uses cmake for building and dependency management.
The following build flags are supported:
| Flag  | Default | Description |
| ------------- | ------------- |  ------------- |
| BUILD_TARGET | PICO | The build target for the project. Pico requires pico sdk to be available. |
| FETCH_REMOTE | ON | Whether to fetch remote dependencies through cmake. If disabled, the remote dependencies can be put within {PROJECT_ROOT}/external. |
| CPP_SPARKPLUG_TESTS | OFF | Whether tests will be compiled. Forcibly disabled when building for PICO |
| CPP_SPARKPLUG_PAHO | OFF | Whether to compile PAHO clients. Forcibly disabled when building for PICO |

## Dependencies
The following dependencies will be pulled and built by cmake:
- https://github.com/kylehofer/pico_tcp_client.git
- https://github.com/kylehofer/cpp_mqtt_client.git
### Only when building for PICO
- https://github.com/kylehofer/pico_tcp_client.git
### Only when building with CPP_SPARKPLUG_PAHO
- https://github.com/eclipse/paho.mqtt.c.git
### Only when building with CPP_SPARKPLUG_TESTS
- https://github.com/eclipse/mosquitto.git
- https://github.com/google/googletest.git
