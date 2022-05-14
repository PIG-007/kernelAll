# kernelAll

Language: [English](https://github.com/PIG-007/kernelAll/blob/master/README_en-US.md) | [中文简体](https://github.com/PIG-007/kernelAll/blob/master/README.md)

只用几个命令即可编译各个版本的内核，且`qemu`适配运行，此外新加`CVE`环境编译建立功能。但是这个主要还是依赖我复现的`CVE`，暂时只有

```
CVE-2021-22555
```

并且可能不同的编译环境编译出来的相关`gadget`地址，或者`cred`结构体地址不太一样，所以在里面的`exp`的相关`gadget`地址什么的还是需要设置一下的。

## 安装

```bash
git clone https://github.com/PIG-007/kernelAll.git
#git clone https://gitee.com/Piggy007/kernelAll.git
cd kernelAll
chmod a+x setup.sh
sudo ./setup.sh
```

## 使用方法

### 1.编译各版本内核

#### 正常编译

```bash
kernelAll_compileKernel -v 4.4.72
```

这里就代表编译内核为`4.4.72`版本的。

完成之后在`~/kernelAll/kernelSource`下可以找到对应版本的内核源码，这个用来编译模块，或者在调试的时候可以用`gdb`的`dir`功能来直接调试源码

![image-20211102101950797](https://pig-007.oss-cn-beijing.aliyuncs.com/img/20211102101957.png)

之后会将`bzImage`和`vmlinux`以及项目根目录下的`rootfs.cpio`放到`kernelPwn`的对应版本文件夹中

![image-20220514230718957](https://pig-007.oss-cn-beijing.aliyuncs.com/img/image-20220514230718957.png)

之后启动内核就是依据当前文件夹下对应版本的内核文件了

![image-20220514230820436](https://pig-007.oss-cn-beijing.aliyuncs.com/img/image-20220514230820436.png)

#### 配置内核

由于更新成了`py`文件，使用`python3`，`pip`安装一下对应包即可

当然还可以对内核添加配置

##### ![image-20220514224807358](https://pig-007.oss-cn-beijing.aliyuncs.com/img/image-20220514224807358.png)

##### debug的内核

更加方便调试，编译内核时加入`-debug`

##### 设置内核

添加一些配置，比如`CONFIG_SLAB_FREELIST_RANDOM=y`，`CONFIG_SLAB_FREELIST_HARDENED=n`，另外加上`*`可以进行所有项匹配。

比如：

`-c CONFIG_SLAB_FREELIST_RANDOM=y CONFIG_SLAB_FREELIST_HARDENED=n CONFIG_NETFILTER_XT_*=y ` 

### 2.编译驱动模块

把驱动模块的源代码放到`path/kernelAll/modFile/`中，然后

```
kernelAll_compileModule kmod.c 4.4.72
```

这里就代表用刚才的编译好的`4.4.72`版本的内核源码来编译`kmod.c`，编译完成之后自动将`kmod.ko`放到`rootfs`文件系统中，并且重新打包。

### 3.添加文件进入文件系统

```bash
kernelAll_pocCopy poc 4.4.72
```

这个即代表将`poc`文件放入对应版本`4.4.72`的`rootfs`文件系统中，然后重新打包。

### 4.启动QEMU

```bash
kernelAll_boot 4.4.72
```

这个即代表依据对应`/path/kernelPwn/`对应版本的内核文件，使用`qemu-system-x86_64`启动刚刚编译好的`4.4.72`内核。

### 5.GDB附加调试

由于做pwn题时肯定需要附加调试，所以也提供了调试功能

```bash
bootPwnKernel 4.4.72 -g 1234
```

即以1234端口为调试端口，可供gdb附加调试。

### 6.自定义

现在相关的CMD设置成软链接模式放入到`/usr/bin/`中，所以直接修改`path/kernelAll/CMD/`对应的CMD命令文件即可

![image-20220514231142282](https://pig-007.oss-cn-beijing.aliyuncs.com/img/image-20220514231142282.png)



## 调试工具

详见

```
~/kernelAll/kernelTool/pigKernelHeap.py
~/kernelAll/kernelTool/pigKernelHeap_debug.py
```

任选其一即可，如果使用本工具编译的带`debug`信息时，那么使用`pigKernelHeap_debug.py`即可，如果不带debug信息时，需要用[vmlinux-to-elf](https://github.com/marin-m/vmlinux-to-elf)来获取带符号的`vmlinux`才行，然后还需要指定在`kmem_cache`结构体中的`random`值的偏移`slab_random_offset`，因为不同编译设置情况下都不一样的，这个最好查一下，或者实际调一下，如果不指定，则默认为`0xb8`。

在gdb中直接`source`引用即可。

### 获取帮助

```bash
pigSlub help
```

![image-20220514234717792](https://pig-007.oss-cn-beijing.aliyuncs.com/img/image-20220514234717792.png)

### 初始化

即传递一下相关配置，由于在`kernel`的Slub堆中，有`harden`、`random`等保护，但是不同版本中有swab计算，或者FD位置为`chunk_addr+size/2`，所以需要传递一下相关配置进行计算。比如这里就开启了harden保护，且版本在`v5.9`，那么就存在FD偏移和swab计算的情况。

```
pigSlub init freelist_harden freelist_harden_swab freelist_size2
```

### 获取freelist

```bash
pigSlub cpu0
pigSlub kmalloc-32
pigSlub all
```

具体自己实验一下吧

![image-20220316213858428](https://pig-007.oss-cn-beijing.aliyuncs.com/img/202203162138641.png)

相对于的`pigSlab`命令也是类似的。



# 注

`ubuntu20.04`的时候，依照`setup.sh`中安装`qemu`已经不太行了，需要自己手动安装对应版本的包，这里就需要安装如下的包，之后运行`bootPwnKernel`相关命令即可启动：

```bash
apt-get install qemu-system-x86
```

本人水平比较菜，如有错误或者bug，请师傅们多担待，欢迎提出相关意见。
