#!/bin/bash

if [ `basename \`pwd\`` = "scripts" ]; then
  cd ..
fi

if [ `basename \`pwd\`` = "test" ]; then
  cd ..
fi

if [ ! -d "test/" ]; then
  echo "$0: error: must be run at the top of the project tree"
  exit -1
fi

# Bundle install

cd webdash-app
bundle install
cd ..

# Start mock ws server

cd test
if [ ! -d "node_modules/ws" ]; then
  npm install ws
fi
rm -f mock-webdash-server.log
nodejs testFiles/mock-webdash-server.js > mock-webdash-server.log &
sleep 1

# Run ws client to send data to mock server

cd ../webdash-app
bundle exec ruby client/webdash-client.rb ws://0.0.0.0:8126 < ../test/testFiles/DriverAwarenessHeadlessTest-log.txt
sleep 1
kill $!
sleep 1

# Check results

cd ../test
grep 'Listening on port 8126' mock-webdash-server.log || exit 1
grep 'client connected' mock-webdash-server.log || exit 2
grep '>>> frame:2581,LateralPosition:.*,yaw:.*$' mock-webdash-server.log || exit 3

