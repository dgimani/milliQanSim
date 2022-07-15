//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file mqBarParameterisation.cc
/// \brief Implementation of the mqBarParameterisation class

#include "mqBarParameterisation.hh"

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"
#include <math.h>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mqBarParameterisation::mqBarParameterisation(  
        G4int    nBarX,
        G4int    nBarY,
        G4double startZ,          //  Z of center of grid. Always set to zero for now
        G4double angleDelta,        //  angular separation between bars
	G4double sourceDist,      //distance to interaction point
	G4double halfWidthBar,       //bars are square in cross-section, this gives length
        G4double halfLengthBar)      //bar length
 : G4VPVParameterisation()
{
   fnBars =  nBarX*nBarY;
   fnBarsX = nBarX;
   fnBarsY = nBarY;
   fStartZ     =  startZ; 
   fAngleDelta    =  angleDelta;
   fSourceDist = sourceDist;
   fHalfWidth  =  halfWidthBar; //barSpacingXY
   fHalfLength =  halfLengthBar; //rotAng
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mqBarParameterisation::~mqBarParameterisation()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mqBarParameterisation::ComputeTransformation
(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
  G4double barSpacingXY = fHalfWidth;
  G4double rotAng = fHalfLength; //bad variable names, repurposing these
 
  //calculate offset so stack is flush with cavern floor
  G4double offset = (copyNo % fnBarsX)*barSpacingXY/std::tan(rotAng)-(fnBarsX-1)*barSpacingXY/std::tan(rotAng)/2;

	// Note: copyNo will start with zero!
  G4double xAngle = (copyNo % fnBarsX)*fAngleDelta - (fnBarsX-1)*fAngleDelta/2;
  G4double yAngle = (copyNo/fnBarsX)*fAngleDelta  - (fnBarsY-1)*fAngleDelta/2;

  //calculate x,y pos (taking slight z offset into account and adjusting per-bar)
  G4double Xposition = (fSourceDist+offset)*std::tan(xAngle);
  G4double Yposition = (fSourceDist+offset)*std::tan(yAngle);
//  G4double Yposition = fSourceDist*std::cos(xAngle)*std::sin(yAngle);
//  G4double Zposition = fStartZ+fSourceDist*(std::cos(xAngle)*std::cos(yAngle)-1)
  G4double Zposition = fStartZ;//+(copyNo % fnBarsX)*barSpacingXY/std::tan(rotAng); // gives offset look

  //G4double Yposition = ((copyNo/fnBarsX) % fnBarsX)*fXYSpacing   - (fnBarsX-1)*fXYSpacing/2;
//  G4int nBarsPerLayer = fnBarsX*fnBarsY;
  //G4double Zposition = fStartZ+(copyNo/nBarsPerLayer)*fZSpacing - (fnLayers-1)*fZSpacing/2;

  std::shared_ptr<G4RotationMatrix> rot = std::make_shared<G4RotationMatrix>();
  //G4RotationMatrix *rot = new G4RotationMatrix();
  
    rot->rotateX(yAngle);
    rot->rotateY(-xAngle);

  G4ThreeVector origin(Xposition,Yposition,Zposition);
  physVol->SetRotation(rot.get());
  //physVol->SetRotation(rot);
  physVol->SetTranslation(origin);
  //delete rot;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mqBarParameterisation::ComputeDimensions
(G4Box& mqBar, const G4int copyNo, const G4VPhysicalVolume*) const
{
  // Note: copyNo will start with zero! and for now, we don't change our geom size for each copy
  //mqBar.SetXHalfLength(fHalfWidth);
  //mqBar.SetYHalfLength(fHalfWidth);
  //mqBar.SetZHalfLength(fHalfLength);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
