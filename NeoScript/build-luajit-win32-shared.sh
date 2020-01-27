 #!/bin/sh -e

# make clean
make -j$(nproc) HOST_CC="/usr/bin/gcc" CROSS="x86_64-w64-mingw32.shared-" TARGET_SYS=Windows
