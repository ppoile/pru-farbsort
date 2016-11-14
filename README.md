Setup:

Download, give execution permission, execute::
http://software-dl.ti.com/codegen/esd/cgt_public_sw/PRU/2.1.3/ti_cgt_pru_2.1.3_linux_installer_x86.bin
...and install to ~/prg/ti-cgt-pru_2.1.3

Download, give execution permission, execute:
http://software-dl.ti.com/processor-sw/esd/PROCESSOR-SDK-LINUX-AM335X/03_01_00_06/exports/ti-processor-sdk-linux-am335x-evm-03.01.00.06-Linux-x86-Install.bin
...and install to ~/prg/ti-processor-sdk-linux-am335x-evm-03.01.00.06

Download and unpack:
https://git.ti.com/pru-software-support-package/pru-software-support-package/archive-tarball/v4.0.2
...and rename top-directory to ~/prg/ti-processor-sdk-linux-am335x-evm-03.01.00.06/example-applications/pru-icss-4.02

Patch pru-icss-4.0.2:
```
cd ~/prg/ti-processor-sdk-linux-am335x-evm-03.01.00.06/example-applications/pru-icss-4.0.2
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

Build:
```
$ source setup-ti-env.ch
$ make
```
