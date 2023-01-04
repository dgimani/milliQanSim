/*
 * mqScintRHit.cc
 *
 *  Created on: 21.04.2019
 *      Author: schmitz
 */

#include "mqScintRHit.hh"

#include <iomanip>

ClassImp(mqScintRHit)
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mqScintRHit::mqScintRHit():
	trackID(-1), parentID(-1),
	EDep_MeV(0.), trackLength_cm(0.),
	copyNo(0),
	hitTime_ns(0.), exitTime_ns(0.)
	//EDepRecoil_keV(0.),
//	particleName(""),
//	hitPositionX_cm(0.),hitPositionY_cm(0.),hitPositionZ_cm(0.)

{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mqScintRHit::~mqScintRHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mqScintRHit::mqScintRHit(const mqScintRHit& right):
		TObject()// inherit from TObject?
{
    trackID         = right.trackID        ;
    parentID        = right.parentID       ;
    EDep_MeV        = right.EDep_MeV          ;
    trackLength_cm        = right.trackLength_cm          ;
    copyNo          = right.copyNo;
//    particleName    = right.particleName   ;
    hitTime_ns  = right.hitTime_ns;
    exitTime_ns  = right.exitTime_ns;
//    hitPositionX_cm    = right.hitPositionX_cm   ;
//    hitPositionY_cm    = right.hitPositionY_cm   ;
//    hitPositionZ_cm    = right.hitPositionZ_cm   ;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const mqScintRHit& mqScintRHit::operator=(const mqScintRHit& right)
{
    trackID         = right.trackID        ;
    parentID        = right.parentID       ;
    EDep_MeV        = right.EDep_MeV          ;
    trackLength_cm        = right.trackLength_cm          ;
    copyNo          = right.copyNo;
//    particleName    = right.particleName   ;
    hitTime_ns  = right.hitTime_ns;
    exitTime_ns  = right.exitTime_ns;

    return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Int_t mqScintRHit::operator==(const mqScintRHit& right) const
{
  return (this==&right) ? 1 : 0;
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......




