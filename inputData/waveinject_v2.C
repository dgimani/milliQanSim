#include "TCanvas.h"
#include "TTree.h"
#include "TGaxis.h"
#include "TStyle.h"
#include "TFile.h"
#include <iostream>
#include <fstream>
#include "TMath.h"
#include "TChain.h"
#include "/homes/dgimani/milliQanSim/include/mqROOTEvent.hh"
#include "/homes/dgimani/milliQanSim/include/mqPMTRHit.hh"
#include "TGraph.h"
#include "TVector.h"
#include "TVectorD.h"
#include "TVectorF.h"
#include "TH1.h"
#include "TH1F.h"
#include "TString.h"
#include "TChain.h"
#include "TMultiGraph.h"
#include <vector>
#include <map>
#include <algorithm>
R__LOAD_LIBRARY(/homes/dgimani/milliQanSim/build/libMilliQanCore.so)
using namespace std;

int simToDataPMT(int simChannel) {
    if (simChannel == 77) return 68;
    else if (simChannel == 78) return 70;
    else if (simChannel == 79) return 69;
    else if (simChannel == 81) return 72;
    else if (simChannel == 82) return 74;
    else if (simChannel == 83) return 73;
    else if (simChannel == 97) return 71;
    else if (simChannel == 96) return 75;

    int layerNumber = simChannel / 216;
    simChannel = simChannel % 216;

    if (simChannel <= 4) {
        return (simChannel + 11) + layerNumber * 16;
    } else if (simChannel <= 12) {
        return simChannel - 1 + layerNumber * 16;
    } else if (simChannel <= 16) {
        return simChannel - 13 + layerNumber * 16;
    } else {
        std::cerr << "Error: simChannel out of range" << std::endl;
        return -1;
    }
}

