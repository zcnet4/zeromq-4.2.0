#!/bin/bash

WORKDIR=`pwd`/..
BUILDTYPE=Release
DST=yx-0.0

cd ${WORKDIR}/build_linux
rm -rf ${DST}
mkdir ${DST}

mkdir ${DST}/bin
# 复制第三方so
cp ${WORKDIR}/3rd/glog/lib/linux/* ${DST}/bin
cp ${WORKDIR}/3rd/gperftools-2.5/lib/linux/* ${DST}/bin
cp ${WORKDIR}/3rd/libstdc++.so.6.0.21 ${DST}/bin

# 复制
cp ${BUILDTYPE}/skynet ${DST}/bin
cp ${BUILDTYPE}/gateway ${DST}/bin
cp ${BUILDTYPE}/gateway-forward ${DST}/bin
cp ${BUILDTYPE}/gateway-zmq ${DST}/bin
cp ${BUILDTYPE}/*.so ${DST}/bin
cp -r ${BUILDTYPE}/lib.target ${DST}/bin
cp ${BUILDTYPE}/lib.target/memory.so ${DST}/bin
cp ${BUILDTYPE}/lib.target/skynet.so ${DST}/bin
cp ${BUILDTYPE}/lib.target/protobuf.so ${DST}/bin

#复制配置
cp -r ${WORKDIR}/gateway/conf ${DST}


# 复制skynet系统脚本
mkdir ${DST}/skynet
cp -r ${WORKDIR}/skynet/lualib ${DST}/skynet/lualib
cp -r ${WORKDIR}/skynet/service ${DST}/skynet/service

# 复制skynet脚本
cp -r ${WORKDIR}/skynet/yx ${DST}/skynet/yx

tar -czf ${DST}.tar.gz ${DST}
cp ${DST}.tar.gz ${WORKDIR}



