# Wordclock

Firmware for a Wordclock based on the ESP8266, powered by the [Sming Framework](https://github.com/SmingHub/Sming).

## Setup

The setup of this repo is based on devcontainers. To all that's required to use it is Docker and an IDE like VS Code to start and connect to the dev container.

### Flashing

There is currently an issue with forwarding devices to containers in Docker Desktop. So the easiest way of interacting with a device connected to the host maschine is the Telnet protocol (sounds stupid but that's the recommended way from Espressif: [Docs](https://docs.espressif.com/projects/esp-idf/en/v5.1/esp32/api-guides/tools/idf-docker-image.html#using-remote-serial-port))

First start the esptool server on your device:

```bash
esp_rfc2217_server.py -v -p 4000 /dev/cu.usbserial-120
```

Then initiate the flashing process from the dev container:

```bash
make flash COM_PORT=rfc2217://host.docker.internal:4000?ign_set_control
```

### Building the file system image with the frontend

The IFS (Inmemory File System) will automatically be built when running `make` for the first time.
However, this is only the case if the image isn't cached already. Sming can't track changes to its underlying files so in order for changes to the frontend to be reflected in the firmware flashed to the device you need to run:

```bash
make buildpart
```

### Release

Generating a single binary:

```bash
/opt/sming/Sming/Components/esptool/esptool/esptool.py --chip esp8266 merge_bin -o ./wordclock-v1_0_0.bin --flash_mode dio --flash_freq 40m --flash_size 4MB 0x00000 ./out/Esp8266/release/firmware/rboot.bin 0x00002000 ./out/Esp8266/release/firmware/rom0.bin 0x000fa000 ./out/fwfs0.bin 0x003fa000 ./out/Esp8266/release/firmware/partitions.bin 0x003fc000 /opt/sming/Sming/Arch/Esp8266/Components/esp8266/sdk/bin/esp_init_data_default.bin
```
