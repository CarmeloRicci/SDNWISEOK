#!/bin/bash
TARGET="stm32f401re-nucleo-spirit1" 
BOARD="ids01a4"
SENSORBOARD="iks01a1"
FileNameSINK="my_first_app_SINK"
FileNameNODE="my_first_app_NODE"

FileNameOutSINK="Sink"
FileNameOutNODE="Node"

Folder='Melo_ST'

#sudo rm -rf $Folder

#mkdir $Folder


#printf "\n\n\n *************************************************** NODE *************************************************** \n\n\n\n"
#make $FileNameNODE TARGET=$TARGET BOARD=$BOARD DEFINES=_MY_ADDRESS=1,SINK=0
#arm-none-eabi-objcopy -O binary $FileNameNODE.$TARGET my_first_app_node.bin
#mv my_first_app_node.bin $Folder/Node.$TARGET 
#make TARGET=$TARGET BOARD=$BOARD clean


#printf "\n\n\n *************************************************** SINK *************************************************** \n\n\n\n"
#make $FileNameSINK TARGET=$TARGET BOARD=$BOARD DEFINES=_MY_ADDRESS=2,SINK=1
#arm-none-eabi-objcopy -O binary $FileNameSINK.$TARGET my_first_app_sink.bin
#mv my_first_app_sink.bin $Folder/Sink.$TARGET 
#make TARGET=$TARGET BOARD=$BOARD clean




##make TARGET=$TARGET BOARD=$BOARD DEFINES=COOJA=0,SINK=0,_MY_ADDRESS=3,SDN_WISE_DEBUG=0
##arm-none-eabi-objcopy -O binary sdn-wise.$TARGET sdn-wise_3.$TARGET
##mv sdn-wise_3.$TARGET $Folder/node_3.$TARGET 
##make TARGET=$TARGET BOARD=$BOARD clean


##make TARGET=$TARGET BOARD=$BOARD  DEFINES=COOJA=0,SINK=0,_MY_ADDRESS=4,SDN_WISE_DEBUG=1
##arm-none-eabi-objcopy -O binary sdn-wise.$TARGET sdn-wise_4.$TARGET
##mv sdn-wise_4.$TARGET $Folder/node_4.$TARGET 
##make TARGET=$TARGET BOARD=$BOARD clean




if [ -z $1 ] && [ -z $2 ]
then


    sudo rm -rf $Folder

    mkdir $Folder


    printf "\n\n\n *************************************************** NODE *************************************************** \n\n\n\n"
    make $FileNameNODE TARGET=$TARGET BOARD=$BOARD DEFINES=_MY_ADDRESS=1,SINK=0
    arm-none-eabi-objcopy -O binary $FileNameNODE.$TARGET my_first_app_node.bin
    mv my_first_app_node.bin $Folder/$FileNameOutNODE.$TARGET 
    make TARGET=$TARGET BOARD=$BOARD clean


    printf "\n\n\n *************************************************** SINK *************************************************** \n\n\n\n"
    make $FileNameSINK TARGET=$TARGET BOARD=$BOARD DEFINES=_MY_ADDRESS=2,SINK=1
    arm-none-eabi-objcopy -O binary $FileNameSINK.$TARGET my_first_app_sink.bin
    mv my_first_app_sink.bin $Folder/$FileNameOutSINK.$TARGET 
    make TARGET=$TARGET BOARD=$BOARD clean



else



	if [ "$1" = 1 ]
	then


		sudo rm $Folder/$FileNameOutNODE.$TARGET
		
        printf "\n\n\n *************************************************** NODE *************************************************** \n\n\n\n"

        make $FileNameNODE TARGET=$TARGET BOARD=$BOARD DEFINES=_MY_ADDRESS=1,SINK=0
        arm-none-eabi-objcopy -O binary $FileNameNODE.$TARGET my_first_app_node.bin
        mv my_first_app_node.bin $Folder/$FileNameOutNODE.$TARGET 
        make TARGET=$TARGET BOARD=$BOARD clean

	fi



	if [ "$1" = 2 ]
	then

        sudo rm $Folder/$FileNameOutSINK.$TARGET

		printf "\n\n\n *************************************************** SINK *************************************************** \n\n\n\n"

        make $FileNameSINK TARGET=$TARGET BOARD=$BOARD DEFINES=_MY_ADDRESS=2,SINK=1
        arm-none-eabi-objcopy -O binary $FileNameSINK.$TARGET my_first_app_sink.bin
        mv my_first_app_sink.bin $Folder/$FileNameOutSINK.$TARGET 
        make TARGET=$TARGET BOARD=$BOARD clean

	fi



fi


