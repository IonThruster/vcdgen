# VCDGEN
A single header c++ library for VCD file generation

### Getting started
```
makdir build
cd build
cmake ..
make
./tests/vcdgen_tester > sample.vcd
gtkwave sample.vcd
```
This should dump to the console a simple VCD file which can be viewed on Open-Source waveform viewers like gtkwave

### Dependencies
* cmake 3.10.0+
* c++ compiler with C++11 support
* gtkwave / a similar GUI program to view waveforms

### Tested on
* g++ 7.5.0
* gtkwave (sudo apt-get install gtkwave)
