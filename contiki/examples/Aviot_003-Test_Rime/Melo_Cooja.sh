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


if [ -z $1 ] && [ -z $2 ]
then

	sudo rm -rf $Folder

	mkdir $Folder


	make $FileNameSend TARGET=$TARGET DEFINES=COOJA=$COOJA_BUILD,DEBUG_MELO=$DEBUG_MELO
	mv rime-client.$TARGET $Folder/1_Send_Cliente.$TARGET 
	make TARGET=$TARGET clean

	make $FileNameReceiver TARGET=$TARGET DEFINES=COOJA=$COOJA_BUILD,DEBUG_MELO=$DEBUG_MELO
	mv rime-server.$TARGET $Folder/2_Receiver_Server.$TARGET 
	make TARGET=$TARGET clean


else


	if [ "$1" = 1 ]
	then

		sudo rm $Folder/$FileNameOut1.sky

		make $FileNameSend TARGET=$TARGET DEFINES=COOJA=$COOJA_BUILD,DEBUG_MELO=$DEBUG_MELO
		mv rime-client.$TARGET $Folder/1_Send_Cliente.$TARGET 
		make TARGET=$TARGET clean

	fi



	if [ "$1" = 2 ]
	then


		sudo rm $Folder/$FileNameOut2.sky


		make $FileNameReceiver TARGET=$TARGET DEFINES=COOJA=$COOJA_BUILD,DEBUG_MELO=$DEBUG_MELO
		mv rime-server.$TARGET $Folder/2_Receiver_Server.$TARGET 
		make TARGET=$TARGET clean

	fi



fi

