#!/bin/bash

sudo apt-get install git build-essential libffi-dev libssl-dev \
                     libreadline6-dev autoconf bison libyaml-dev \
                     zlib1g-dev libncurses5-dev libpq-dev postgresql \
                     apache2 libcurl4-openssl-dev apache2-threaded-dev \
                     libapr1-dev libaprutil1-dev nodejs npm

npm install ws

sudo su postgres -c "createuser jenkins -d -S"

sudo dd if=/dev/zero of=/swap bs=1M count=1024
sudo mkswap /swap
sudo swapon /swap

