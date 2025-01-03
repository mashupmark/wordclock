# Wordclock

## Setup

The setup of this repo is based on devcontainers. To all that's required to use it is Docker and an IDE like VS Code to start and connect to the dev container.

Tipp: You might want to run `make` after starting up the container the first time to build all dependencies and get rid of errors shown inside the IDE.

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