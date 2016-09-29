#!/bin/bash

# Figure out src dir and tools dir
SOURCE="${BASH_SOURCE[0]}"
# resolve $SOURCE until the file is no longer a symlink
while [ -h "$SOURCE" ]; do
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
TOOLSDIR="$(cd -P "$( dirname "${SOURCE}" )" && pwd )"
SOURCEDIR="$( cd ${TOOLSDIR}/.. && pwd)"
printf "srcdir: $SOURCEDIR\n"
printf "toolsdir: $TOOLSDIR\n"

# includes
source ${TOOLSDIR}/utils.sh


# Compile source code and install artifacts in src/../build folder
function compile {
    cd ${SOURCEDIR}/../build
    cmake -DCMAKE_INSTALL_PREFIX="." ..
    make && make install
}

# clean out the build artifacts
function clean {
    set -x
    rm -rf ${SOURCEDIR}/../build/*
    set +x
}

# buid source and install artifacts to /op/datom
function install {
    cd ${SOURCEDIR}/../build
    cmake -DCMAKE_INSTALL_PREFIX="/opt/datom" ..
    make && make install
}

# In the idl folder run make to generate code
function idlgen {
    cd ${SOURCEDIR}/idl
    make clean
    make infra
}

# call arguments verbatim:
$@
