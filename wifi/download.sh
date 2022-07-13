# !/bin/sh
sudo cp *.ko /lib/modules/`uname -r`/
sudo cp modules /etc/modules
sudo depmod
