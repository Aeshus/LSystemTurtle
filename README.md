# L-System Turtle

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
