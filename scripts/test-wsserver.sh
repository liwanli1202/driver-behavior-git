#!/bin/bash

set -e

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

# Start ruby ws server

cd webdash-app
bundle install
RAILS_ENV=test bundle exec rake vehicle_connector:start > log/ws_server.log &
sleep 2

# Run ws client to send data to mock server

bundle exec ruby client/webdash-client.rb wss://localhost:8126 client/webdash-client-1.key client/webdash-client-1.crt < ../test/testFiles/DriverAwarenessHeadlessTest-log.txt
sleep 1
kill $!
sleep 1

# Check results

grep 'Websocket connection opened at 8126' log/ws_server.log || { echo "No connection open" && exit 2; }
grep 'f,*,*,*,*' log/ws_server.log || { echo "Last frame not processed" && exit 3; }

#grep 'frame:2581,LateralPosition:.*,yaw:.*$' log/ws_server.log || { echo "Last frame not processed" && exit 3; }

