cmd_/home/hacker/kernelAll/modFile/kmod.ko := ld -r -m elf_x86_64  -z max-page-size=0x200000  --build-id  -T ./scripts/module-common.lds -o /home/hacker/kernelAll/modFile/kmod.ko /home/hacker/kernelAll/modFile/kmod.o /home/hacker/kernelAll/modFile/kmod.mod.o;  true
