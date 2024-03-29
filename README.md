
# cpp_sparkplug - A C++ Sparkplug library
This project is to solve a requirement for easy Sparkplug implementation on embedded systems. It is built upon the tahu c library and adds easy to implement c++ functionality. Currently only supported on linux platforms.

## WIP
This project is a work in progress, and I'm adding functionality to keep it up to date with the Sparkplug spec. The idea of the framework is to use a single core Sparkplug base framework built around the Node class, with support for other embedded systems coming from different SparkplugClient implementations.

## Software Goals
- [X] Cpp Controller Client
- [X] Comprehensive E2E Tests
- [X] Expand Unit Tests
- [X] Property Support
- [X] Command Support
- [X] Primary Host Support
- [ ] Template Support
- [ ] DataSet Support

## Building
This library uses cmake for building and dependency management.
The following build flags are supported:
| Flag  | Default | Description |
| ------------- | ------------- |  ------------- |
| BUILD_TARGET | LINUX | The build target for the project. |
| FETCH_REMOTE | ON | Whether to fetch remote dependencies through cmake. If disabled, the remote dependencies can be put within {PROJECT_ROOT}/external. |
| CPP_SPARKPLUG_TESTS | ON | Whether tests will be compiled. |
| CPP_SPARKPLUG_STATIC | OFF | Builds as a static library. |
| CPP_SPARKPLUG_SHARED | ON | Builds as a shared library. |
| CPP_SPARKPLUG_PAHO_ASYNC | ON | Whether to compile PAHO Async clients. |
| CPP_SPARKPLUG_PAHO_SYNC | ON | Whether to compile PAHO Sync clients. |
| CPP_SPARKPLUG_MQTT | ON | Whether to compile CPP MQTT clients. |
| CPP_SPARKPLUG_EXAMPLES | ON | Whether to compile examples. |

## Dependencies
The following dependencies will be pulled and built by cmake:
### Only when building with CPP_SPARKPLUG_MQTT
- https://github.com/kylehofer/cpp_mqtt_client.git
### Only when building with CPP_SPARKPLUG_PAHO_ASYNC or CPP_SPARKPLUG_PAHO_SYNC
- https://github.com/eclipse/paho.mqtt.c.git
### Only when building with CPP_SPARKPLUG_TESTS
- https://github.com/eclipse/mosquitto.git
- https://github.com/google/googletest.git
