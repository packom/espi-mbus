# espi-mbus

An MBus application for ESPi (ESP8266 based Raspberry Pi form factor board), controlling an [MBus Master Hat](https://www.packom.net/m-bus-master-hat/).

## Building and Flashing

To build use the [piersfinlayson/esp8266-build container](https://hub.docker.com/r/piersfinlayson/esp8266-build), which includes the [esp-open-sdk](https://github.com/pfalcon/esp-open-sdk) and other tools necessary to build and flash apps for the ESP8266.

You'll need to run the container with a command like this, changing:
* The local volume builds are stored in (from ~/container-data/builds to the directory of your choice).
* The host's tty device for your ESPi (or other ESP8266 board) (from /dev/ttyUSB0 here).  Make sure it is mapped to /dev/ttyUSB0 though, or you will need to change the Makefile.

```
docker run --rm --name esp8266-build-usb -ti -h esp8266-build-usb -v ~/container-data/builds:/home/esp/builds --device /dev/ttyUSB0:/dev/ttyUSB0 piersfinlayson/esp8266-build:3.0.0
```

Then clone this repo:

```
git clone https://github.com/packom/espi-mbus
```

Then build and flash it:

```
cd espi-mbus
make flash_app
```

To connect to the ESP serial device to monitor any diagnostics output:

```
make con
```

## Operation

Right now the app simply issues a primary data request to device address 48, using ESP GPIO 4 as TX and GPIO 5 as RX, and outputs the response as hex bytes to the console.