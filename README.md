# anedya-dev-core
A core library is written in C and provides almost all functionalities of operations with the Anedya Platform.

This core library makes no assumptions about the underlying architecture and thus it is assumed that very minimal standard C libraries are 
available at runtime. This library requires interfaces to be implemented by the platform layer.

For example, an Embedded system might not have alloc(), malloc(), or free() defined, where it needs to work with statically allocated space.

See anedya_config.h for tuning operations of this library.

## anedya-dev-core Architecture

![](/docs/res/Anedya_core_concept.png)

The core SDK contains only functional code which is required to interact with the Anedya platform. The core library does not include:
- TLS Implementation stack
- MQTT or HTTP implementation stack
- Timekeeping implementation to work with RTC and other timers in the system.

Core library is designed to be highly portable. As shown in the figure, the core library is never used directly by the end application.
Rather, it is included in the device platform-specific SDK. 

### Platform Layer

The core library depends on the platform layer to function, which is specific to the hardware being used. The core library declares some APIs
in `anedya_interface.h` which needs to be defined by the platform layer. So just by implementing the methods defined in `anedya_interface.h` the library can be
ported to any hardware platform.

In general, the platform layer is responsible for handling:
- TLS connection
- MQTT or HTTP connection state
- IPv4 or IPv6 stack
- Connection through some Modems

The core library does not have any other dependency apart from the platform layer.