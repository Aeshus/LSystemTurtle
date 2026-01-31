#!/bin/bash
build -p LSystemTurtle/LSystemTurtlePkg.dsc -a X64

cp ../Build/LSystemTurtle/RELEASE_GCC/X64/LSystemTurtle.efi ./esp/EFI/BOOT/BOOTX64.EFI

qemu-system-x86_64 -drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/ovmf/OVMF_CODE.fd \
                                                        -drive format=raw,file=fat:rw:esp \
                                                        -net none
