#!/bin/bash
TARGET="stm32f401re-nucleo-spirit1" 
BOARD="ids01a4"
SENSORBOARD="iks01a1"

mkdir cooja_firmwares

make TARGET=$TARGET BOARD=$BOARD DEFINES=COOJA=0,SINK=1,_MY_ADDRESS=0,SDN_WISE_DEBUG=0 
arm-none-eabi-objcopy -O binary my_first_app.$TARGET my_first_app.$TARGET
mv my_first_app.$TARGET cooja_firmwares/my_first_appbinst.$TARGET 
make TARGET=$TARGET BOARD=$BOARD clean


make TARGET=$TARGET BOARD=$BOARD clean
