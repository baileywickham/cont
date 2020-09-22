# Overlay
There is an issue with mounting an overlayfs.

- mount overlay with fail with, not sure why.
```
filesystem on '/home/y/workspace/cont/containers/test/upper' not supported as upperdir
```
- The line in the kernel says that there is a flag in the dentry called d_real or something, not sure what it does.
- DCACHE_OP_REAL
- A later version of the kernel fixes this bug

- [overlayfs](https://www.kernel.org/doc/Documentation/filesystems/overlayfs.txt)
- [line in kernel](https://github.com/torvalds/linux/blob/0dd68a34eccd598109eb845d107a7e8e196745db/fs/overlayfs/super.c#L815)
- [commit](https://github.com/torvalds/linux/commit/7925dad839e70ab43140e26d6b93e807b5969f05)

