#!/bin/sh
#
# This file was automatically generated.
#
# It can be overwritten by nios2-flash-programmer-generate or nios2-flash-programmer-gui.
#

#
# Converting ELF File: /afs/ualberta.ca/home/x/u/xuefei1/Desktop/ECE492_Projects/niosII_microc_lab1/scripts/software/lab2/lab2.elf to: "../flash/lab2_generic_tristate_controller_0.flash"
#
elf2flash --input="/afs/ualberta.ca/home/x/u/xuefei1/Desktop/ECE492_Projects/niosII_microc_lab1/scripts/software/lab2/lab2.elf" --output="../flash/lab2_generic_tristate_controller_0.flash" --boot="$SOPC_KIT_NIOS2/components/altera_nios2/boot_loader_cfi.srec" --base=0x0 --end=0x400000 --reset=0x0 

#
# Programming File: "../flash/lab2_generic_tristate_controller_0.flash" To Device: generic_tristate_controller_0
#
nios2-flash-programmer "../flash/lab2_generic_tristate_controller_0.flash" --base=0x0 --sidp=0x1109078 --id=0x0 --timestamp=1485821838 --device=1 --instance=0 '--cable=USB-Blaster on localhost [2-1.6]' --program 

