#!/bin/bash

rm -rf build
mkdir build
cd build

if [ -z "${LLVM_CONFIG}" ];
then
	export LLVM_CONFIG=llvm-config
fi

cmake	-DCMAKE_BUILD_TYPE=RelWithDebInfo \
			..