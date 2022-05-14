#kernelAll

Language: [English](https://github.com/PIG-007/kernelAll/blob/master/README_en-US.md) | [中文](https://github.com/PIG-007/kernelAll/ blob/master/README.md)

Only a few commands can be used to compile various versions of the kernel, and `qemu` can be adapted to run. In addition, the `CVE` environment compilation and establishment function is added. But this mainly depends on the `CVE` that I reproduced. For the time being, only

````
CVE-2021-22555
````

And the address of the relevant `gadget` compiled by different compilation environments, or the address of the `cred` structure may not be the same, so the relevant `gadget` address of the `exp` in it still needs to be set.

## Install

```bash
git clone https://github.com/PIG-007/kernelAll.git
#git clone https://gitee.com/Piggy007/kernelAll.git
cd kernelAll
chmod a+x setup.sh
sudo ./setup.sh
````

## Instructions

### 1. Compile each version of the kernel

#### compile normally

```bash
kernelAll_compileKernel -v 4.4.72
````

This means that the compiled kernel is version `4.4.72`.

After completion, you can find the kernel source code of the corresponding version under `~/kernelAll/kernelSource`, which is used to compile the module, or you can use the `dir` function of `gdb` to directly debug the source code when debugging

![image-20211102101950797](https://pig-007.oss-cn-beijing.aliyuncs.com/img/20211102101957.png)

After that, `bzImage` and `vmlinux` and `rootfs.cpio` in the project root directory will be placed in the corresponding version folder of `kernelPwn`

![image-20220514230718957](https://pig-007.oss-cn-beijing.aliyuncs.com/img/image-20220514230718957.png)

After that, the kernel is started according to the corresponding version of the kernel file in the current folder.

![image-20220514230820436](https://pig-007.oss-cn-beijing.aliyuncs.com/img/image-20220514230820436.png)

#### configure the kernel

Since it has been updated to a `py` file, use `python3` and `pip` to install the corresponding package

Of course, you can also add configuration to the kernel

##### ![image-20220514224807358](https://pig-007.oss-cn-beijing.aliyuncs.com/img/image-20220514224807358.png)

##### debug kernel

More convenient to debug, add `-debug` when compiling the kernel

##### set kernel

Add some configuration, such as `CONFIG_SLAB_FREELIST_RANDOM=y`, `CONFIG_SLAB_FREELIST_HARDENED=n`, and add `*` to match all items.

for example:

`-c CONFIG_SLAB_FREELIST_RANDOM=y CONFIG_SLAB_FREELIST_HARDENED=n CONFIG_NETFILTER_XT_*=y `

### 2. Compile the driver module

Put the source code of the driver module in `path/kernelAll/modFile/`, then

````
kernelAll_compileModule kmod.c 4.4.72
````

Here, it means to compile `kmod.c` with the compiled kernel source code of `4.4.72` version. After the compilation is completed, `kmod.ko` is automatically placed in the `rootfs` file system and repackaged.

### 3. Add files into the file system

```bash
kernelAll_pocCopy poc 4.4.72
````

This means to put the `poc` file into the `rootfs` file system of the corresponding version `4.4.72`, and then repackage it.

### 4. Start QEMU

```bash
kernelAll_boot 4.4.72
````

This means that according to the kernel file corresponding to the corresponding version of `/path/kernelPwn/`, use `qemu-system-x86_64` to start the just compiled `4.4.72` kernel.

### 5. GDB additional debugging

Since additional debugging is definitely required when doing pwn questions, a debugging function is also provided.

```bash
bootPwnKernel 4.4.72 -g 1234
````

That is, the port 1234 is used as the debugging port, which can be used for additional debugging by gdb.

### 6. Customize

Now the relevant CMD is set to soft link mode and put into `/usr/bin/`, so you can directly modify the CMD command file corresponding to `path/kernelAll/CMD/`

![image-20220514231142282](https://pig-007.oss-cn-beijing.aliyuncs.com/img/image-20220514231142282.png)



## debugging tools

see details

````
~/kernelAll/kernelTool/pigKernelHeap.py
~/kernelAll/kernelTool/pigKernelHeap_debug.py
````

You can choose one of them. If you use this tool to compile with `debug` information, you can use `pigKernelHeap_debug.py`. If you don't have debug information, you need to use [vmlinux-to-elf](https:/ /github.com/marin-m/vmlinux-to-elf) to get the signed `vmlinux`, then you also need to specify the offset `slab_random_offset` of the `random` value in the `kmem_cache` structure, because Different compilation settings are different. It is best to check this, or actually adjust it. If it is not specified, it will default to `0xb8`.

You can directly `source` reference in gdb.

### Get help

```bash
pigSlub help
````

![image-20220514234717792](https://pig-007.oss-cn-beijing.aliyuncs.com/img/image-20220514234717792.png)

### Initialize

That is to pass the relevant configuration, because in the Slub heap of `kernel`, there are `harden`, `random` and other protections, but there are swab calculations in different versions, or the FD position is `chunk_addr+size/2`, so it needs to be passed Calculate the relevant configuration. For example, if harden protection is enabled here, and the version is `v5.9`, then there is a situation of FD offset and swap calculation.

````
pigSlub init freelist_harden freelist_harden_swab freelist_size2
````

### Get freelist

```bash
pigSlub cpu0
pigSlub kmalloc-32
pigSlub all
````

Experiment for yourself.

![image-20220316213858428](https://pig-007.oss-cn-beijing.aliyuncs.com/img/202203162138641.png)

Relative to the `pigSlab` command is similar.



# Note

In `ubuntu20.04`, it is not enough to install `qemu` according to `setup.sh`, you need to manually install the corresponding version of the package, here you need to install the following package, and then run the `bootPwnKernel` related commands start up:

```bash
apt-get install qemu-system-x86
````

My level is relatively dishes. If there are errors or bugs, please bear with me, and welcome comments.