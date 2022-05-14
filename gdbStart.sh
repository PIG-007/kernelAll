#!/bin/bash

gdb \
    -ex 'add-symbol-file ./vmlinux 0xffffffff81000000' \
    -ex 'add-symbol-file /home/hacker/kernelAll/modFile/myHeapMod.ko 0xffffffffc0000000' \
    -ex 'target remote localhost:1234' \
    -ex 'continue'\
    # -ex 'b *(0xffffffffc0000000+0xd7)' \
    # -ex 'b *(0xffffffffc0000000+0x140)' \
    # -ex 'break *0x400E88' \
    # -ex 'continue'