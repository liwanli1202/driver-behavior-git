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

# Run unit tests and generate code coverage report

#cd webdash-app/db/
#rake db:seed RAILS_ENV=test  	

#cd ../../



cd test
make clean
make check
gcovr -x -r ../Code/software/apps/ > coverage.xml
sed 's,filename=\",filename=\"Code/software/apps/,' < coverage.xml > coverage-pathfix.xml
mv coverage-pathfix.xml coverage.xml
cd ..

# Run Rails application tests


cd webdash-app
bundle install --without production
mkdir -p features/reports
rake db:schema:load
bundle exec rake db:create:all || exit 2
bundle exec rake db:migrate || exit 3
bundle exec rake ci:setup:minitest test
RAILS_ENV=test bundle exec rake ci:setup:cucumber cucumber
cd ..

# Run ws client test (no client/server authentication, simple printing server)

scripts/test-wsclient.sh

# Run ruby ws server test

scripts/test-wsserver.sh

# Run integration test

#<<<<<<< HEAD
cd Code/software/apps/
#=======
#cd ./cppunit-reports
#touch *.xml
#cd ../..

#cd ./Code/software/apps/
#>>>>>>> fea267
make -f Makefile-DriverAwarenessTest clean
make -f Makefile-DriverAwarenessTest
./DriverAwarenessHeadlessTest.sh

cd ../../..
cd ./test/cppunit-reports
touch *.xml

