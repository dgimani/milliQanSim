/*
 * mqScintSD.cc
 *
 *  Created on: 18.04.2017
 *      Author: schmitz
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


//  G4int copyNo=volCopyNo;

  G4int volCopyNo = aStep->GetPostStepPoint()->GetTouchable()->GetCopyNumber(2);
  G4int copyNo=0;
  if(volCopyNo<6){

  //get the detector to retrieve layer information
//  mqDetectorConstruction* detector = (mqDetectorConstruction*)G4RunManager::GetRunManager()
//                                            ->GetUserDetectorConstruction();
  //get number of bars per layer
//  G4int nBarPerLayer = detector->GetNBarPerLayer();

  G4int nBarPerLayer = 9*6*4;

  char layerNumberChar = aStep->GetPostStepPoint()->GetTouchable()->GetVolume(2)->GetName().back();
  G4int layerNumber = layerNumberChar-48;
  G4int subStackCopyNo = aStep->GetPostStepPoint()->GetTouchable()->GetCopyNumber(4);
  
  copyNo = nBarPerLayer*layerNumber + volCopyNo + 4*subStackCopyNo;
  } else {copyNo = volCopyNo;}

  G4cout << "Exiting scint! Copy Number is: " << copyNo << G4endl;





  const G4VProcess* creaProc= aStep->GetTrack()->GetCreatorProcess();
  G4String creaProcName;
  if (creaProc) creaProcName = creaProc->GetProcessName();
  else creaProcName = "0";

  G4String vertexVolume = aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName();
//  G4cout << vertexVolume << " " << G4endl;
  G4double edep;
  if(aStep->GetTrack()->GetDefinition()->GetParticleName().contains("e+") || aStep->GetTrack()->GetDefinition()->GetParticleName().contains("e-")
      || aStep->GetTrack()->GetDefinition()->GetParticleName().contains("mu")
      || aStep->GetTrack()->GetDefinition()->GetParticleName().contains("tau")
   ){ edep = aStep->GetPostStepPoint()->GetTotalEnergy(); }
  else edep = aStep->GetPostStepPoint()->GetKineticEnergy();
  mqScintHit* hit = new mqScintHit();
	  hit->SetTrackID  (aStep->GetTrack()->GetTrackID());          // trackID
	  hit->SetParentID (aStep->GetTrack()->GetParentID());         // parentID
	  hit->SetHitEnergy(edep); 
	  hit->SetHitTime(aStep->GetPostStepPoint()->GetGlobalTime());
	  hit->SetHitPosition(aStep->GetPostStepPoint()->GetPosition());
	  //hit->SetParticleName( aStep->GetTrack()->GetDefinition()->GetParticleName() );
	  hit->SetParticleName( aStep->GetTrack()->GetDefinition()->GetPDGEncoding() );
	  hit->SetCopyNo(copyNo); //energy deposit
	  hit->SetProcName(creaProcName); //creation process of the particle that caused the hit
	  hit->SetCreatorVolName(vertexVolume); //creation volume of the particle that caused the hit
	  scintCollection->insert(hit); 
  return true;
}

G4bool mqScintSD::ProcessHitsExit(const G4Step* aStep,G4TouchableHistory*)
{

  G4int volCopyNo = aStep->GetPreStepPoint()->GetTouchable()->GetCopyNumber(2);
  G4int copyNo=0;
  if(volCopyNo<6){

  //get the detector to retrieve layer information
//  mqDetectorConstruction* detector = (mqDetectorConstruction*)G4RunManager::GetRunManager()
//                                            ->GetUserDetectorConstruction();
  //get number of bars per layer
//  G4int nBarPerLayer = detector->GetNBarPerLayer();

  G4int nBarPerLayer = 9*6*4;

  char layerNumberChar = aStep->GetPreStepPoint()->GetTouchable()->GetVolume(2)->GetName().back();
  G4int layerNumber = layerNumberChar-48;
  G4int subStackCopyNo = aStep->GetPreStepPoint()->GetTouchable()->GetCopyNumber(4);
  copyNo = nBarPerLayer*layerNumber + volCopyNo + 4*subStackCopyNo;
  } else {copyNo = volCopyNo;}

  G4cout << "Exiting scint! Copy Number is: " << copyNo << G4endl;

  const G4VProcess* creaProc= aStep->GetTrack()->GetCreatorProcess();
  G4String creaProcName;

  if (creaProc) creaProcName = creaProc->GetProcessName();
  else creaProcName = "0";

  G4String vertexVolume = aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName();
//  G4cout << vertexVolume << " " << G4endl;
  G4double edep;
  if(aStep->GetTrack()->GetDefinition()->GetParticleName().contains("e+") || aStep->GetTrack()->GetDefinition()->GetParticleName().contains("e-")
      || aStep->GetTrack()->GetDefinition()->GetParticleName().contains("mu")
      || aStep->GetTrack()->GetDefinition()->GetParticleName().contains("tau")
   ){ edep = aStep->GetPreStepPoint()->GetTotalEnergy(); }
  else edep = aStep->GetPreStepPoint()->GetKineticEnergy();
  mqScintHit* hit = new mqScintHit();
	  hit->SetTrackID  (aStep->GetTrack()->GetTrackID());          // trackID
	  hit->SetParentID (aStep->GetTrack()->GetParentID());         // parentID
	  hit->SetExitEnergy     (edep); // total Energy
	  hit->SetExitTime(aStep->GetPreStepPoint()->GetGlobalTime());// Global Time
	  hit->SetExitPosition(aStep->GetPreStepPoint()->GetPosition());
	  //hit->SetParticleName( aStep->GetTrack()->GetDefinition()->GetParticleName() );
	  hit->SetParticleName( aStep->GetTrack()->GetDefinition()->GetPDGEncoding() );
	  hit->SetCopyNo(copyNo); //energy deposit
	  hit->SetProcName(creaProcName); //creation process of the particle that caused the hit
	  hit->SetCreatorVolName(vertexVolume); //creation volume of the particle that caused the hit
	  scintCollection->insert(hit);
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

