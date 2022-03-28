#!/bin/sh
set -e

export STM32_CUBE_F3_PATH=/project/STM32CubeF3
export STM32_TOOLCHAIN_PATH=/usr
export STM32_TARGET_TRIPLET=arm-none-eabi

cd /build

cmake -GNinja /project
ninja install
