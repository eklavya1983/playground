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
KAFKADIR="$( cd ${SOURCEDIR}/../kafka_2.11-0.10.0.0 && pwd )"
ZKDIR="$( cd ${SOURCEDIR}/../zookeeper-3.4.9 && pwd )"
printf "srcdir: $SOURCEDIR\n"
printf "toolsdir: $TOOLSDIR\n"
printf "kafkadir: $KAFKADIR\n"

# includes
source ${TOOLSDIR}/utils.sh


# Compile source code and install artifacts in src/../build folder
function compile {
    export CC=/usr/bin/gcc-4.9
    export CXX=/usr/bin/g++-4.9
    echo "cc is $CC"
    echo "cxx is $CXX"
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

# In the idl folder run make to generate code
function idlgen {
    cd ${SOURCEDIR}/idls
    make clean
    make infra
}

# buid source and install artifacts to /op/datom
function install {
    cd ${SOURCEDIR}/../build
    cmake -DCMAKE_INSTALL_PREFIX="/opt/datom" ..
    make && make install
}

# clean zookeeper and kafka logs locally
function cleandatom {
    rm -rf /tmp/zookeeper/*
    rm -rf /tmp/kafka-logs/*
}

# Stops zk & kafka locally
function stopdatom {
    while true; do
        local pids=$(ps ax | grep java | grep -i kafka | grep -v grep | awk '{print $1}')
        if [[ -z $pids ]]; then
            break
        fi
        kill -9 $pids
        sleep 1
    done
}

# Starts zk & kafka locally
function startdatom {
    while true; do
        ${KAFKADIR}/bin/zookeeper-server-start.sh -daemon ${KAFKADIR}/config/zookeeper.properties
        ${KAFKADIR}/bin/kafka-server-start.sh -daemon ${KAFKADIR}/config/server.properties
        break
        # NOTE: The below logic isn't executed.  What I found is if I don't clean logs sometimes
        # kafka crashes saying another broker is already registered.  When there is a use case
        # where we don't clean logs the below logic of waiting and trying again may work
        sleep 5 
        local cnt=$(ps ax | grep java | grep -i kafka | grep -v grep | wc | awk '{print $1}')
        if [[ $cnt -gt 1 ]]; then
            break
        fi
        logwarn "Failed to start zookeeper and kafka.  Trying again after stopping"
        stopdatom
    done
}

# Stops, clean datom
function cleanstopdatom {
    stopdatom
    cleandatom
}

# Stops, clean, and starts datom
function cleanstartdatom {
    cleanstopdatom
    startdatom
    sleep 1
    ${ZKDIR}/bin/zkCli.sh create /datom datom
}


# Another version of starting datom not in use
function startdatom2 {
    while true; do
        ${KAFKADIR}/bin/zookeeper-server-start.sh -daemon ${KAFKADIR}/config/zookeeper.properties
        local pids=$(ps ax | grep java | grep -i kafka | grep -v grep | awk '{print $1}')
        if [[ ! -z $pids ]]; then
            break
        fi
        logwarn "Failed to start zookeeper.  Trying again...."
    done

    while true; do
        ${KAFKADIR}/bin/kafka-server-start.sh -daemon ${KAFKADIR}/config/server.properties
        local cnt=$(ps ax | grep java | grep -i kafka | grep -v grep | wc | awk '{print $1}')
        if [[ $cnt -gt 1 ]]; then
            break
        fi
        logwarn "Failed to start kafka.  Trying again...."
    done
}

# call arguments verbatim:
$@
