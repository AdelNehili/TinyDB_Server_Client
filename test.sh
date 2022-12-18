#!/bin/bash

make

gnome-terminal --geometry="40x24+0+0" -e "./Server.o test_db.bin"
sleep 0.5
gnome-terminal --geometry="132x24+100+0" -e "./Client.o"
gnome-terminal --geometry="132x24+100+0" -e "./Client.o"

exit 
