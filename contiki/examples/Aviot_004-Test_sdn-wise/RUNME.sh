#!/bin/bash
TARGET="sky"
COOJA_BUILD=1
SDN_WISE_DEBUG=1

mkdir cooja_firmwares

make TARGET=$TARGET DEFINES=COOJA=$COOJA_BUILD,SINK=1,SDN_WISE_DEBUG=$SDN_WISE_DEBUG,DEBUG_MELO=1
mv my_first_app.$TARGET cooja_firmwares/my_first_appbin.$TARGET 
make TARGET=$TARGET clean

