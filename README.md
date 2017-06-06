# Retrode Linux driver

After compiling with `make` and `make install` (the latter as root or through `sudo`),
you'll need to create a few files to load the driver correctly.

Create a `/etc/udev/load_hid_retrode.sh` file with the content:
```
#!/bin/bash
DRIVER=$1
DEVICE=$2
HID_DRV_PATH=/sys/bus/hid/drivers
/sbin/modprobe hid_retrode_standalone
echo ${DEVICE} > ${HID_DRV_PATH}/hid-generic/unbind
echo ${DEVICE} > ${HID_DRV_PATH}/hid-retrode/bind
```

Create a `/etc/udev/rules.d/80-retrode.rules` file with the content:
```
DRIVER=="hid-generic", ENV{MODALIAS}=="hid:b0003g*v00000403p000097C1", RUN+="/bin/sh /etc/udev/load_hid_retrode.sh hid-generic %k"
```

After a reboot, the driver should be loaded automatically when the dongle gets plugged in.
