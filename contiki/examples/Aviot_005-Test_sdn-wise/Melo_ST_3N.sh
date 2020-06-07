#!/bin/bash
TARGET="stm32f401re-nucleo-spirit1" 
BOARD="ids01a4"
SENSORBOARD="iks01a1"
DEBUG_MELO=1

Folder='Melo_ST'

sudo rm -rf $Folder

mkdir $Folder

printf "\n\n\n *************************************************** SINK WITHOUT SENSORBOARD *************************************************** \n\n\n\n"
make TARGET=$TARGET BOARD=$BOARD DEFINES=COOJA=0,SINK=1,_MY_ADDRESS=1,SDN_WISE_DEBUG=1,DEBUG_MELO=$DEBUG_MELO
arm-none-eabi-objcopy -O binary sdn-wise.$TARGET sdn-wise.$TARGET
mv sdn-wise.$TARGET $Folder/sink_NO_SENSORBOARD.$TARGET 
make TARGET=$TARGET BOARD=$BOARD clean


printf "\n\n\n *************************************************** NODE 2 WITHOUT SENSORBOARD *************************************************** \n\n\n\n"
make TARGET=$TARGET BOARD=$BOARD DEFINES=COOJA=0,SINK=0,_MY_ADDRESS=2,SDN_WISE_DEBUG=1,DEBUG_MELO=$DEBUG_MELO
arm-none-eabi-objcopy -O binary sdn-wise.$TARGET sdn-wise_2.$TARGET
mv sdn-wise_2.$TARGET $Folder/node_2.$TARGET 
make TARGET=$TARGET BOARD=$BOARD clean


printf "\n\n\n *************************************************** NODE 3 WITHOUT SENSORBOARD *************************************************** \n\n\n\n"
make TARGET=$TARGET BOARD=$BOARD DEFINES=COOJA=0,SINK=0,_MY_ADDRESS=3,SDN_WISE_DEBUG=1,DEBUG_MELO=$DEBUG_MELO
arm-none-eabi-objcopy -O binary sdn-wise.$TARGET sdn-wise_2.$TARGET
mv sdn-wise_2.$TARGET $Folder/node_3.$TARGET 
make TARGET=$TARGET BOARD=$BOARD clean


printf "\n\n\n *************************************************** SINK WITH SENSORBOARD *************************************************** \n\n\n\n"
make TARGET=$TARGET BOARD=$BOARD SENSORBOARD=$SENSORBOARD DEFINES=COOJA=0,SINK=1,_MY_ADDRESS=1,SDN_WISE_DEBUG=1,DEBUG_MELO=$DEBUG_MELO
arm-none-eabi-objcopy -O binary sdn-wise.$TARGET sdn-wise.$TARGET
mv sdn-wise.$TARGET $Folder/sink_SENSORBOARD.$TARGET 
make TARGET=$TARGET BOARD=$BOARD SENSORBOARD=$SENSORBOARD clean

#make TARGET=$TARGET BOARD=$BOARD DEFINES=COOJA=0,SINK=0,_MY_ADDRESS=3,SDN_WISE_DEBUG=0
#arm-none-eabi-objcopy -O binary sdn-wise.$TARGET sdn-wise_3.$TARGET
#mv sdn-wise_3.$TARGET $Folder/node_3.$TARGET 
#make TARGET=$TARGET BOARD=$BOARD clean


#make TARGET=$TARGET BOARD=$BOARD  DEFINES=COOJA=0,SINK=0,_MY_ADDRESS=4,SDN_WISE_DEBUG=1
#arm-none-eabi-objcopy -O binary sdn-wise.$TARGET sdn-wise_4.$TARGET
#mv sdn-wise_4.$TARGET $Folder/node_4.$TARGET 
#make TARGET=$TARGET BOARD=$BOARD clean


#make TARGET=$TARGET BOARD=$BOARD clean

