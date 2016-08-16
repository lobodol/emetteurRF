# RF emitter
This project is the hardware part of my quadcopter remote project.
This Arduino sketch reads data from serial port (sent by smartphone) and send it through RF 433 MHz chanel using Virtual Wire library.

Here what the whole looks like :
```
               \/_   ))))) RF 433MHz ))))   _\/
               /                              \
   +----------+                                +---------+
   | Arduino  |                                | Arduino |                +------------+     
   | receiver |                                | emitter |---/USB wire/---| smartphone |
   +----------+                                +---------+                +------------+
```