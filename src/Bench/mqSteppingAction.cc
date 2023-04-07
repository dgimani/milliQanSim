/*
 * mqSteppingAction.cc
 *
 *  Created on: 18.04.2017
 *      Author: schmitz
 */
#include "mqSteppingAction.hh"
#include "mqEventAction.hh"
#include "mqTrackingAction.hh"
#include "mqPMTSD.hh"
#include "mqScintSD.hh"
#include "mqUserTrackInformation.hh"
#include "mqUserEventInformation.hh"
#include "mqSteppingMessenger.hh"
#include "mqHistoManager.hh"
//#include "mqUserRegionInformation.hh"

// Includes Physical Constants and System of Units
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4SteppingManager.hh"
#include "G4SDManager.hh"
#include "G4EventManager.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessType.hh"
#include "G4HadronicProcessType.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4OpBoundaryProcess.hh"
#include "Randomize.hh"
#include "G4RunManager.hh"
#include <math.h>       /* pow */
#include <vector>
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
mqSteppingAction::mqSteppingAction(mqHistoManager* histo)
  :oneStepPrimaries(false), histoManager(histo), killPhoton(false)
  //killPhoton(true)
{
  steppingMessenger = new mqSteppingMessenger(this);
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
mqSteppingAction::~mqSteppingAction()
{
//  delete steppingMessenger;
  //delete histoManager;
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void mqSteppingAction::UserSteppingAction(const G4Step * theStep){
  G4Track* theTrack = theStep->GetTrack();
  G4int trackID = theTrack->GetTrackID();

  mqUserTrackInformation* trackInformation
  	  	  	  	  	  	  	 =(mqUserTrackInformation*)theTrack->GetUserInformation();
  mqUserEventInformation* eventInformation
    						=(mqUserEventInformation*)G4EventManager::GetEventManager()
    						->GetConstCurrentEvent()->GetUserInformation();

  G4StepPoint* thePrePoint     = theStep->GetPreStepPoint();
  G4VPhysicalVolume* thePrePV  = thePrePoint->GetPhysicalVolume();
 // G4VLogicalVolume* thePreLV   = thePrePV->GetLogicalVolume();
  
 // mqUserRegionInformation* thePreRInfo
 //						 = (mqUserRegionInformation*)(thePreLV->GetRegion()->GetUserInformation());

  G4StepPoint* thePostPoint    = theStep->GetPostStepPoint();
  G4VPhysicalVolume* thePostPV = thePostPoint->GetPhysicalVolume();
 //G4VLogicalVolume* thePostLV  = thePostPV->GetLogicalVolume();

 // mqUserRegionInformation* thePostRInfo
 //  						 = (mqUserRegionInformation*)(thePostLV->GetRegion()->GetUserInformation());
  
 //G4cout << "Start Volume " << thePrePV->GetName()<< G4endl;
  G4OpBoundaryProcessStatus boundaryStatus = Undefined;
  static G4OpBoundaryProcess* boundary     = NULL;

  G4ProcessManager* pm;
  G4ProcessVector* pv;
  //find the boundary process only once
  if(!boundary){
    pm = theStep->GetTrack()->GetDefinition()->GetProcessManager();
    G4int nprocesses = pm->GetProcessListLength();
    pv = pm->GetProcessList();
    G4int i;
    for( i=0;i<nprocesses;i++){
      if((*pv)[i]->GetProcessName()=="OpBoundary"){
	boundary = (G4OpBoundaryProcess*)(*pv)[i];
	break;
      }
    }
  }
  G4double myStepLength = 0.0;
  myStepLength = theStep->GetStepLength();
  G4double myEnergyDelta = 0.0;
  G4double myEnergyEDep = 0.0;
  G4double eventTime = 0.0;
  G4double totalEnergy = 0.0;
  G4String particleName = "";

  particleName = theTrack->GetDefinition()->GetParticleName();
  myEnergyEDep = theStep->GetTotalEnergyDeposit();
  myEnergyDelta = theStep->GetDeltaEnergy();
  G4int stepID = theStep->GetTrack()->GetCurrentStepNumber();
  G4int eventID = eventInformation->GetEventID();
  mqScintSD* scintSD;
  mqPMTSD* pmtSD;
  //  if(eventID % 1000==0) G4cout << "Event ID is: " << eventID << G4endl;
  //
  // Get information about the start point of the step
  //
  G4ThreeVector myStartPosition;
  G4ThreeVector myStartDirection;
  G4double myStartXYMagnitude;
  G4String myStartProcessName = "";
  G4String myStartVolumeName = "";
  G4String myEndVolumeName = ""; //using this, so defining it here instead of below

  G4int preCopyNo = thePrePoint->GetTouchable()->GetCopyNumber(); //also using these two
  G4int postCopyNo = thePostPoint->GetTouchable()->GetCopyNumber();
  const G4Event* evt = G4RunManager::GetRunManager()->GetCurrentEvent();
  
  G4double myStartTime = -1.0;
  G4ThreeVector myStartMomentumDirection;
  G4double myStartKineticEnergy = 0.;
  G4String startMat = "";
  G4String myCreatorProcess = "";
  G4double cosDetect= 0;//myStartDirection.dot(G4ThreeVector(0,0,1));
//  G4double pi=3.14159265359; //using this explicitly since 
  G4double angleDetect=0;//acos(cosDetect)*180.0/PI;
  G4double polyAngleFitResult=0;
  //Test if there is a step
  if (thePrePoint != NULL) {
	  if (thePrePoint->GetPhysicalVolume() != NULL)
		  myStartVolumeName = thePrePoint->GetPhysicalVolume()->GetName();
	  if (thePrePoint->GetProcessDefinedStep() != NULL)
		  myStartProcessName =
				  thePrePoint->GetProcessDefinedStep()->GetProcessName();
	  startMat = thePrePoint->GetMaterial()->GetName();
	  myStartPosition = thePrePoint->GetPosition();
	  myStartDirection = thePrePoint->GetMomentumDirection();
	  myStartTime = thePrePoint->GetGlobalTime();
	  myStartKineticEnergy = thePrePoint->GetKineticEnergy();
  	  myStartXYMagnitude = sqrt(pow(myStartPosition.x(),2)+pow(myStartPosition.y(),2));
  
  }

    G4TrackVector* fSecondary;
  if(theTrack->GetParentID()==0){
    //This is a primary track

    fSecondary=fpSteppingManager->GetfSecondary();
    G4int tN2ndariesTot = fpSteppingManager->GetfN2ndariesAtRestDoIt()
      + fpSteppingManager->GetfN2ndariesAlongStepDoIt()
      + fpSteppingManager->GetfN2ndariesPostStepDoIt();

    	//If we havent already found the conversion position and there were
    	//secondaries generated, then search for it


  }
 
  if(!thePostPV){//out of world
    return;
  }

  G4ParticleDefinition* particleType = theTrack->GetDefinition();

  // kill optical photons which are below pmt sensitivity
  // for now kill photons which were created after 10^20 ns
  //if(killPhoton && (particleType==G4OpticalPhoton::OpticalPhotonDefinition()) && theTrack->GetGlobalTime() > pow(10.,15.)* ns)//(theTrack->GetTotalEnergy() < 1.9*eV))
  //	  theTrack->SetTrackStatus(fStopAndKill);

  //if ( trackInformation->GetReflectionCount() > 200) G4cout << "reflection " << trackInformation->GetReflectionCount() << G4endl;
  //if ( trackInformation->GetInternalReflectionCount() > 200) G4cout << "internal reflection " << trackInformation->GetInternalReflectionCount() << G4endl;

  //kill photons which are reflected internally (i.e. dielectric-dielectric) more than 200 times
  //if(trackInformation->GetInternalReflectionCount() > 1000)
  //	  theTrack->SetTrackStatus(fStopAndKill);


  if((particleType==G4OpticalPhoton::OpticalPhotonDefinition())){

          myCreatorProcess = theTrack->GetCreatorProcess()->GetProcessName();

    //Was the photon absorbed by the absorption process
    if(thePostPoint->GetProcessDefinedStep()->GetProcessName()=="OpAbsorption"){
      eventInformation->IncAbsorption();
      trackInformation->AddTrackStatusFlag(absorbed);
    }



    boundaryStatus=boundary->GetStatus();

   //
   // for debugging
   //
   //boundary->DumpInfo();
   //G4cout << "========================================================" << G4endl;
   //G4cout << "TrackID "  <<  trackID << G4endl;
   //boundary->SetVerboseLevel(4);
   //G4cout << "========================================================" << G4endl;


    //Check to see if the particle was actually at a boundary
    //Otherwise the boundary status may not be valid
    //Prior to Geant4.6.0-p1 this would not have been enough to check
    if(thePostPoint->GetStepStatus()==fGeomBoundary){
      switch(boundaryStatus){


      //Undefined
      //FresnelRefraction
      //FresnelReflection
      //TotalInternalReflection
      //LambertianReflection
      //LobeReflection
      //SpikeReflection
      //BackScattering
      //Absorption
      //Detection
      //NotAtBoundary
      //SameMaterial
      //StepTooSmall
      //NoRINDEX


      case Undefined:
      break;
      case NoRINDEX:

      break;
      case Absorption:
   	  trackInformation->AddTrackStatusFlag(boundaryAbsorbed);
    	  eventInformation->IncBoundaryAbsorption();
	  //G4cout << thePostPV->GetName() << G4endl;
	break;
      case Detection:
	{       
	//Note, this assumes that the volume causing detection
                      //is the photocathode because it is the only one with
	                  //non-zero efficiency
	//Trigger sensitive detector manually

	//determine cosine of angle of incidence  
        cosDetect= abs(myStartDirection.dot(G4ThreeVector(0,0,1)));
	//G4cout << "my start direction: " << myStartDirection.x() << " " << myStartDirection.y() << " " << myStartDirection.z() << G4endl;
		
	//determine angle of incidence
	angleDetect=acos(cosDetect)*180/M_PI; //should always be less than 90 degrees

        //using a 6th degree polynomial fit based on the data in OpticalData/PMTAngularSpec.txt. Using 6th degree because detections are infrequent compared to total # of steps so it's not that expensive
        polyAngleFitResult=3.93E-11*pow(angleDetect,6)-9.69E-09*pow(angleDetect,5)+9.10E-07*pow(angleDetect,4)-4.26E-05*pow(angleDetect,3)+9.17E-04*pow(angleDetect,2)-8.22E-03*angleDetect+9.98E-01;	

	//if we don't think we're optimally coupled, this second 62 degrees term below is the glass-scintillator critical angle correction. Specifically, it's the correction for silicone oil-to-scintillator. The borosilicate glass-to-scintillator correction is 72.9 degrees. Enabling this cutoff for silicone oil for now since we're doing the optical coupling manually and it could possibly be a poor coupling, but it should mostly not matter for a bar since the detection angles are much sharper than 62 degrees most of the time
	if(G4UniformRand()<polyAngleFitResult && angleDetect<62){
	//if(G4UniformRand()<cosDetect){                               //if we think we did a good job coupling, then just use this relationship (which falls off at high theta anyways)

	    //G4cout << "hit registered!" << G4endl;
	    //G4cout << theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() << G4endl;
	    //if(G4UniformRand()<0.01) G4cout << "Cosine is " << cosDetect << G4endl;
	    myEndVolumeName=theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();
//		if (myEndVolumeName.contains("PMTParamPhys") || myEndVolumeName.contains("ScintSlabPMT") || myEndVolumeName.contains("ScintPanelPMT") || myEndVolumeName.contains("PMTFourth")){
	    G4String sdPMTName="PMT_SD";
	    pmtSD = (mqPMTSD*)G4SDManager::GetSDMpointer()->FindSensitiveDetector(sdPMTName);
	    //pmtSD->GetCollectionName(1);
	    if(pmtSD){
	        pmtSD->ProcessHits_constStep(theStep,NULL);
	  //  G4cout << "Process Hits PMT" << G4endl;
	    }
	    trackInformation->AddTrackStatusFlag(hitPMT);

//		}
	}
	}
	  break;
      case FresnelReflection:
	  trackInformation->IncReflections();
	  trackInformation->IncInternalReflections();
    	  
	  break;
      case TotalInternalReflection:
	  if(G4UniformRand()<0.01) theTrack->SetTrackStatus(fStopAndKill);
	  trackInformation->IncReflections();
	  trackInformation->IncInternalReflections();
    	  break;

      case SpikeReflection:
	  trackInformation->IncReflections();
	  trackInformation->IncInternalReflections();
    	  
	  break;
      case LambertianReflection: //diffuse surfaces only
	  trackInformation->IncReflections();
	  trackInformation->IncInternalReflections();

    	  break;


      default:
    	  break;
      }
    }
  }


  //
  // search and record

  // 2) interaction of neutrons
  //





	//Get information about the end point of the step

//	G4String myEndVolumeName = "";
	G4String myEndProcessName = "";
	G4double myEndTime = -1.0;
	G4ThreeVector myEndPosition;
	G4ThreeVector myEndDirection;
	G4double myEndKineticEnergy = -1.0;
	G4double sumEnergyDep=0;
	G4double sumEnergyDelta=0;
	G4String endMat = "";
	G4int nbOfInteractions = 0;
	G4int nbOfElastics = 0;


	if (thePostPoint != NULL) {
		myEndKineticEnergy = thePostPoint->GetKineticEnergy();
		if (thePostPoint->GetPhysicalVolume() != NULL) {
			myEndVolumeName = thePostPoint->GetPhysicalVolume()->GetName();
			endMat = thePostPoint->GetMaterial()->GetName();
		}

		if (thePostPoint->GetProcessDefinedStep() != NULL) {
			myEndProcessName =
					thePostPoint->GetProcessDefinedStep()->GetProcessName();
		}


		myEndPosition = thePostPoint->GetPosition();
		myEndDirection = thePostPoint->GetMomentumDirection();
		myEndTime = thePostPoint->GetGlobalTime();

//		if(particleName.contains("mu") && myStartVolumeName.contains("World") && myEndVolumeName=="barParam"){G4cout << "leaving world, entering " << myEndVolumeName << G4endl;}
		//count interactions
		nbOfInteractions = 1;
		//flag elastic scattering
		if(myEndProcessName.contains("Elastic") !=0 ) nbOfElastics = 1;

		
//		if(myEndVolumeName.contains("World") && myStartVolumeName=="rockPhysic") G4cout << "Particle " << particleName << " exiting rock" << G4endl;


////////////// use this to check on particle energies and hit times in specific detectors/////////
///*
                 if(particleName.contains("monopole") //monopolemu 
                 //if(particleName.contains("mu") //mumonopole 
//                 if((particleName.contains("e-") || particleName.contains("gamma") || particleName.contains("e+"))//mumonopole if(particleName.contains("mu") //mumonopole 
			&& (!myStartVolumeName.contains("plScin") && !myStartVolumeName.contains("slab_physic") && !myStartVolumeName.contains("panel_physic"))
			&& (myEndVolumeName.contains("plScin") || myEndVolumeName.contains("slab_physic") || myEndVolumeName.contains("panel_physic"))
//			&& (myStartVolumeName.contains("barStack") || myStartVolumeName.contains("World"))	//particle started in the stack, or came from the world
//			&& (myEndVolumeName.contains("barParam")
//				|| myEndVolumeName.contains("panel")
//				|| myEndVolumeName.contains("Panel")
//				|| myEndVolumeName.contains("slab")
//				|| myEndVolumeName.contains("Slab")) //particle ends up in one of the bar wrapping layers, somewhere, afterwards
//			&& (postCopyNo < 18) //hit bar
			){ //also, particle entered one of the middle layers
//			&& theStep->GetPostStepPoint()->GetTouchable()->GetVolume()->GetName().back()-48==1){ //particle does so in the middlemost layer in particular
			
			
			totalEnergy = thePostPoint->GetTotalEnergy()/MeV;
			G4cout << "Particle energy is " << totalEnergy << " MeV" << G4endl;

			eventTime = theStep->GetPostStepPoint()->GetGlobalTime()/ns;
			G4cout << "Event time is " << eventTime << " ns" << G4endl;

			G4cout << "Event number is " << evt->GetEventID() << G4endl;

			G4cout << "Track ID is " << trackID << G4endl;

			G4cout << "Start Volume name is : " << myStartVolumeName << G4endl;
			G4cout << "End Volume name is : " << myEndVolumeName << G4endl;
			/*
			G4ThreeVector volpos(0.0,0.0,0.0);
			const G4VTouchable* touch=thePostPoint->GetTouchable();
			int i=0;

			while(touch->GetVolume(i)->GetName() != "World"){
				volpos += touch->GetVolume(i)->GetTranslation();
				i++;
			}

			G4cout << "Scint Volume Position is: " << G4endl
						<< "X: " << volpos.x()/m << G4endl
						<< "Y: " << volpos.y()/m << G4endl
						<< "Z: " << volpos.z()/m << G4endl;
			*/
/*
			G4int barCopyNum = theStep->GetPostStepPoint()->GetTouchable()
						->GetCopyNumber(2);
			if(barCopyNum<(4*4*2*2+1)){
				G4cout << "Particle " << particleName << " hit bar!" << G4endl;
				eventInformation->SetBarHit(eventInformation->GetBarHit()+1);
			} else if(barCopyNum<(4*4*2*2+5)){
				G4cout << "Particle " << particleName << " hit slab!" << G4endl;
				eventInformation->SetSlabHit(eventInformation->GetSlabHit()+1);
			} else {
				G4cout << "Particle " << particleName << " hit panel!" << G4endl;
				eventInformation->SetPanelHit(eventInformation->GetPanelHit()+1);
			}
*/  
			G4String sdScintName="Scint_SD";
  			scintSD = (mqScintSD*)G4SDManager::GetSDMpointer()->FindSensitiveDetector(sdScintName);
			scintSD->ProcessHitsEnter(theStep,NULL);
//			std::ofstream particleBarHit;
//			particleBarHit.open("/media/ryan/Storage/computing/mqFullSim/data/particleHitData_midLayerMuon.txt",std::ofstream::out | std::ofstream::app);
//			int barNum = postCopyNo;
//			particleBarHit << evt->GetEventID() << "	" << trackID << "	" << barNum << "	" << particleName << "	" << totalEnergy << "	" << eventTime << "	" << G4endl;
//			particleBarHit.close();
		       }
//*/
//                 if(particleName.contains("mu") //mumonopole 
                 //if((particleName.contains("e-") || particleName.contains("gamma") || particleName.contains("e+"))//mumonopole if(particleName.contains("mu") //mumonopole 
                 if(particleName.contains("monopole") //monopolemu 
			&& (myStartVolumeName.contains("plScin") || myStartVolumeName.contains("slab_physic") || myStartVolumeName.contains("panel_physic"))
			&& (!myEndVolumeName.contains("plScin") && !myEndVolumeName.contains("slab_physic") && !myEndVolumeName.contains("panel_physic"))
			){ //also, particle entered one of the middle layers

  			G4String sdScintName="Scint_SD";
  			scintSD = (mqScintSD*)G4SDManager::GetSDMpointer()->FindSensitiveDetector(sdScintName);
			G4cout << "exiting scint! copy number is " << preCopyNo << G4endl;
			scintSD->ProcessHitsExit(theStep,NULL);
			G4cout << "========================================================================" << G4endl;
		 	}

//		if(particleName.contains("mu") && myStartVolumeName.contains("plScin"))
//			{G4cout << "muon in bars!" << G4endl;}
//////////////////////////////////////////////////////////////////////////////////////////////
/*
 //using this to kill muons that we don't want to hit the detector, since we trigger on muons and I don't want to waste sim runtime
///////////////////// trigger check ////////////////////////////
		if(particleName.contains("mu") && myStartVolumeName.contains("World")
				&& (myEndVolumeName.contains("barParam") || myEndVolumeName.contains("Panel"))
				&& !(eventInformation->GetMuonTrigger())) {theStep->GetTrack()->SetTrackStatus(fStopAndKill);
//	G4cout << "Track killed because it didn't trigger!" << G4endl; 
//	G4cout << "trigger instances; Top: " << eventInformation->GetMuonTrack(theStep->GetTrack()->GetTrackID())->GetMuonTriggerUp() << " Bottom: " << eventInformation->GetMuonTrack(theStep->GetTrack()->GetTrackID())->GetMuonTriggerLow() << G4endl;}
			}
///////////////////////////////////////////////////////////////
*/	
	}

/*
//////////// neutron interaction manager /////////////
	if( particleName.contains("neutron") ){

		if (eventInformation->GetNeutronTracks()->size() > 0){ //there are neutron tracks
		eventInformation->GetNeutronTrack(theStep->GetTrack()->GetTrackID())->AddNbOfElastics(nbOfElastics);
		eventInformation->GetNeutronTrack(theStep->GetTrack()->GetTrackID())->AddNbOfInteractions(nbOfInteractions);
		}
	}
*/

	if( particleName.contains("mu") ){
		//sum energy deposit
		sumEnergyDep=eventInformation->GetMuonTrack(theStep->GetTrack()->GetTrackID())->GetEnergyDeposit()+myEnergyEDep;
		eventInformation->GetMuonTrack(theStep->GetTrack()->GetTrackID())->SetEnergyDeposit(sumEnergyDep);
/*		
		//muon enters scintillator
		if((!myStartVolumeName.contains("scint") && !myStartVolumeName.contains("Scint"))
			&& (myEndVolumeName.contains("scint") || myEndVolumeName.contains("Scint"))){
	  		
			G4String sdScintName="Scint_SD";
			mqScintSD* scintSD = (mqScintSD*)G4SDManager::GetSDMpointer()->FindSensitiveDetector(sdScintName);
			G4cout << "entering scint! copy number is " << postCopyNo << G4endl;
			scintSD->ProcessHitsEnter(theStep,NULL);
		}
*/
		//muon enters scintillator
/*		
		if((myStartVolumeName.contains("scint") || myStartVolumeName.contains("Scint"))
			&& (!myEndVolumeName.contains("scint") && !myEndVolumeName.contains("Scint"))){
			
			G4String sdScintName="Scint_SD";
			mqScintSD* scintSD = (mqScintSD*)G4SDManager::GetSDMpointer()->FindSensitiveDetector(sdScintName);
			G4cout << "exiting scint! copy number is " << preCopyNo << G4endl;
			scintSD->ProcessHitsExit(theStep,NULL);
		}
*/
//	G4cout << "muon process is: " << myEndProcessName << G4endl;
	}

/*
///////////// gamma interaction manager ///////////////
	if( particleName.contains("gamma") ){
		//sum energy deposit
		sumEnergyDep=eventInformation->GetGammaTrack(theStep->GetTrack()->GetTrackID())->GetEnergyDeposit()+myEnergyEDep;
		eventInformation->GetGammaTrack(theStep->GetTrack()->GetTrackID())->SetEnergyDeposit(sumEnergyDep);
//	if(theTrack->GetCurrentStepNumber()==1) G4cout << "Gamma track creator process is: " << theTrack->GetCreatorProcess()->GetProcessName() << " and energy is " << eventInformation->GetGammaTrack(trackID)->GetInitialEnergy() << G4endl;	
	}
*/
//
/*
	if(particleName.contains("e+") || particleName.contains("e-")){
		if(theTrack->GetCurrentStepNumber()==1) G4cout << "Electron track creator process is: " << theTrack->GetCreatorProcess()->GetProcessName() << " and energy is " << theTrack->GetTotalEnergy() << G4endl;// " *and kinetic energy is* " << theTrack->GetKineticEnergy() << G4endl;
		}

*/		sumEnergyDep=eventInformation->GetEventEnergyDeposit()+myEnergyEDep;
		eventInformation->SetEventEnergyDeposit(sumEnergyDep);

/*
///////////////////// trigger check ////////////////////////////	
//	if(particleName.contains("mu") && myStartVolumeName.contains("World") && myEndVolumeName=="trigPad_physic_up"){ //muon hits trigger paddle
		if((myStartVolumeName.contains("barStack") || myStartVolumeName.contains("World")) //particle started in the stack, or came from the world
			&& myEndVolumeName.contains("barParam") //particle ends up in one of the bar wrapping layers, somewhere, afterwards
			&& particleName.contains("mu")
			&& ((postCopyNo == 2) || (postCopyNo == 5))){ //also, particle entered one of the middle layers
//			&& theStep->GetPostStepPoint()->GetTouchable()->GetVolume()->GetName().back()-48==1){ //particle does so in the middlemost layer in particular
			
		eventInformation->GetMuonTrack(theStep->GetTrack()->GetTrackID())->SetMuonTriggerUp(true);	
		if(eventInformation->GetMuonTrack(theStep->GetTrack()->GetTrackID())->GetMuonTriggerLow()) eventInformation->SetMuonTrigger(true);
	}
	//if(particleName.contains("mu") && myStartVolumeName.contains("World") && myEndVolumeName=="trigPad_physic_low"){ //muon hits trigger paddle
		if((myStartVolumeName.contains("barStack") || myStartVolumeName.contains("World")) //particle started in the stack, or came from the world
			&& myEndVolumeName.contains("barParam") //particle ends up in one of the bar wrapping layers, somewhere, afterwards
			&& particleName.contains("mu")
			&& ((postCopyNo == 0) || (postCopyNo == 3))){ //also, particle entered one of the middle layers
//			&& theStep->GetPostStepPoint()->GetTouchable()->GetVolume()->GetName().back()-48==1){ //particle does so in the middlemost layer in particular
		eventInformation->GetMuonTrack(theStep->GetTrack()->GetTrackID())->SetMuonTriggerLow(true);
		if(eventInformation->GetMuonTrack(theStep->GetTrack()->GetTrackID())->GetMuonTriggerUp()) eventInformation->SetMuonTrigger(true);
	}
///////////////////////////////////////////////////////////////
*/	
/*	
	if(particleName.contains("gamma") && myStartVolumeName=="plScin_physic" && myEndVolumeName.contains("World")){ //gamma exiting scintillator to the world
		eventInformation->GetGammaTrack(theStep->GetTrack()->GetTrackID())->SetGammaOutScintillator(true);
	}
  */
  
}




