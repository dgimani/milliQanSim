#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"

void rawAreaPlot(){

  TChain ch("Events");
   ch.Add("/media/ryan/Storage/computing/mqTestStand/data/Test/MilliQan.root");

  TH1F *hMEDep = new TH1F("hMECount","PMT Signal Area, CloseMid",100,0,50000);
  hMECount->GetXaxis()->SetTitle("Pulse Area (nVs");
  hMECount->GetYaxis()->SetTitle("Bin Members");

  TCanvas *c1 = new TCanvas("c1","c1",0,400,600,300);
  ch.Draw("area_CH1>>hMECount");
  
}
