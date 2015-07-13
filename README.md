# CoreFreq
## Purpose
CoreFreq is made for the Intel 64-bits Processor, architectures Atom, Core2, Nehalem, SandyBridge and above.

## Build & Run
 1- Download or clone the source code into a working directory.
 
 2- Build the programs.
```
make
```

```
cc -g -c corefreqd.c -o corefreqd.o
cc -g -lpthread -lrt -o corefreqd corefreqd.c
cc -g -c corefreq-cli.c -o corefreq-cli.o
cc -g -lrt -o corefreq-cli corefreq-cli.c
make -C /lib/modules/4.0.7-2-ARCH/build M=/workdir/CoreFreq modules
make[1]: Entering directory '/usr/lib/modules/4.0.7-2-ARCH/build'
  CC [M]  /workdir/CoreFreq/intelfreq.o
  Building modules, stage 2.
  MODPOST 1 modules
  CC      /workdir/CoreFreq/intelfreq.mod.o
  LD [M]  /workdir/CoreFreq/intelfreq.ko
make[1]: Leaving directory '/usr/lib/modules/4.0.7-2-ARCH/build'
```

### Start

 3- Load the Kernel module, as root.
```
insmod intelfreq.ko
```
 4- Start the daemon, as root.
```
./corefreqd
```
 5- Start the client, as user.
```
./corefreq-cli
```

### Stop

 6- Press [CTRL]+[C] to stop the client.
 7- Press [CTRL]+[C] to stop the daemon.
 8- Unload the kernel module with the rmmod command
```
rmmod intelfreq.ko
```

## Screenshots
 * Use ```dmesg``` or ```journalctl -kf``` to check if the driver is started
```
kernel: IntelFreq [Intel(R) Core(TM) i7 CPU 920 @ 2.67GHz]
        Signature [06_1A] Architecture [Nehalem/Bloomfield]
        8/8 CPU Online , Clock @ {146/2939877074} MHz
        Ratio={12,20,0,0,0,0,21,21,21,22}
kernel: Topology(0) Apic[  0] Core[  0] Thread[  0]
kernel: Topology(1) Apic[  2] Core[  1] Thread[  0]
kernel: Topology(2) Apic[  4] Core[  2] Thread[  0]
kernel: Topology(3) Apic[  6] Core[  3] Thread[  0]
kernel: Topology(4) Apic[  1] Core[  0] Thread[  1]
kernel: Topology(5) Apic[  3] Core[  1] Thread[  1]
kernel: Topology(6) Apic[  5] Core[  2] Thread[  1]
kernel: Topology(7) Apic[  7] Core[  3] Thread[  1]
```

![alt text](http://blog.cyring.free.fr/images/CoreFreq.png "CoreFreq")

# Regards
_`CyrIng`_
