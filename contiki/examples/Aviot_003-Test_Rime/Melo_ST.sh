#!/bin/bash

TARGET="stm32f401re-nucleo-spirit1" 
BOARD="ids01a4"
SENSORBOARD="iks01a1"

Folder='Melo_Reali'

FileNameSend="rime-client"
FileNameReceiver="rime-server"

FileNameOut1="1_Send_Cliente"
FileNameOut2="2_Receiver_Server"



if [ -z $1 ] && [ -z $2 ]
then


	sudo rm -rf $Folder

	mkdir $Folder


	make $FileNameSend TARGET=$TARGET BOARD=$BOARD
	arm-none-eabi-objcopy -O binary $FileNameSend.$TARGET $FileNameSend.bin
	mv $FileNameSend.bin $Folder/$FileNameOut1
	make TARGET=$TARGET BOARD=$BOARD clean


	printf "\n\n\n\n\n\n\n\n\n\n\n\n\n"


	make $FileNameReceiver TARGET=$TARGET BOARD=$BOARD
	arm-none-eabi-objcopy -O binary $FileNameReceiver.$TARGET $FileNameReceiver.bin
	mv $FileNameReceiver.bin $Folder/$FileNameOut2
	make TARGET=$TARGET BOARD=$BOARD clean


else




	if [ "$1" = 1 ]
	then


		sudo rm $Folder/$FileNameOut1
				
		make $FileNameSend TARGET=$TARGET BOARD=$BOARD
		arm-none-eabi-objcopy -O binary $FileNameSend.$TARGET $FileNameSend.bin
		mv $FileNameSend.bin $Folder/$FileNameOut1
		make TARGET=$TARGET BOARD=$BOARD clean

	fi



	if [ "$1" = 2 ]
	then


		printf "\n\n\n\n\n\n\n\n\n\n\n\n\n"
		
		sudo rm $Folder/$FileNameOut2

		make $FileNameReceiver TARGET=$TARGET BOARD=$BOARD
		arm-none-eabi-objcopy -O binary $FileNameReceiver.$TARGET $FileNameReceiver.bin
		mv $FileNameReceiver.bin $Folder/$FileNameOut2
		make TARGET=$TARGET BOARD=$BOARD clean

	fi



fi


