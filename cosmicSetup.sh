#!/bin/bash
#create local build of sim, configured to generate cosmic muons
. buildsetup.sh
cp src/mqDetectorConstruction.cc.default mqDetectorConstruction.cc
cd build
cmake ../
make -j8
./MilliQan ../runMac/specmuonFullPropagated_no_vis.mac
