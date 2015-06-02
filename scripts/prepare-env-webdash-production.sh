#!/bin/bash

sudo deluser --remove-home deploy
sudo adduser --system --disabled-password --shell /bin/bash deploy
# On workstation, generate keyfile: ssh-keygen -t rsa -C nca-webdash-deploy
sudo su deploy -c "cd ; mkdir -p .ssh"
sudo su deploy -c "cd ; rm -f .ssh/authorized_keys"
sudo su deploy -c "cd ; cat > .ssh/authorized_keys <<EOF
ssh-rsa AAAAB3NzaC1yc2EAAAABIwAAAQEAxa6F6trAUZl9RGIpxrioaBtZdlg01XV5V+w/kvg+sStwFI2j8VNbq3nmwfAmjyxAn3uwHH1oaIxx+k4CcunubED70BIFG+j7q0zpKf728fRiy8OWWg5NodnlLQ81MQkaALR3105r9k8vD49ZcxAsf/EQ/cI9Gi6kRQyOzEFiCS0ebp0Tg/beea5/lz6KJxIlsVA/jYSIyFHsGksanmIFtXyoBWa293z9DPFIFNRr09o+09uiTzvwVrnkPi+h9TO33bTKhX5pk72H1hMv1UHL1kmyWyu73QRNzPJlrHw+cZYYLTCI8uobrAxBpbXilSsx8o1pwJLQLOAYw7mniTx1cw== mdailey@mdailey-t
ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQCX8KTAZfVTmgM7E60AY7tfaOh/frSQweqidQkLja8EXRZrl0Pb77NZx5T/zLUfprkEAZ1OOcRjcIAYDjlpq5n9NLzwb7VmB5Tt1bvybjERW4c5XbTGlmcapMM/omYAvqAwzu84oTIdb5GE/nCriOWE2+g2QD7bDIILTRoiLTxBwyQJ9lEElf3Hv5yI5DDdjToA16N3NgA+Selp2CTvHo+kzPp68aGNsupCvLqR6Yhsczd+Zx7O+KDki1XchE5nFVk196Z2lljbO7f2B753B3bEdGh93/8aBPzSMS3q+s0HClnj+mWQWXobaEX3IoRoePSZH+/1x7V4ZKEFqICaPL6J st20000135
ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQDJf5brhmewmfIFCQ9Y3jTdExA3tYmJaSZgAJGV3IqlEIhwBOXlWWocnqpqaXc/xbb7gnTOdqxbp5jemjURHb0HuB0X9YyF+jO9IA7n5gupxt0phS0y0yxVFVIt05EUIPZBKwT8c6kgQaikrxkaeTWVrgB3Jl4ZoR4aKOfdblw73iQwLQUU2zK3PIlgooWbUCKXb2MXkx4aVeVsPN+YCCe9GbEN7rETspgDYzeFBX+Dqc1bNlQwKC8Wbq3YmMMxvzzXymkhUDcckIu/W8otr09uZAHty2dqF2ZFALFDRAse1VajS02j3Qg9yWH0oxVE0fN7EpxhnezQOg1iC8Jexl2F sir.ivy@gmail.com
ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQDEGvife6mug1AEXNSLz7qRqMQ33g7TsVmPwVIE8bhE1IPrKi/zbB/ysgLcem/nrdYdkHdT/s4Vkp3hY4KVkkgZmweMvNtW1/eq9kVLHX6VVDLtogztsWWK7xdQ8snR1A/lNdSs7NNif1wywtZBIo8up+/ma0M55Rt/XTru4WPu2sJJzY7/tr5zYRj2c/NZa1/Lm7wdHZkEJ277pP7j6XX5xT5mRlxyTnx2WEhzdlZ7EcAfn3T6aIlLBB0AqK9lshPjNhQS0CHBHZeZdy9uzojF3hF6G7lJjzMZExugMYUrI6Kq2gkB1af7r7CBXV11W96jEGbC89YrcKufKRaadxiV st20000368
EOF"

# Add all deployers to the authorized_keys of the deploy user

sudo apt-get install git build-essential libffi-dev libssl-dev \
                     libreadline6-dev autoconf bison libyaml-dev \
                     zlib1g-dev libncurses5-dev libpq-dev postgresql \
                     apache2 libcurl4-openssl-dev apache2-threaded-dev \
                     libapr1-dev libaprutil1-dev

sudo su postgres -c "createuser -d -S deploy"

# On workstation:
# 1. Add capistrano, capistrano-rbenv, capistrano-rbenv-install to Gemfile
# 2. bundle install
# 3. cap install
# 4. Edit Capfile
# 5. Edit config files
# 6. cap production deploy
# 7. If production database is not ready, run cap production deploy:db:create

# Add to deploy .bashrc:
# export PATH="~/.rbenv/bin:$PATH"
# eval \"\$(rbenv init -)\"
# export PATH=\"~/.rbenv/plugins/ruby-build/bin:$PATH\"
# rbenv global 2.2.0

sudo dd if=/dev/zero of=/swap bs=1M count=1024
sudo mkswap /swap
sudo swapon /swap

sudo su deploy -c 'cd ~/webdash/current ; rbenv local 2.1.5 ; bundle exec passenger-install-apache2-module'

sudo a2dissite 000-default
sudo sh -c "cat > /etc/apache2/sites-available/webdash.conf <<EOF
   <VirtualHost *:80>
      ServerName nca-webdash.pineapplevisionsystems.com
      # !!! Be sure to point DocumentRoot to 'public'!
      DocumentRoot /home/deploy/webdash/current/public    
      <Directory /home/deploy/webdash/current/public>
         # This relaxes Apache security settings.
         AllowOverride all
         # MultiViews must be turned off.
         Options -MultiViews
         # Uncomment this if you're on Apache >= 2.4:
         Require all granted
      </Directory>
   </VirtualHost>
EOF"
sudo a2ensite webdash

sudo sh -c "cat > /etc/apache2/mods-available/passenger.conf <<EOF
   <IfModule mod_passenger.c>
     PassengerRoot /home/deploy/webdash/shared/bundle/ruby/2.1.0/gems/passenger-4.0.56
     PassengerDefaultRuby /home/deploy/.rbenv/versions/2.1.5/bin/ruby
   </IfModule>
EOF"
sudo sh -c "cat > /etc/apache2/mods-available/passenger.load <<EOF
   LoadModule passenger_module /home/deploy/webdash/shared/bundle/ruby/2.1.0/gems/passenger-4.0.56/buildout/apache2/mod_passenger.so
EOF"
sudo a2enmod passenger

sudo service apache2 restart

sudo su -c "cat > /etc/init/god.conf <<EOF

description \"God Process Monitoring\"
author \"Christoph Geschwind <christoph@mixxt.net>\"

start on runlevel [2345]
stop on runlevel [!2345]

respawn
respawn limit 5 20 # stop respawning if it fails 5 times in 20 seconds

pre-start script
  mkdir -p /var/run/god # create gods pid directory, you might not need this
end script

chdir /home/deploy/webdash/current

script
  echo $$ > /var/run/god/god.pid
  export RBENV_ROOT=/home/deploy/.rbenv
  exec /home/deploy/.rbenv/bin/rbenv exec bundle exec god -D -c /home/deploy/webdash/current/config/vehicle-connector.god
end script

EOF"

