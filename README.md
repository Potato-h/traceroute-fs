Before compile you need to install following packages:
- fuse3
- libfuse3-3
- libfuse3-dev
- meson

Compile with these commands:
meson <directory-with-this-file>/build 
ninja -C <directory-with-this-file>/build

You can find program by this path:
<directory-with-this-file>/build/fsw

usage: ./<directory-with-this-file>/build/fsw [options] <mountpoint>

FUSE options:
    -h   --help            print help
    -V   --version         print version
    -d   -o debug          enable debug output (implies -f)
    -f                     foreground operation
    -s                     disable multi-threaded operation
    -o clone_fd            use separate fuse device fd for each thread
                           (may improve performance)
    -o max_idle_threads    the maximum number of idle worker threads
                           allowed (default: 10)
    -o kernel_cache        cache files in kernel
    -o [no]auto_cache      enable caching based on modification times (off)
    -o umask=M             set file permissions (octal)
    -o uid=N               set file owner
    -o gid=N               set file group
    -o entry_timeout=T     cache timeout for names (1.0s)
    -o negative_timeout=T  cache timeout for deleted names (0.0s)
    -o attr_timeout=T      cache timeout for attributes (1.0s)
    -o ac_attr_timeout=T   auto cache timeout for attributes (attr_timeout)
    -o noforget            never forget cached inodes
    -o remember=T          remember cached inodes for T seconds (0s)
    -o modules=M1[:M2...]  names of modules to push onto filesystem stack
    -o allow_other         allow access by all users
    -o allow_root          allow access by root
    -o auto_unmount        auto unmount on process termination

Options for subdir module:
    -o subdir=DIR	    prepend this directory to all paths (mandatory)
    -o [no]rellinks	    transform absolute symlinks to relative

Options for iconv module:
    -o from_code=CHARSET   original encoding of file names (default: UTF-8)
    -o to_code=CHARSET	    new encoding of the file names (default: UTF-8)

