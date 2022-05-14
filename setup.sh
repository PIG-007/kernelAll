#!/usr/bin/env bash

project_path=$(pwd)

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


mkdir kernelSource
sudo ln -s -f $project_path/kernelCMD/* /usr/bin/
chmod +x $project_path/kernelCMD/*
dec_rootfs ./rootfs.cpio