#!/bin/bash
TARGET="sky"
COOJA_BUILD=1
SDN_WISE_DEBUG=1
DEBUG_MELO=1

Folder='Melo_Cooja'

sudo rm -rf $Folder

mkdir $Folder


make TARGET=$TARGET DEFINES=COOJA=$COOJA_BUILD,DEBUG_MELO=$DEBUG_MELO
mv my_first_app.$TARGET $Folder/my_first_app.$TARGET 
make TARGET=$TARGET clean
