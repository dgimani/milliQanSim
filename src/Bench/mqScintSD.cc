/*
 * mqScintSD.cc
 *
 *  Created on: 11.04.2013
 *      Author: heuermann
 */


#include "mqScintSD.hh"
#include "mqScintHit.hh"
#include "mqDetectorConstruction.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include <G4StepPoint.hh>
#include "G4TouchableHistory.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4ios.hh"
#include <fstream>
#include <iostream>
#include "Randomize.hh"

using namespace::std;


mqScintSD::mqScintSD(G4String name)
:G4VSensitiveDetector(name),scintCollection(0), verbose(0)
{
  G4String HCname;
  collectionName.insert(HCname = "scintCollection");

  //HCID= -1;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mqScintSD::~mqScintSD(){
//	if(scintCollection) delete scintCollection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mqScintSD::Initialize(G4HCofThisEvent* HCE)
{
  scintCollection = new mqScintHitsCollection
                          (SensitiveDetectorName,collectionName[0]);


static G4int HCID = -1;
  if(HCID < 0)
  { HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); }
  HCE->AddHitsCollection( HCID, scintCollection );

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool mqScintSD::ProcessHits(G4Step* ,G4TouchableHistory* ){
  return false;
}

G4bool mqScintSD::ProcessHitsEnter(const G4Step* aStep,G4TouchableHistory*)
{
  //store temp variables
  this->SetHitEnergy     (aStep->GetPreStepPoint()->GetTotalEnergy()); // total Energy (KE+Rest mass)
  this->SetHitTime(aStep->GetPreStepPoint()->GetGlobalTime());// Global Time
  this->SetHitPosition(aStep->GetPreStepPoint()->GetPosition()); //position of entry to scintillator
  return true;
}

G4bool mqScintSD::ProcessHitsExit(const G4Step* aStep,G4TouchableHistory*)
{

//  const G4VTouchable* aStep->GetPreStepPoint()->GetTouchable() = aStep->GetPreStepPoint()->GetTouchable();

//  const G4VProcess* process = aStep->GetPostStepPoint()->GetProcessDefinedStep();

  G4int volCopyNo = aStep->GetPreStepPoint()->GetTouchable()->GetCopyNumber(2);
  G4int copyNo=0;
  if(volCopyNo<6){

  //get the detector to retrieve layer information
//  mqDetectorConstruction* detector = (mqDetectorConstruction*)G4RunManager::GetRunManager()
//                                            ->GetUserDetectorConstruction();
  //get number of bars per layer
//  G4int nBarPerLayer = detector->GetNBarPerLayer();

  G4int nBarPerLayer = 2*2*4;

  char layerNumberChar = aStep->GetPreStepPoint()->GetTouchable()->GetVolume(2)->GetName().back();
  G4int layerNumber = layerNumberChar-48;
  G4int subStackCopyNo = aStep->GetPreStepPoint()->GetTouchable()->GetCopyNumber(3);
  copyNo = nBarPerLayer*layerNumber + volCopyNo + 4*subStackCopyNo;
  } else {copyNo = volCopyNo;}

//  G4cout << "Exiting scint! Copy Number is: " << copyNo << G4endl;

//  G4double energyEnter = aStep->GetTotalEnergy();
//  if(edep == 0.) return false;

//  const G4VProcess* creaProc= aStep->GetTrack()->GetCreatorProcess();
//  G4String procName;
//  G4String creaProcName;

//  if (process) procName = process->GetProcessName();
//  else procName = "0";

//  if (creaProc) creaProcName = creaProc->GetProcessName();
//  else creaProcName = "0";

  mqScintHit* hit = new mqScintHit();
//  G4cout << "Initial hit time is: " << this->GetHitTime()/CLHEP::ns << " ns" << G4endl;
//  G4cout << "Initial energy is: " << this->GetHitEnergy()/CLHEP::GeV << " GeV" << G4endl;
  G4ThreeVector pathDiff = this->GetHitPosition()-aStep->GetPostStepPoint()->GetPosition();
//  G4cout << "Path length is: " << std::abs(pathDiff.mag())/CLHEP::cm << " cm" << G4endl;
//  G4cout << "Deposited energy is: " << (this->GetHitEnergy()-aStep->GetPostStepPoint()->GetTotalEnergy())/CLHEP::MeV << " MeV" << G4endl;

	  if(this->GetHitTime()!=-1){
//	  hit = new mqScintHit(); //so create new hit
      //
	  hit->SetTrackID  (aStep->GetTrack()->GetTrackID());          // trackID
	  hit->SetParentID (aStep->GetTrack()->GetParentID());         // parentID
	  // hit->SetVtxPos   (aStep->GetTrack()->GetVertexPosition());
	  //hit->SetProcName (procName);
	  //hit->SetVtxProcName(creaProcName);
	  //hit->SetStripNo(  aStep->GetPreStepPoint()->GetTouchable()->GetReplicaNumber(0) );
	  hit->SetHitEnergy(this->GetHitEnergy()); 
	  hit->SetExitEnergy     (aStep->GetPostStepPoint()->GetTotalEnergy()); // total Energy
	  //hit->SetEdep     (edep);                         // set initial value of energy deposit
	  //hit->SetEvtx     (aStep->GetTrack()->GetVertexKineticEnergy());
	  hit->SetHitTime(this->GetHitTime());
	  hit->SetExitTime(aStep->GetPostStepPoint()->GetGlobalTime());// Global Time
	  //hit->SetLocal    (aStep->GetPreStepPoint()->GetLocalTime());  // Local Time
	  //hit->SetLocalPos (localPointE); // local position
	  //hit->SetMomentum( aStep->GetPreStepPoint()->GetMomentum());
//	  hit->SetParticleName( aStep->GetTrack()->GetDefinition()->GetParticleName() );
	  //hit->SetVtxVolName (aStep->GetPreStepPoint()->GetTouchable()->GetVolume()->GetLogicalVolume()->GetName());
	  //hit->SetProcName (process->GetProcessName());
	  hit->SetHitPosition(this->GetHitPosition());
	  hit->SetExitPosition(aStep->GetPostStepPoint()->GetPosition());
	  //hit->SetPosLocal (aStep->GetPreStepPoint()->GetTouchable()->GetHistory()->GetTopTransform().TransformPoint(pointE));
	  //hit->SetpName (aStep->GetTrack()->GetDefinition()->GetParticleName()); //particle name	 
	  hit->SetCopyNo(copyNo); //energy deposit

	  //hit->SetWeight(aStep->GetTrack()->GetWeight());
	  //if (aStep->GetTrack()->GetDefinition()->GetParticleName().contains("neutron")){
		  //const G4TrackVector *mySecondaries = aStep->GetSecondary();
          //
		  //for (size_t i = 0; i < mySecondaries->size(); i++) {
          //
	  	  //		G4String secParticle =
	  	  //				(*mySecondaries)[i]->GetDefinition()->GetParticleName();
          //
	  	  //		if (secParticle.contains("Ge")) {
	  	  //			G4cout << aStep->GetTrack()->GetDefinition()->GetParticleName() << ": Secondary Ge" << G4endl;
	  	  //			G4cout << "Track ID: " << aStep->GetTrack()->GetTrackID() << G4endl;
	  	  //			G4cout << "Step length: " << aStep->GetStepLength() << G4endl;
	  	  //			G4cout << "Non Ionizing Energy Deposit: " << aStep->GetNonIonizingEnergyDeposit() << G4endl;
	  	  //			G4cout << "Ge Track ID: " << (*mySecondaries)[i]->GetTrackID() << G4endl;
	  	  //		}
          //
	  	  //}
	  scintCollection->insert(hit);
	this->SetHitPosition(G4ThreeVector());
	this->SetHitTime(-1);
	this->SetHitEnergy(0);
	}
  return true;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mqScintSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verbose>0) {

     G4int NbHits = scintCollection->entries();
     G4cout << "\n-------->Hits Collection: in this event they are " << NbHits
            << " hits in the scint : " << G4endl;
     for (G4int i=0;i<NbHits;i++){
    	 //if ((*scintCollection)[i]->GetCopyNo() > 0)
    	 (*scintCollection)[i]->Print();
     }

   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

