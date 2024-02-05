
# CPU
This example creates a Sparkplug Node which will create a Device for each logical core in your computer. It reads /proc/stat to both generate the Devices and also to fill metrics with usage data.

# Configuration
The default configuration will use the following:
## Group Name
Examples
## Node Id
Computer
## MQTT Address
tcp://localhost:1883
## Client Id
sparkplug_client_{Random Int}

All these fields can be configured using command line args, for example: ./cpu_example GroupId NodeId tcp://localhost:1883 ClientId