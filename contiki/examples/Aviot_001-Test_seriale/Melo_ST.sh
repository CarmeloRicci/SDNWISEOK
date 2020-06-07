#!/bin/bash
TARGET="stm32f401re-nucleo-spirit1" 
BOARD="ids01a4"
SENSORBOARD="iks01a1"
DEBUG_MELO=1

Folder='Melo_ST'

sudo rm -rf $Folder

mkdir $Folder

make TARGET=$TARGET BOARD=$BOARD
arm-none-eabi-objcopy -O binary sdn-wise.$TARGET my_first_app_2.$TARGET
mv my_first_app_2.$TARGET $Folder/app_2.$TARGET 
make TARGET=$TARGET BOARD=$BOARD clean



#make TARGET=$TARGET BOARD=$BOARD DEFINES=COOJA=0,SINK=0,_MY_ADDRESS=3,SDN_WISE_DEBUG=0
#arm-none-eabi-objcopy -O binary sdn-wise.$TARGET sdn-wise_3.$TARGET
#mv sdn-wise_3.$TARGET $Folder/node_3.$TARGET 
#make TARGET=$TARGET BOARD=$BOARD clean


#make TARGET=$TARGET BOARD=$BOARD  DEFINES=COOJA=0,SINK=0,_MY_ADDRESS=4,SDN_WISE_DEBUG=1
#arm-none-eabi-objcopy -O binary sdn-wise.$TARGET sdn-wise_4.$TARGET
#mv sdn-wise_4.$TARGET $Folder/node_4.$TARGET 
#make TARGET=$TARGET BOARD=$BOARD clean


#make TARGET=$TARGET BOARD=$BOARD clean

