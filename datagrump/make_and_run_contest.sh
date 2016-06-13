#!/bin/bash

export DATE=$(date +"%s")

make clean;
make;

cat controller.cc > ./runs/cc/$DATE &
cat controller.hh > ./runs/hh/$DATE &
cat run-contest > ./runs/rc/$DATE &
./run-contest anubhav-duffield 2>&1 | tee ./runs/f/$DATE 


cat /tmp/contest_uplink_log > ./runs/sul/$DATE
/usr/local/bin/mm-throughput-graph 500 ./runs/sul/$DATE > throughput.html 
/usr/local/bin/mm-delay-graph ./runs/sul/$DATE > delay.html
