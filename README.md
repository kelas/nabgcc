
```
sudo apt install gcc-arm-none-eabi libnewlib-arm-none-eabi picolibc-arm-none-eabi
make
```

```
--specs=nosys.specs

If you want to have your custom implementation of those missing system calls, is to create your own,
minimal syscalls.c from the newlib libgloss/arm/syscalls.c source file: you can implement some system calls,
and stubbing the others you don't need. newlib source code can be browsed from here, and see section 12 
System Calls and 12.1 Definitions for OS interface of the newlib documentation for more details.
```



