# espi-mbus

An MBus application for ESPi (ESP8266 based Raspberry Pi form factor board), controlling an [MBus Master Hat](https://www.packom.net/m-bus-master-hat/).

## Building and Flashing

To build use the [piersfinlayson/esp8266-build container](https://hub.docker.com/r/piersfinlayson/esp8266-build), which includes the [esp-open-sdk](https://github.com/pfalcon/esp-open-sdk) and other tools necessary to build and flash apps for the ESP8266.

You'll need to run the container with a command like this, changing:
* The local volume builds are stored in (from ~/container-data/builds to the directory of your choice).
* The host's tty device for your ESPi (or other ESP8266 board) (from /dev/serial0 here).  Make sure it is mapped to /dev/ttyUSB0 though, or you will need to change the Makefile.

```
docker run --rm --name esp8266-build-usb -ti -h esp8266-build-usb -v ~/container-data/builds:/home/esp/builds --device /dev/serial0:/dev/ttyUSB0 piersfinlayson/esp8266-build:latest
```

Then clone this repo:

```
git clone https://github.com/packom/espi-mbus
```

Then build and flash it:

```
cd espi-mbus
make
make flash_app
```

To connect to the ESP serial device to monitor any diagnostics output:

```
make con
```

## Operation

Right now the app issues a data request to device primary address 48, using ESP GPIO 4 as TX and GPIO 5 as RX, and outputs the response as decoded XML.

## libmbus

espi-mbus includes [libmbus](https://github.com/rscada/libmbus).  libmbus's license is as follows:

BSD 3-Clause License

Copyright (c) 2010-2012, Raditex Control AB
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
