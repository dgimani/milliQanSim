#!/bin/bash
#create local build of sim, configured to generate gamma tests of the standalone NaI setup
. buildsetup.sh
cp CMakeLists.txt.XRD CMakeLists.txt
cp include/XRD/brDetectorConstruction.hh.XRD include/mqDetectorConstruction.hh
cp include/XRD/brPMTSD.hh.XRD include/mqPMTSD.hh
cp include/XRD/brScintSD.hh.XRD include/mqScintSD.hh
cp include/XRD/brUserEventInformation.hh.XRD include/mqUserEventInformation.hh
cp include/XRD/brEventAction.hh.XRD include/mqEventAction.hh
cp include/XRD/brShieldingPhysicsList.hh.XRD include/mqShieldingPhysicsList.hh
cp include/XRD/brTrackingAction.hh.XRD include/mqTrackingAction.hh
cp include/XRD/brMuonTrack.hh.XRD include/mqMuonTrack.hh
cp include/XRD/brScintHit.hh.XRD include/mqScintHit.hh
cp include/XRD/brScintRHit.hh.XRD include/mqScintRHit.hh
cp include/XRD/brROOTEvent.hh.XRD include/mqROOTEvent.hh
cp src/XRD/brDetectorConstruction.cc.XRD src/mqDetectorConstruction.cc
cp src/XRD/brHistoManager.cc.XRD src/mqHistoManager.cc
cp src/XRD/brPMTSD.cc.XRD src/mqPMTSD.cc
cp src/XRD/brScintSD.cc.XRD src/mqScintSD.cc
cp src/XRD/brSteppingAction.cc.XRD src/mqSteppingAction.cc
cp src/XRD/brUserEventInformation.cc.XRD src/mqUserEventInformation.cc
cp src/XRD/brEventAction.cc.XRD src/mqEventAction.cc
cp src/XRD/brShieldingPhysicsList.cc.XRD src/mqShieldingPhysicsList.cc
cp src/XRD/brTrackingAction.cc.XRD src/mqTrackingAction.cc
cp src/XRD/brMuonTrack.cc.XRD src/mqMuonTrack.cc
cp src/XRD/brScintHit.cc.XRD src/mqScintHit.cc
cp src/XRD/brScintRHit.cc.XRD src/mqScintRHit.cc
cp src/XRD/brROOTEvent.cc.XRD src/mqROOTEvent.cc
cd build
cmake ../
make -j8
./XRD ../runMac/sidegamma_no_vis.mac