void waveinject_v2(TString input, TString output) {
   TChain rootEvents("Events");
   //rootEvents.Add("MilliQan_cosmicSimSample.root");
   rootEvents.Add(input);
   mqROOTEvent* myROOTEvent = new mqROOTEvent();
   rootEvents.SetBranchAddress("ROOTEvent", &myROOTEvent);
   //TFile* outfile = new TFile("MilliQan_waveinjected_v2.root", "RECREATE");
   TFile* outfile = new TFile(output, "RECREATE");
   
   const int nDigitizers = 5;
   const int nChannelsPerDigitizer = 16;
   const int nBins = 1024;
   double binWidth = 2.5;
   double rms_noise = 1;

   Float_t waveform[nDigitizers][nChannelsPerDigitizer][nBins] = {{{0}}};

   TTree* injectedTree = new TTree("Events", "Tree with digitizer waveform data");
   injectedTree->Branch("waveform", waveform, Form("waveform[%d][%d][%d]/F", nDigitizers, nChannelsPerDigitizer, nBins));

   TF1 *fit = new TF1("fit", "gaus(0)", 0, 5000);
   fit->SetParameter(0, 7.23967e-02);
   fit->SetParameter(1, 1.48539e+03);
   fit->SetParameter(2, 2.90976e+02);

   TFile* f = new TFile("modified_waveform.root");
   TH1F* pulse_shape = (TH1F*)f->Get("average_waveform");

   TRandom3 randGen(0);
   Long64_t nentries = rootEvents.GetEntries();
   //std::cout << "Entries: " << nentries << std::endl;

   for (Long64_t i = 0; i < nentries; i++) {
      if (i % (nentries / 100) == 0) //std::cout << "Processing Event " << i << "..." << std::endl;
      rootEvents.GetEntry(i);
      memset(waveform, 0, sizeof(waveform));

      std::map<int, std::vector<mqPMTRHit*>> pmtHitsMap;

      for (int j = 0; j < myROOTEvent->GetPMTRHits()->size(); j++) {
         mqPMTRHit* PMTRHit = myROOTEvent->GetPMTRHits()->at(j);
         int PMT_number = PMTRHit->GetPMTNumber();
         pmtHitsMap[PMT_number].push_back(PMTRHit);
      }

      for (const auto& [PMT_number, hits] : pmtHitsMap) {
         int remappedPMT = simToDataPMT(PMT_number);
         if (remappedPMT == -1) continue;

         int digitizer = remappedPMT / nChannelsPerDigitizer;
         int channel = remappedPMT % nChannelsPerDigitizer;

         // Extract and sort hit times
         std::vector<double> hitTimes;
         for (const auto& hit : hits) {
            if(hit->GetFirstHitTime() > 500.0 ) continue;
	    hitTimes.push_back(hit->GetFirstHitTime());
         }
	 if(hitTimes.size()==0) continue;
         std::sort(hitTimes.begin(), hitTimes.end());

         // Calculate median hit time
         double median_hit_time;
         size_t size = hitTimes.size();
         if (size % 2 == 0) {
            median_hit_time = 0.5 * (hitTimes[size / 2 - 1] + hitTimes[size / 2]);
         } else {
            median_hit_time = hitTimes[size / 2];
         }
         if (hits.size() > 50) {
            double areaSum = 0.0;
            for (size_t k = 0; k < hits.size(); ++k) {
                areaSum += fit->GetRandom();
            }

            TH1F* new_waveform = (TH1F*)pulse_shape->Clone();
            //new_waveform->Scale(areaSum * (1077.24 / 828.03) / new_waveform->Integral(480, 640));
            
	    // Scale only the bins within [500, 660]
            double scaleFactor = areaSum * (1077.24 / 828.03) / new_waveform->Integral(480, 640);
            for (int bin = 500; bin <= 660; ++bin) {
                double binContent = new_waveform->GetBinContent(bin);
                new_waveform->SetBinContent(bin, binContent * scaleFactor);
            }

            int integer_shift = static_cast<int>(median_hit_time / binWidth);
            double fractional_shift = fmod(median_hit_time, binWidth) / binWidth;

            for (int bin = 0; bin < nBins; ++bin) waveform[digitizer][channel][bin] = 0.0;

            for (int bin = 0; bin < nBins; ++bin) {
                int shifted_bin = bin + integer_shift;
                if (shifted_bin >= nBins - 1 || shifted_bin < 0) continue;

                double value = (1.0 - fractional_shift) * new_waveform->GetBinContent(shifted_bin + 1) +
                               fractional_shift * new_waveform->GetBinContent(shifted_bin + 2);
                double noise = randGen.Gaus(0, rms_noise);
                waveform[digitizer][channel][shifted_bin] += (value + noise);

                if (waveform[digitizer][channel][shifted_bin] > 1250) waveform[digitizer][channel][shifted_bin] = 1250;
                if (waveform[digitizer][channel][shifted_bin] < -50) waveform[digitizer][channel][shifted_bin] = -50;
            }

            for (int bin = 0; bin < integer_shift; ++bin) waveform[digitizer][channel][bin] = 0.0;

            delete new_waveform;
         } else {
            for (mqPMTRHit* PMTRHit : hits) {
               double initial_hit_time = PMTRHit->GetFirstHitTime();
               //cout << initial_hit_time << endl;
	       if(initial_hit_time>500) {continue;}
               TH1F* new_waveform = (TH1F*)pulse_shape->Clone();
               double event_area = fit->GetRandom();

               //new_waveform->Scale(event_area * (1077.24 / 828.03) / new_waveform->Integral(480, 640));
	       
	       // Scale only the bins within [500, 660]
               double scaleFactor = event_area * (1077.24 / 828.03) / new_waveform->Integral(480, 640);
               for (int bin = 500; bin <= 660; ++bin) {
                   double binContent = new_waveform->GetBinContent(bin);
                   new_waveform->SetBinContent(bin, binContent * scaleFactor);
               }

               int integer_shift = static_cast<int>(initial_hit_time / binWidth);
               //cout << "integer shift: " << integer_shift << endl;
	       double fractional_shift = fmod(initial_hit_time, binWidth) / binWidth;

               //for (int bin = 0; bin < nBins; ++bin) waveform[digitizer][channel][bin] = 0.0;

               for (int bin = 0; bin < nBins; ++bin) {
                  int shifted_bin = bin + integer_shift;
                  if (shifted_bin >= nBins - 1 || shifted_bin < 0) continue;

                  double value = (1.0 - fractional_shift) * new_waveform->GetBinContent(shifted_bin + 1) +
                                 fractional_shift * new_waveform->GetBinContent(shifted_bin + 2);
		  waveform[digitizer][channel][bin] += (value);

               }

               for (int bin = 0; bin < integer_shift; ++bin) waveform[digitizer][channel][bin] = 0.0;

               delete new_waveform;
            }
                  for (int bin = 0; bin < nBins; bin++) {
                        double noise = randGen.Gaus(0, rms_noise);
			waveform[digitizer][channel][bin] += noise;
                  	if (waveform[digitizer][channel][bin] > 1250) waveform[digitizer][channel][bin] = 1250;
                  	if (waveform[digitizer][channel][bin] < -50) waveform[digitizer][channel][bin] = -50;
		  }

	   }
      }
      injectedTree->Fill();
   }

      cout << "final" << endl;
   outfile->cd();
   injectedTree->Write();
   f->Close();
   outfile->Close();
}

