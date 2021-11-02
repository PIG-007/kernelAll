# kernelAll

Language: [English](https://github.com/PIG-007/kernelAll/blob/master/README_en-US.md) | [中文简体](https://github.com/PIG-007/kernelAll/blob/master/README.md)

只用几个命令即可编译各个版本的内核，且qemu适配运行。

## 安装

```bash
cd ~/
git clone https://github.com/PIG-007/kernelAll.git
#git clone https://gitee.com/Piggy007/kernelAll.git
cd kernelAll
chmod a+x setup.sh
sudo ./setup.sh
```

## 使用方法

### 1.编译各版本内核

```bash
compileKernel 4.4.72
```

这里就代表编译内核为4.4.72版本的。

完成之后在~/kernelAll/kernelSource下可以找到对应版本的内核源码，这个用来编译模块，或者在调试的时候可以用gdb的dir功能来直接调试源码

![image-20211102101950797](https://pig-007.oss-cn-beijing.aliyuncs.com/img/20211102101957.png)

### 2.编译驱动模块

把驱动模块的源代码放到~/kernelAll/modFile/中，然后

```
compileModule kmod.c 4.4.72
```

这里就代表用刚才的编译好的4.4.72版本的内核源码来编译kmod.c，编译完成之后自动将kmod.ko放到rootfs文件系统中，并且重新打包。

### 3.添加文件进入文件系统

```bash
kernelPocCopy poc
```

这个即代表将poc文件放入rootfs文件系统中，然后重新打包。

### 4.启动QEMU

```bash
bootPwnKernel 4.4.72
```

这个即代表以当前的文件系统，使用qemu-system-x86_64启动刚刚编译好的4.4.72内核。

### 5.GDB附加调试

由于做pwn题时肯定需要附加调试，所以也提供了调试功能

```bash
bootPwnKernel 4.4.72 -g 1234
```

即以1234端口为调试端口，可供gdb附加调试。



## 扩展

做pwn题时经常需要修改init和qemu启动选项，所以这里也提供相关的更新功能

### 1.更新init

这个可以直接更新在~/kernelAll/中的init文件，然后输入`updateKernelAll`命令即可。

![image-20211102104712173](https://pig-007.oss-cn-beijing.aliyuncs.com/img/20211102104712.png)

### 2.更新qemu启动项

这个需要在`~/kernelAll/kernelCMD/bootPwnKernel`文件中更新

找到如下位置，对应修改所需选项即可，然后再输入`updateKernelAll`命令更新即可。

![image-20211102110003180](https://pig-007.oss-cn-beijing.aliyuncs.com/img/20211102110003.png)



# 注

本人水平比较菜，如有错误或者bug，请师傅们多担待，欢迎提出相关意见。

