#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"

void MuonPMTHit(){

  TChain ch("Events");
   ch.Add("/media/ryan/Storage/computing/mqTestStand/data/Test/MilliQan.root");

  TH1F *hMEDep = new TH1F("hMECount","Photoelectrons Detected Per Incident Muon, Test",100,1000,1500);
  hMECount->GetXaxis()->SetTitle("Number of Photoelectrons Detected");
  hMECount->GetYaxis()->SetTitle("Bin Members");

  TCanvas *c1 = new TCanvas("c1","c1",0,400,600,300);
  ch.Draw("NbOfPMTHits>>hMECount");
  
}
