#! /bin/bash


sudo apt-get -y install make gcc bison flex libssl-dev musl-tools
sudo apt-get -y install libncurses5-dev openssl 
sudo apt-get -y install build-essential 
sudo apt-get -y install pkg-config
sudo apt-get -y install libc6-dev
sudo apt-get -y install libelf-dev
sudo apt-get -y install zlibc minizip 
sudo apt-get -y install libidn11-dev libidn11
sudo apt-get -y install bc
sudo apt-get -y install cpio
sudo apt-get -y install tar wget
sudo apt-get -y install qemu



chmod a+x ~/kernelAll/kernelCMD/*
sudo cp ~/kernelAll/kernelCMD/* /usr/bin/
cd ~/kernelAll
dec_rootfs rootfs.cpio