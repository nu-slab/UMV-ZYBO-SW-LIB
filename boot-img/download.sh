# !/bin/sh
sudo mount /dev/mmcblk0p1 ~/mnt-pt
sudo cp ~/boot-img/BOOT.BIN ~/boot-img/image.ub ~/boot-img/boot.scr ~/mnt-pt
sudo umount ~/mnt-pt
sudo reboot
