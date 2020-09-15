# Overlay
There is an issue with mounting an overlayfs.

This is the line that is failing

- [line in kernel](https://github.com/torvalds/linux/blob/0dd68a34eccd598109eb845d107a7e8e196745db/fs/overlayfs/super.c#L815)
- [commit](https://github.com/torvalds/linux/commit/7925dad839e70ab43140e26d6b93e807b5969f05)

