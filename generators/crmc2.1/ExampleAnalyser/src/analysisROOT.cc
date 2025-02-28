#include <iostream>
#include <string>
#include <TLorentzVector.h>
#include <TFile.h>
#include <TChain.h>
#include <TH1D.h>

using namespace std;


int main (int argc, char **argv)
{
  if (argc==1) {
    cout << "\n Please, provide input data file(s) in ROOT format\n" << endl;
    return 0;
  }
  
  //-------------------SET UP DATA
  TFile* theOutFile;
  string outFileName ("new_histogram_file_ROOT.root");
  cout << " ! Opening output file: " << outFileName << endl;
  theOutFile = new TFile (outFileName.c_str(), "RECREATE");

  TChain data("Particle");
  for (int ia=1; ia<argc; ++ia) {
    data.AddFile(argv[ia]);
  }

  int nPart;
  data.SetBranchAddress("nPart", &nPart);

  const int MAXP = 100000;
  int pdgid[MAXP];
  int status[MAXP];
  double px[MAXP];
  double py[MAXP];
  double pz[MAXP];
  double E[MAXP];
  double m[MAXP];
  data.SetBranchAddress("pdgid", pdgid);
  data.SetBranchAddress("status", status);
  data.SetBranchAddress("px", px);
  data.SetBranchAddress("py", py);
  data.SetBranchAddress("pz", pz);
  data.SetBranchAddress("E", E);
  data.SetBranchAddress("m", m);
  
  theOutFile->mkdir ("model1");

  //------------------SET UP HISTOGRAMS
  TH1D* exampleHist = new TH1D ("dNdeta",";#eta;dN/d#eta",21,-10,10);


  //-------------------EVENT LOOP
  int nEvts = data.GetEntries();
  for (int iEvt=0; iEvt<nEvts; ++iEvt)
    {
      data.GetEntry(iEvt);
      
      //-------------------PARTICLE LOOP
      for (int ipar=0; ipar < nPart; ++ipar)
	{
	  if (status[ipar] != 1) continue; //get final state particles. status == 2 are decayed particles, status == 4 is beam particles

	  TLorentzVector lv(px[ipar],py[ipar],pz[ipar],E[ipar]);
	  
	  //HepMC3::GenVertex* parent_vertex = p->production_vertex();
	  const int id = pdgid[ipar];
	  const double eta = lv.Eta();
	  //const double pt = p->momentum ().perp ();
	  //const double e = p->momentum ().e ();
	  
          //for more advance paramters see HepMC documentation or load #include <TParticle.h> and fill object (see analysis.h)

          //-------------------EVENT SELECTION
          //-------------------FILL HISTOGRAMS WITH PER PARTICLE VARIABLES
          exampleHist->Fill (eta);
	}//PARTICLE LOOP
      //-------------------FILL HISTOGRAMS WITH PER EVENT VARIABLES
    }//EVENT LOOP
  //---------------FINALISE HISTOGRAMS
  exampleHist->Scale (1. / nEvts, "width");

  //----------------Closing Files
  std::cout << " ! Writing output file: " << outFileName << std::endl;
  theOutFile->Write();
  delete exampleHist;
  exampleHist = 0;
  std::cout << " ! Closing output file: " << outFileName << std::endl;
  theOutFile->Close();
  delete theOutFile;
  theOutFile = 0;
  return 0;
}
