#!/bin/bash
TARGET="sky"
COOJA_BUILD=1
SDN_WISE_DEBUG=1
DEBUG_MELO=1

Folder='Melo_Cooja'

sudo rm -rf $Folder

mkdir $Folder

printf "\n\n\n *************************************************** SINK WITH DEBUG_MELO *************************************************** \n\n\n\n"
make TARGET=$TARGET DEFINES=COOJA=$COOJA_BUILD,SINK=1,SDN_WISE_DEBUG=$SDN_WISE_DEBUG,DEBUG_MELO=$DEBUG_MELO
mv sdn-wise.$TARGET $Folder/sink.$TARGET 
make TARGET=$TARGET clean

printf "\n\n\n *************************************************** NODE WITH DEBUG_MELO *************************************************** \n\n\n\n"
make TARGET=$TARGET DEFINES=COOJA=$COOJA_BUILD,SINK=0,SDN_WISE_DEBUG=$SDN_WISE_DEBUG,DEBUG_MELO=$DEBUG_MELO
mv sdn-wise.$TARGET $Folder/node.$TARGET 
make TARGET=$TARGET clean

printf "\n\n\n *************************************************** SINK WITHOUT DEBUG_MELO *************************************************** \n\n\n\n"
make TARGET=$TARGET DEFINES=COOJA=$COOJA_BUILD,SINK=1,SDN_WISE_DEBUG=$SDN_WISE_DEBUG
mv sdn-wise.$TARGET $Folder/sink_NM.$TARGET 
make TARGET=$TARGET clean

printf "\n\n\n *************************************************** NODE WITHOUT DEBUG_MELO *************************************************** \n\n\n\n"
make TARGET=$TARGET DEFINES=COOJA=$COOJA_BUILD,SINK=0,SDN_WISE_DEBUG=$SDN_WISE_DEBUG
mv sdn-wise.$TARGET $Folder/node_NM.$TARGET 
make TARGET=$TARGET clean
