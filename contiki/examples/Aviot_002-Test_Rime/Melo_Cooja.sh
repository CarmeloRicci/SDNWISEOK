#!/bin/bash
TARGET="sky"
COOJA_BUILD=1
SDN_WISE_DEBUG=1
DEBUG_MELO=1

Folder='Melo_Emulati'

FileNameSend="rime-client"
FileNameReceiver="rime-server"

FileNameOut1="1_Send_Cliente"
FileNameOut2="2_Receiver_Server"


sudo rm -rf $Folder

mkdir $Folder


make $FileNameSend TARGET=$TARGET DEFINES=COOJA=$COOJA_BUILD
mv rime-client.$TARGET $Folder/1_Send_Cliente.$TARGET 
make TARGET=$TARGET clean

make $FileNameReceiver TARGET=$TARGET DEFINES=COOJA=$COOJA_BUILD
mv rime-server.$TARGET $Folder/2_Receiver_Server.$TARGET 
make TARGET=$TARGET clean


