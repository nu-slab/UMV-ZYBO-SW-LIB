# !/bin/sh
sudo cp -ar lib/* /usr/local/lib
sudo cp -r share/* /usr/local/sahre
sudo cp -r include/* /usr/local/include
sudo cp -r bin/* /usr/local/bin
sudo cp -r share/* /usr/local/share

sudo ldconfig
