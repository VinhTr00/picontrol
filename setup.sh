#!/bin/bash

echo "Creating dropping service"

sudo mkdir -p ~/bin
sudo scp build/main ~/bin/main

sudo scp conf/systemd/dropping.service /etc/systemd/system/dropping.service

sudo systemctl daemon-reload
sudo systemctl enable dropping.service
sudo systemctl start dropping.service

echo "###############################"
echo "### Setup Service Completed ###"
echo "###############################"