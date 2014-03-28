#!/bin/sh
#
# This file was automatically generated.
#
# It can be overwritten by nios2-flash-programmer-generate or nios2-flash-programmer-gui.
#

#
# Converting SOF File: C:\Users\kigunda\Documents\Ece492\Repository\ARCap\output_files\de0_nano_system.sof to: "..\flash/de0_nano_system_epcs_flash_controller_0.flash"
#
sof2flash --input="C:/Users/kigunda/Documents/Ece492/Repository/ARCap/output_files/de0_nano_system.sof" --output="../flash/de0_nano_system_epcs_flash_controller_0.flash" --epcs 

#
# Programming File: "..\flash/de0_nano_system_epcs_flash_controller_0.flash" To Device: epcs_flash_controller_0
#
nios2-flash-programmer "../flash/de0_nano_system_epcs_flash_controller_0.flash" --base=0x2009000 --epcs --sidp=0x200A0C8 --id=0x0 --timestamp=1395959215 --device=1 --instance=0 '--cable=USB-Blaster on localhost [USB-0]' --program 

#
# Converting ELF File: C:\Users\kigunda\Documents\Ece492\Repository\ARCap\Software\ARCap\ARCap.elf to: "..\flash/ARCap_epcs_flash_controller_0.flash"
#
elf2flash --input="C:/Users/kigunda/Documents/Ece492/Repository/ARCap/Software/ARCap/ARCap.elf" --output="../flash/ARCap_epcs_flash_controller_0.flash" --epcs --after="../flash/de0_nano_system_epcs_flash_controller_0.flash" 

#
# Programming File: "..\flash/ARCap_epcs_flash_controller_0.flash" To Device: epcs_flash_controller_0
#
nios2-flash-programmer "../flash/ARCap_epcs_flash_controller_0.flash" --base=0x2009000 --epcs --sidp=0x200A0C8 --id=0x0 --timestamp=1395959215 --device=1 --instance=0 '--cable=USB-Blaster on localhost [USB-0]' --program 

