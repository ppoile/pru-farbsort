# Setup

A good introduction what to optain can be found on http://processors.wiki.ti.com/index.php/PRU-ICSS_Getting_Started_Guide

__NOTE:__ Make sure that the PRU Code Generation Tools and the Linux Processor SDK (and its included software support package) are compatible with the kernel and its drivers.

A valid combination that works:
* [Debian 8.6](https://debian.beagleboard.org/images/bone-debian-8.6-lxqt-4gb-armhf-2016-11-06-4gb.img.xz)
* [CGT 2.1.4](http://software-dl.ti.com/codegen/esd/cgt_public_sw/PRU/2.1.4/ti_cgt_pru_2.1.4_linux_installer_x86.bin)
* [SDK 3.0.2](http://software-dl.ti.com/processor-sdk-linux/esd/AM335X/latest/index_FDS.html)
 

## PRU Code Generation Tools

Download correct version of the tools: http://software-dl.ti.com/codegen/non-esd/downloads/download.htm#PRU
give execution permission, execute
...and install to ~/prg/ti-cgt-pru_x.y.z

## Linux Processor SDK
Download correct version of the SDK: http://www.ti.com/tool/PROCESSOR-SDK-AM335X
give execution permission, execute
...and install to ~/prg/ti-processor-sdk-linux-am335x-evm-xx.xx.xx.xx

Patch pru-icss-a.b.c:
```
cd ~/prg/ti-processor-sdk-linux-am335x-evm-xx.xx.xx.xx/example-applications/pru-icss-a.b.c
cat > 0001-pru_virtio_ring-make-it-compile.patch << 'EOF'
From 610ba2f327bbbc0c778349199cc0cda6079d235c Mon Sep 17 00:00:00 2001
From: Andreas Meier <andreas.meier@bbv.ch>
Date: Mon, 14 Nov 2016 10:20:26 +0100
Subject: [PATCH] pru_virtio_ring: make it compile

---
 include/pru_virtio_ring.h | 6 +++---
 1 file changed, 3 insertions(+), 3 deletions(-)

diff --git a/include/pru_virtio_ring.h b/include/pru_virtio_ring.h
index 6cbb32f..219f82c 100755
--- a/include/pru_virtio_ring.h
+++ b/include/pru_virtio_ring.h
@@ -131,9 +131,9 @@ static inline void vring_init(struct vring *vr, uint32_t num, void *p,
                              uint64_t align)
 {
        vr->num = num;
-       vr->desc = p;
-       vr->avail = (void *)((char *)p + num*sizeof(struct vring_desc));
-       vr->used = (void *)(uintptr_t)(((uint64_t)&vr->avail->ring[num]
+       vr->desc = (vring_desc *)p;
+       vr->avail = (vring_avail *)((char *)p + num*sizeof(struct vring_desc));
+       vr->used = (vring_used *)(uintptr_t)(((uint64_t)&vr->avail->ring[num]
                + sizeof(uint16_t) + align-1) & ~(align - 1));
 }

--
2.7.4
EOF
patch -p1 0001-pru_virtio_ring-make-it-compile.patch
```

Adapt ```setup-ti-env.sh``` to reference the above installed software packages.

# Build

```
$ source setup-ti-env.sh
$ make
```
