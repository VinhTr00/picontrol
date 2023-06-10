# Raspberry Pi with PCA9685 and SBus Protocol

## Features
- Using `I2C` to communicate with PCA9685
- Using `built-in UART` to read sbus 

## Getting started
This document asumes that you have already configured network/wifi connectivity on your Raspberry Pi.

```
$ sudo apt update && sudo apt upgrade
$ sudo apt-get install git
```

### Install WiringPi
```
$ git clone https://github.com/WiringPi/WiringPi.git
$ cd WiringPi
$ ./build
```

### Install Project
```
$ git clone -b dev/sbus https://github.com/VinhTr00/raspberry-with-pca9685
$ cd raspberry-with-pca9685
$ make
```

### How to run
```
$ ./build/main
```

