cmd_/home/hacker/kernelAll/modFile/modules.order := {   echo /home/hacker/kernelAll/modFile/myHeapMod.ko; :; } | awk '!x[$$0]++' - > /home/hacker/kernelAll/modFile/modules.order
