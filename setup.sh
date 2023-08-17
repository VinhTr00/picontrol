#!/bin/bash

echo "Creating picontrol service"

sudo mkdir -p ~/bin
sudo scp build/picontrol ~/bin/picontrol

sudo scp conf/systemd/picontrol.service /etc/systemd/system/picontrol.service

sudo systemctl daemon-reload
sudo systemctl enable picontrol.service
sudo systemctl start picontrol.service

echo "###############################"
echo "### Setup Service Completed ###"
echo "###############################"