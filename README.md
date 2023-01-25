# milliQanSim

This is the simulation for the MilliQan Detector. Compatibility: Geant 4.9+, ROOT 5.34 and other ROOT5 or ROOT6 onward (ideally ROOT6)

================  Running the simulation  ========================

To run the sim:
Create a build directory, then use cmake ../ and make
Copy the /config/ directory into your build directory. Adjust config/particles.ini to modify particle properties.
Use ./MilliQan to start the sim. You can either run macros within the geant interface (setting your macropath using /control/macroPath /insert/macro/folder/here and then /control/execute yourmacro.mac), or you can run via batch mode by using: ./MilliQan /path/to/your/macro/yourmacro.mac.

================  Quick run script  =========================

ALTERNATIVELY, TO RUN THE SIMULATION:
I've written some nice macros which build everything, modify everything, and copy everything from scratch for two processes: Studying cosmogenic muons and a standalone sim of a NaI detector. Just run those bash scripts (cosmicSetup.sh, gammaSetup.sh) and sit back. The default number of events is 100, changed in the macros referenced in each script in runMac/ by modifying "/run/beamOn 100" to something else.

================  Visualization  =========================

The visualization macro is vis.mac. To run with visualization:

./MilliQan
/control/macroPath /path/to/your/milliQanSim/runMac/
/control/execute vis.mac
/control/execute specmuonFullPropagated_no_vis.mac (for cosmics/full sim)
or
/control/execute sidegamma_no_vis.mac (for NaI)

When first starting, I suggest seeing how both of cosmics and the NaI geometry work, just to give a sense of what's in here.

You can use quickmuon.mac (and quickmuon_pencil.mac) to throw muons at the sim if you turn on /run/beamOn 1 (or more), or use mcp_vis.mac to throw mCPs at the demonstrator (if your MilliQan.cc is configured properly). These use OGL; if you prefer a different visualization package or your geant build doesn't use OGL, there are several other options available (need to modify/write your own macro though). Also, mcp_vis is provided for visualization and generation of mCPs (just vis.mac + mcp_novis.mac).

================  Macros  =========================

The two macros suggested for running with cosmics and NaI gammas are, respectively:

--specmuonFullPropagated_no_vis.mac (fully simulated distribution of muons from surface of earth; most accurate, the one I use most often)

--sidegamma_no_vis.mac (has lots of xray sources built in you can uncomment, but in particular can be used to simulate Cd109 which milliQan uses for calibration

Other batch macros:

--muon_no_vis.mac (pencil beam of muons with some initial spatial distribution)

--specmuon_no_vis.mac (cos^2 distribution of muons)

--specmuon45_no_vis.mac (cos^2 dist. of muons but truncated at 45 degrees)

--mcp_novis.mac (generates mCPs based on 4vectors. This is the default generation scheme if you just use /run/beamOn 1 assuming MilliQPrimaryGeneratorAction is your particle generator)

You can copy any of these and change the source to other particle types, of course.

To use macros with /gps/ (general particle source) commands, you can comment out MilliQPrimaryGeneratorAction and uncomment mqPrimaryGeneratorAction in MilliQan.cc. For any trajectory-loading simulations (namely beam muons and mCPs), you must use a G4ParticleGun-compatible command (usually most /gps/ commands have a /gun/ equivalent, but not all).


================  MODIFYING SIMULATION SETTINGS  =========================

Basically everything  having to do with geometry is constructed in the mqDetectorConstruction. Everything having to do with saved data is stored in mqSteppingAction (particle stuff) and mqPMTSD (detector stuff), with a couple other pieces in mqTrackingAction also. Physics is defined using mqShieldingPhysicsList. Outputs and datastructures are defined using mqUserEventInformation (for event level things), mq[ParticleName]Track (for particle scoring information) and then mqROOTEvent (to convert data into a ROOT file). If you want to modify any sim feature, go to the relevant files and you'll find the information there. For example, if you want to rotate the demonstrator, or turn on/off the cavern, you'll find that in mqDetectorConstruction. If it's not obvious how to modify something, just email me and I'll be happy to explain. Some of this is detailed in a source file readme: src/FileDescription.txt 


IMPORTANT: When running your macro, be sure to change the destination filepath in runMac/specmuon_no_vis.mac and runMac/muon_no_vis.mac or (equivalent) so your data gets stored somewhere (if you don't enable this, it'll just get named Sim123456789.root or similar). To use macros from within the Geant4 interactive terminal, use /control/macroPath /path/to/milliQanSim/runMac/ followed by /control/execute yourmacro.mac


================  DEPENDENCY INFORMATION  =========================

The following software dependencies are required to run this simulation. If you want to figure out how to expand software support or if you have knowledge about how to expand these software dependencies to support more builds, please email me at the address below (or feel free to make the commits yourself). Additional dependencies for ROOT and Geant4 may be required, see the documentation for those packages for more information.


ROOTv5.30+ (ROOT5 or ROOT6, preferably ROOT6) : Build must be at least v5.30, with ROOT6 supported as well. Currently tested under ROOT5.34 and ROOT6.18. To adjust ROOT5/ROOT6 compatibility, just adjust the line in CMakeLists.txt where ROOT5 or ROOT6 looks for Cintex/Cling, respectively. Then do a find and replace in FindROOT.cmake for CINT and cint <-> CLING and cling for ROOT5 and ROOT6, respectively..


gcc4.8.4+ or gcc5 for ROOT5 (earlier than gcc6), gcc4.8+ for ROOT6 (no upper bound) : Tested working on gcc4.8.5, gcc5.3.0 and gcc5.5.0 for ROOT5. Tested working on gcc7.4.0 on ROOT6. Tested NOT working for ROOT5, gcc4.8.1, at least for one particular build. Might work for an earlier build of gcc4.8, but earlier builds are not tested. Using C++11 standard in general.

Geant4.9.6+ : Build originally developed on Geant4.9.6, confirmed working on Geant4.10.03. Not tested on Geant4.8 or previous.
When building, use the following cmake command, ideally using Geant4.10.5: cmake -DGEANT4_INSTALL_DATA=ON -DGEANT4_USE_OPENGL_X11=ON -DCMAKE_INSTALL_PREFIX=/path/to/geant4.10.5-install /path/to/geant4.10.05.p01

This command has 2 install options: -DGEANT4_INSTALL_DATA=ON, which installs precision Geant4 datasets, and -DGEANT4_USE_OPENGL_X11=ON, which is needed for visualization. You can install other visualization options this way and try them by appropriately modifying the runMac/vis.mac file if you don't want to deal with command line visualization.

Note: A change to a header in one of the physics lists meant there needed to be some migration to move to Geant4.10.6+. It's simple-ish but not a priority right now, so just use Geant4.10.05-p01 for now unless you have a specific need for a newer version of Geant4. If you do, just email me (schmitz@ucsb.edu).

CMake2.6+ : CMake will refuse to build unless this is true. Tested under CMake 3.14.1, should work under CMake3 for all versions but these have not been exhaustively tested

BOOST: To use this simulation, you must add BOOST libraries to your dependencies. There are lots of ways to do this, but the laziest way is to just add the boost libraries to your GEANT include dir. Currently using boost v1.70.0. Note that I currently add BOOST libraries (at least, in cvmfs) by explicitly adding an include line to CMakeLists.txt. CMakeLists.txt doesn't play well with environmental variables, otherwise I'd make this automatic (at least on cvmfs). 

================  CONTACT/QUESTIONS  =========================

For questions, comments, or insights, email Ryan Schmitz at schmitz@ucsb.edu or ryan@cern.ch
