#! /bin/bash


sudo apt-get install make gcc bison flex libssl-dev musl-tools
sudo apt-get install libncurses5-dev openssl 
sudo apt-get install build-essential 
sudo apt-get install pkg-config
sudo apt-get install libc6-dev
sudo apt-get install libelf-dev
sudo apt-get install zlibc minizip 
sudo apt-get install libidn11-dev libidn11
sudo apt-get install bc
sudo apt-get install cpio


cd ~/kernelAll
dec_rootfs rootfs.cpio
chmod a+x ~/kernelAll/kernelCMD/*
sudo cp ~/kernelAll/kernelCMD/* /usr/bin/