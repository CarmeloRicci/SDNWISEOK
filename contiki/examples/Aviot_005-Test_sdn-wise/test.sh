#!/bin/bash
TARGET="stm32f401re-nucleo-spirit1" 
BOARD="ids01a4"
SENSORBOARD="iks01a1"
DEBUG_MELO=1

Folder='Melo_ST'

sudo rm -rf $Folder

mkdir $Folder


printf "\n\n\n *************************************************** NODE 2 WITHOUT SENSORBOARD *************************************************** \n\n\n\n"
make TARGET=$TARGET BOARD=$BOARD DEFINES=COOJA=0,SINK=0,_MY_ADDRESS=2,SDN_WISE_DEBUG=1,DEBUG_MELO=$DEBUG_MELO
arm-none-eabi-objcopy -O binary sdn-wise.$TARGET sdn-wise_2.$TARGET
mv sdn-wise_2.$TARGET $Folder/node_2.$TARGET 
make TARGET=$TARGET BOARD=$BOARD clean
