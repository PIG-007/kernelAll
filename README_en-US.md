# kernelAll

Language: [English](https://github.com/PIG-007/kernelAll/blob/master/README_en-US.md) | [中文简体](https://github.com/PIG-007/kernelAll/blob/master/README.md)

Compile many 

You can compile the kernels of various versions with only a few commands, and qemu can run it.

## Install

```bash
cd ~/
git clone https://github.com/PIG-007/kernelAll.git
#git clone https://gitee.com/Piggy007/kernelAll.git
cd kernelAll
chmod a+x setup.sh
sudo ./setup.sh
```

## Usage

### 1.Compile each version of the kernel 

```bash
compileKernel 4.4.72
```

This means that the version of the compiled kernel is 4.4.72. 

After completion, you can find the kernel source code of the corresponding version under `~/kernelAll/kernelSource`, this is used to compile the module, or you can use the `dir` function of `gdb` to directly debug the source code when debugging.

![image-20211102101950797](https://pig-007.oss-cn-beijing.aliyuncs.com/img/20211102101957.png)

### 2.Compile the driver module 

Put the source code of the driver module into the `~/kernelAll/modFile/`, then 

```
compileModule kmod.c 4.4.72
```

This means using the compiled kernel source code of version 4.4.72 to compile `kmod.c`. After the compilation, kmod.ko is automatically placed in the rootfs file system and repackaged. 

### 3.Add files to the file system 

```bash
kernelPocCopy poc
```

This means that the `poc` file is put into the rootfs file system, and then repackaged. 

### 4.Start QEMU 

```bash
bootPwnKernel 4.4.72
```

This means that with the current file system, using `qemu-system-x86_64` to start the just-compiled 4.4.72 kernel. 

### 5.GDB additional debugging 

Since additional debugging is definitely needed when doing pwn questions, debugging functions are also provided.

```bash
bootPwnKernel 4.4.72 -g 1234
```

That is, port 1234 is used as a debugging port for additional debugging by gdb.



## Expand

It is often necessary to modify the `init` and `qemu startup options` when doing pwn questions, so related update functions are also provided here

### 1.Update init 

This can be directly updated in the `init` file in `~/kernelAll/`, and then enter the `updateKernelAll` command.

![image-20211102104712173](https://pig-007.oss-cn-beijing.aliyuncs.com/img/20211102104712.png)

### 2.Update qemu startup items

This needs to be updated in the `~/kernelAll/kernelCMD/bootPwnKernel` file 

Find the following location, modify the required options accordingly, and then enter the `updateKernelAll` command to update. 

![image-20211102110003180](https://pig-007.oss-cn-beijing.aliyuncs.com/img/20211102110003.png)



# Post

My skill level is very low.So if there are errors or bugs, please bear with me and welcome your comments. 

