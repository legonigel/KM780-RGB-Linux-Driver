# KM780-RGB-Linux-Driver

A driver/ configuration utility for the G.SKILL KM780 RGB keyboard on Linux

CURRENTLY A WORK IN PROGRESS


# Build

This project is created using QT Creator community version and QT 5.5.0

The exact build process or requirments have not been documented. If you would like to make this, give it a try and if it doesn't work, open an issue


# Instalation

 TODO

## udev rules

To allow the program to have write access to the keyboard, udev rules need to be added, to change the permissions to the USB device from 0664 to 0666. An example set of rules can be found in the udev_rules folder. This file can be copied to `/etc/udev/rules.d/`.

```bash
$ cp udev_rules/40-usb-km780-rgb.rules /etc/udev/rules.d/
```
