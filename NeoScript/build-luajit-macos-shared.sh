 #!/bin/sh -e

export MACOSX_DEPLOYMENT_TARGET=10.12
make -j4 CC="$1 -fPIC"
