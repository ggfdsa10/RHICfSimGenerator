#include <iostream>
#include <iomanip>
#include "analysis.h"
#include "histogram.hh"

namespace io = boost::iostreams;

int main (int argc, char **argv)
{
  if (argc==1) {
    std::cout << "\n Please, provide input data file(s) in hepmc or hepmcgz format\n" << std::endl;
    return 0;
  }
  
  //------------------SET UP HISTOGRAMS
  nd_histogram::axis<double>        ax(21,-10,10);
  nd_histogram::histogram<double,1> exampleHist(ax);

  //-------------------SET UP DATA
  std::vector<std::string> files;
  for (int ia=1; ia<argc; ++ia) files.push_back(argv[ia]);
  
  DataManager data;
  //for more models, loop over models and call SetFiles each time
  data.SetFiles (files); 


  //-------------------EVENT LOOP
  int nEvts = 0;
  while (data.GetNextEvent ()) {
    ++nEvts;
    
    //-------------------PARTICLE LOOP
    for (auto& p : data.evt.particles()) {
      // get final state particles. status == 2 are decayed particles,
      // status == 4 is beam particles
      if (p->status () != 1) continue; 

      const double eta = p->momentum ().eta ();
      //auto  parent_vertex = p->production_vertex();
      //const int    id     = p->pdg_id ();
      //const double pt     = p->momentum ().perp ();
      //const double e      = p->momentum ().e ();

      //for more advance paramters see HepMC documentation

      //-------------------EVENT SELECTION
      //-------------------FILL HISTOGRAMS WITH PER PARTICLE VARIABLES
      exampleHist.fill (eta);
    }//PARTICLE LOOP
    //-------------------FILL HISTOGRAMS WITH PER EVENT VARIABLES
  }//EVENT LOOP
  //---------------FINALISE HISTOGRAMS
  nd_histogram::ascii_plotter<double,1>::plot<>(std::cout,exampleHist);
  
  return 0;
}
