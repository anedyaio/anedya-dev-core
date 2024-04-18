# anedya-dev-core
A core library is written in C++ and develops core for almost all functionalities of operations with Anedya Platform.

This core library makes no assumptions about the underlying architecture and thus it is assumed that no standard C libraries are 
available at runtime. This library requires interfaces to be implemented by the platform layer.

For example, an Embedded system might not have alloc(), malloc() or free() defined, where it needs to work with statically allocated space.

See config.h for tuning operations of this library.