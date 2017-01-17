#!/bin/bash

WORKDIR=`pwd`

cp ${WORKDIR}/3rd/glog/lib/linux/libglog.so.0 ${WORKDIR}/build_linux/Debug/libglog.so.0
cd ${WORKDIR}/build_linux/Debug
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:.:${MyLIBRARY_PATH}
./gateway ~/gateway_conf/gateway.conf
cd ${WORKDIR}



