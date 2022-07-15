# milliQanSim
 
This is the simulation for the MilliQan Detector. Compatibility: Geant 4.9+, ROOT 5.34 and other ROOT5 or ROOT6 onward (ideally ROOT6)

To change the number of detector layers or the number of detectors per layer, modify the appropriate parameters in mqDetectorConstruction.cc

To run the sim:
Create a build directory, then use cmake ../ and make
Copy the /config/ directory into your build directory. Adjust config/particles.ini to modify particle properties.
Use ./MilliQan to start the sim. You can either run macros within the geant interface (setting your macropath using /control/macroPath /insert/macro/folder/here and then /control/execute yourmacro.mac), or you can run via batch mode by using: ./MilliQan /path/to/your/macro/yourmacro.mac.

ALTERNATIVELY, TO RUN THE SIMULATION:
I've written some nice macros which build everything, modify everything, and copy everything from scratch for two processes: Studying cosmogenic muons and a standalone sim of a NaI detector. Just run those bash scripts (cosmicSetup.sh, gammaSetup.sh) and sit back.

The visualization macro is vis.mac. You can use quickmuon.mac (and quickmuon_pencil.mac) to throw muons at the sim if you turn on /run/beamOn 1 (or more), or use mcp_vis.mac to throw mCPs at the demonstrator. These use OGL; if you prefer a different visualization package or your geant build doesn't use OGL, there are several other options available (need to modify/write your own macro though). Also, mcp_vis is provided for visualization and generation of mCPs (just vis.mac + mcp_novis.mac)

The batch macros (or one to use for large jobs) are:

--muon_no_vis.mac (pencil beam of muons with some initial spatial distribution)

--specmuon_no_vis.mac (cos^2 distribution of muons)

--specmuon45_no_vis.mac (cos^2 dist. of muons but truncated at 45 degrees)

--specmuonFullPropagated_no_vis.mac (fully simulated distribution of muons from surface of earth; most accurate)

--mcp_novis.mac (generates mCPs based on 4vectors. This is the default generation scheme if you just use /run/beamOn 1 assuming MilliQPrimaryGeneratorAction is your particle generator)

--sidegamma_no_vis.mac (has lots of xray sources built in you can uncomment, but in particular can be used to simulate Cd109 which milliQan uses for calibration

You can copy any of these and change the source to other particle types, of course.

To use macros with /gps/ (general particle source) commands, you can comment out MilliQPrimaryGeneratorAction and uncomment mqPrimaryGeneratorAction in MilliQan.cc. For any trajectory-loading simulations (namely beam muons and mCPs), you must use a G4ParticleGun-compatible command (usually most /gps/ commands have a /gun/ equivalent, but not all).


MODIFYING SIMULATION SETTINGS:

Basically everything  having to do with geometry is constructed in the mqDetectorConstruction. Everything having to do with saved data is stored in mqSteppingAction (particle stuff) and mqPMTSD (detector stuff), with a couple other pieces in mqTrackingAction also. Physics is defined using mqShieldingPhysicsList. Outputs and datastructures are defined using mqUserEventInformation (for event level things), mq[ParticleName]Track (for particle scoring information) and then mqROOTEvent (to convert data into a ROOT file). If you want to modify any sim feature, go to the relevant files and you'll find the information there. For example, if you want to rotate the demonstrator, or turn on/off the cavern, you'll find that in mqDetectorConstruction. If it's not obvious how to modify something, just email me and I'll be happy to explain. Some of this is detailed in a source file readme: src/FileDescription.txt 


IMPORTANT: When running your macro, be sure to change the destination filepath in runMac/specmuon_no_vis.mac and runMac/muon_no_vis.mac or (equivalent) so your data gets stored somewhere (if you don't do this, it'll get lost! or maybe renamed something you don't want). To use macros from within the Geant4 interactive terminal, use /control/macroPath /path/to/milliQanDemoSim/runMac/ followed by /control/execute yourmacro.mac


DEPENENCY INFORMATION:

The following software dependencies are required to run this simulation. If you want to figure out how to expand software support or if you have knowledge about how to expand these software dependencies to support more builds, please email me at the address below (or feel free to make the commits yourself). Additional dependencies for ROOT and Geant4 may be required, see the documentation for those packages for more information.


ROOTv5.30+ (ROOT5 or ROOT6) : Build must be at least v5.30, with ROOT6 supported as well. Currently tested under ROOT5.34 and ROOT6.18. To adjust ROOT5/ROOT6 compatibility, just adjust the line in CMakeLists.txt where ROOT5 or ROOT6 looks for Cintex/Cling, respectively. Then do a find and replace in FindROOT.cmake for CINT and cint <-> CLING and cling for ROOT5 and ROOT6, respectively..


gcc4.8.4+ or gcc5 for ROOT5 (earlier than gcc6), gcc4.8+ for ROOT6 (no upper bound) : Tested working on gcc4.8.5, gcc5.3.0 and gcc5.5.0 for ROOT5. Tested working on gcc7.4.0 on ROOT6. Tested NOT working for ROOT5, gcc4.8.1, at least for one particular build. Might work for an earlier build of gcc4.8, but earlier builds are not tested. Using C++11 standard in general.

Geant4.9.6+ : Build originally developed on Geant4.9.6, confirmed working on Geant4.10.03. Not tested on Geant4.8 or previous.

CMake2.6+ : CMake will refuse to build unless this is true. Tested under CMake 3.14.1, should work under CMake3 for all versions but these have not been exhaustively tested

BOOST: To use this simulation, you must add BOOST libraries to your dependencies. There are lots of ways to do this, but the laziest way is to just add the boost libraries to your GEANT include dir. Currently using boost v1.70.0. Note that I currently add BOOST libraries (at least, in cvmfs) by explicitly adding an include line to CMakeLists.txt. CMakeLists.txt doesn't play well with environmental variables, otherwise I'd make this automatic (at least on cvmfs). 


For questions, comments, or insights, email Ryan Schmitz at schmitz@ucsb.edu or ryan@cern.ch
