#!/bin/bash

rm -rf build
mkdir build
cd build || exit 1

if [ -z "${LLVM_CONFIG}" ]; then
	export LLVM_CONFIG=llvm-config
	echo "[*] LLVM_CONFIG is not set, default to llvm-config"
fi

if ! which "$LLVM_CONFIG" &> /dev/null; then
	echo "[x] Failed to locate $LLVM_CONFIG"
	exit 1
fi

if [ -z "$DCMAKE_BUILD_TYPE" ]; then
	DCMAKE_BUILD_TYPE=RelWithDebInfo
	echo "[*] DCMAKE_BUILD_TYPE is not set, default to RelWithDebInfo"
fi

cmake	-DCMAKE_BUILD_TYPE="$DCMAKE_BUILD_TYPE" ..