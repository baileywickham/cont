# cont
A minimal container implementation in C, written to practice syscalls, linux, and C.


## About
newroot contains the new filesystem root. In my case it is a minimal alpine distro.

Eventually I want to get this into a state where there is a daemon to manage the individual contianers, and mulitple containers can be ran. This is similar to how docker works, with dockerd manageing the runc and containerd backend.

## Overlayfs
Overlayfs is used to create virtual file systems which will not be written to. This is done by using overlayfs with multiple lower dirs. Overlays may be composed as you would expect. This is used when multiple containers use the same base image.

To restore a container, I think you would move the workdir to the upperdir.

## TODO
- Figure out network namespaces
- Figure out mount namespaces, other mounts besides proc
- Automount proc fs `mount -t proc proc /proc` **DONE**
- Figure out clone/fork
- Determine role of Daemon

## Notes
- https://github.com/riswandans/litesploit/blob/master/modules/exploits/linux/localroot/overlayfs.c

- https://github.com/opencontainers/runc/tree/master/libcontainer

- `netdb.h` and getservbyname
- https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=ef94b1864d1ed5be54376404bb23d22ed0481feb
