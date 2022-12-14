#!/bin/bash

make

gnome-terminal --geometry="40x24+0+0" -e "./Server"
sleep 0.5
gnome-terminal --geometry="132x24+100+0" -e "./Client"
gnome-terminal --geometry="132x24+100+0" -e "./Client"

exit 
