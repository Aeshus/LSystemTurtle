# UEFI L-System & Turtle Graphics

![Screenshot](.github/image.png)

This project implements an L-System as described in _The Algorithmic Beauty Of Plants, by  Przemyslaw Prusinkiewicz, Aristid Lindenmayer_.

It consists of a string expander system, based on dynamic rules, which are then run on an initial input ("axiom").

The result of that L-System is then used to direct the turtle to draw a given shape.

All of this is implemented via TianoCore's EDKII development environment, in C.

## How to set up

Install & setup EDK2, and git clone this into the top level directory.

```bash
build -p LSystemTurtle/LSystemTurtlePkg.dsc -a X64
```

And then, copy that and open it in QEMU using OVMF.

Copy:
```bash
cp ../Build/LSystemTurtle/DEBUG_GCC/X64/LSystemTurtle.efi ./esp/EFI/BOOT/BOOTX64.EFI
```

Run:
```bash
qemu-system-x86_64 -drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/ovmf/OVMF_CODE.fd \
                                                        -drive format=raw,file=fat:rw:esp \
                                                        -net none
```
