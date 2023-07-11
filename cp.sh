#!/bin/bash
sudo umount /mnt/usb

echo "prepare keyboard in dfu mode ..."
echo "in 3 seconds ..."
sleep 3

sh ./build_with_docker.sh

sudo mount /dev/sda1 /mnt/usb
sudo cp sekigon_crkbd_ec_tcy.uf2 /mnt/usb
sudo umount /mnt/usb
