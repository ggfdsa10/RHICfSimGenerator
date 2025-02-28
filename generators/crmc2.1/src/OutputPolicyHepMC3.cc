#include <OutputPolicyHepMC3.h>

#include <CRMCoptions.h>
#include <CRMCinterface.h>
#include <CRMCconfig.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace io = boost::iostreams;

//--------------------------------------------------------------------
void OutputPolicyHepMC3::InitOutput(const CRMCoptions& cfg)
{
    fOut = new io::filtering_ostream();
    if(cfg.GetOutputMode()==CRMCoptions::eHepMC3GZ){
        fOut->push(io::gzip_compressor(io::zlib::best_compression));
    }
    fOut->push(io::file_descriptor_sink(cfg.GetOutputFileName()), std::ios_base::trunc);

    _writer = new HepMC3::WriterAscii(*fOut);
    _hepmc3.init(cfg);


    // for RHICf Simulation
    mTmpEventNumber = 0;
    int rhicfRunType = -1;
    if(cfg.fRHICfRunType == "TL"){rhicfRunType = 0;}
    if(cfg.fRHICfRunType == "TS"){rhicfRunType = 1;}
    if(cfg.fRHICfRunType == "TOP"){rhicfRunType = 2;}
    TString tmpOption = cfg.fSaveOption;
    tmpOption.ToUpper();

    int tmpModelIdx = -1;
    if(cfg.fModelIdx == 0){tmpModelIdx = 2;} // EPOS-LHC
    else if(cfg.fModelIdx == 7){tmpModelIdx = 3;} // QGSJETII-04
    else if(cfg.fModelIdx == 13){tmpModelIdx = 4;} // QGSJETIII-01
    else{tmpModelIdx = -1;}

    mSimManager = new RHICfSimManager();
    mSimManager -> SetRHICfRunType(rhicfRunType);
    mSimManager -> SetSimModel(tmpModelIdx); 
    mSimManager -> SetFilePath(cfg.fDataSavePath);

    mIsSavedNeutron = false;
    if(tmpOption == "N" || tmpOption == "NEUTRON"){
        mIsSavedNeutron = true;
        mSimManager -> SavePi0AndNeutron();
    }

    mSimManager -> Init();
    mParticleArr = mSimManager -> GetParticleArr();
}

//--------------------------------------------------------------------
void OutputPolicyHepMC3::FillEvent(const CRMCoptions& cfg, const int nEvent)
{
    if(!_writer){
        throw std::runtime_error("!!!No HepMC3 ASCII writer defined!");
    }
    if(!_hepevt.convert(_event)){
        throw std::runtime_error("!!!Could not read next event");
    }
    if(!cfg.IsTest()){
        _hepmc3.fillInEvent(cfg, nEvent, _event);
        // _writer->write_event(_event);
    }


    // RHICfSimManager 
    mSimManager -> Clear();
    mRHICfGammaIdx.clear();
    mIsPi0Event = false;
    mIsNeuEvent = false;

    mTmpProcessId = 0;
    switch (gCRMC_data.typevt) // if negative typevt mini plasma was created by event (except -4)
    {
        case  0: break; //unknown for qgsjetII
        case  1: mTmpProcessId = 101; break; // minbias
        case -1: mTmpProcessId = 101; break; // minbias
        case  2: mTmpProcessId = 105; break; // double diff
        case -2: mTmpProcessId = 105; break; // double diff
        case  3: mTmpProcessId = 106; break; // central diff
        case -3: mTmpProcessId = 106; break; // central diff
        case  4: mTmpProcessId = 103; break; // left single diff
        case -4: mTmpProcessId = 104; break; // right single diff
        default: cerr << "process ID not recognised for setting HEPEVT" << endl;
    }

    // random vertex for STAR
    double collisionVtxX = 0.; // [mm]
    double collisionVtxY = 0.; // [mm]
    double collisionVtxZ = 0.; // [mm]
    mSimManager -> GetVertexRandom(collisionVtxX, collisionVtxY, collisionVtxZ);

    mSimManager -> SetEventInfo(mTmpProcessId, -999., -999., -999.);


    int parNum = _event.particles_size();
    for(int par=0; par<parNum; par++) {
        auto p = (_event.particles())[par];

        int stat = p -> status();
        int id = p -> id();
        int pid = p -> pdg_id();
        double eta = p -> momentum().pseudoRapidity();
        double vx = p -> production_vertex()->position().x() + collisionVtxX; // [mm]
        double vy = p -> production_vertex()->position().y() + collisionVtxY; // [mm]
        double vz = p -> production_vertex()->position().z() + collisionVtxZ; // [mm]
        double t = p -> production_vertex()->position().t(); // [mm/c]
        double px = p -> momentum().px();
        double py = p -> momentum().py();
        double pz = p -> momentum().pz();
        double e = p -> momentum().e();
        double mass = p -> generated_mass();

        int parentSize = p -> parents().size();
        int daughterSize = p -> children().size();

        int parentIdx1 = -1;
        int parentIdx2 = -1;
        int daughterIdx1 = -1;
        int daughterIdx2 = -1;

        if(parentSize == 1){
            parentIdx1 = p -> parents()[0] -> id();
            parentIdx2 = 0;
        }
        if(stat == 2){
            daughterIdx1 = p -> children()[0] -> id();
            if(daughterSize == 2){daughterIdx2 = p -> children()[1] -> id();}
        }

        mParticle = (TParticle*)mParticleArr -> ConstructedAt(par);
        mParticle -> SetPdgCode(pid);
        mParticle -> SetStatusCode(stat);
        mParticle -> SetProductionVertex(vx, vy, vz, t); // [mm, mm, mm, mm/c]
        mParticle -> SetMomentum(px, py, pz, e); // [GeV/c]
        mParticle -> SetCalcMass(mass); // [GeV/c^2]
        mParticle -> SetFirstMother(parentIdx1);
        mParticle -> SetLastMother(parentIdx2);
        mParticle -> SetFirstDaughter(daughterIdx1);
        mParticle -> SetLastDaughter(daughterIdx2);

        // Find a RHICf hit particles
        if(stat == 1){ // final state particle 
            int hit = mSimManager -> GetRHICfGeoHit(vx, vy, vz, px, py, pz, e);
            if(hit < 0){continue;}

            if(pid == 22){ // final state gamma and neutron
                mRHICfGammaIdx.push_back(par);
            }
            if(pid == 2112){
                mIsNeuEvent = true;
            }
        }
    }

    // Find a RHICf pi0 events
    int RHICfgammaNum = mRHICfGammaIdx.size();
    for(int i=0; i<RHICfgammaNum; i++){
        int gamma1Idx = mRHICfGammaIdx[i];
        mParticle = (TParticle*)mParticleArr -> At(gamma1Idx);

        int gamma1MotherIdx = mParticle -> GetFirstMother();
        mParticle = (TParticle*)mParticleArr -> At(gamma1MotherIdx-1);
        int gamma1MotherPID = mParticle -> GetPdgCode();

        for(int j=i+1; j<RHICfgammaNum; j++){
            int gamma2Idx = mRHICfGammaIdx[j];
            mParticle = (TParticle*)mParticleArr -> At(gamma2Idx);

            int gamma2MotherIdx = mParticle -> GetFirstMother();
            mParticle = (TParticle*)mParticleArr -> At(gamma2MotherIdx-1);
            int gamma2MotherPID = mParticle -> GetPdgCode();

            if(gamma2MotherPID != 111){continue;}
            if(gamma1MotherIdx == gamma2MotherIdx){
                mIsPi0Event = true;
            }
        }
    }
      
    if(mIsSavedNeutron){
        if(mIsPi0Event || mIsNeuEvent){
            mSimManager -> Fill();
            mTmpEventNumber++;
            EventPrint();

            return;
        }
    }
    else{
        if(mIsPi0Event){
            mSimManager -> Fill();
            mTmpEventNumber++;
            EventPrint();

            return;
        }
    }

}

//--------------------------------------------------------------------
void OutputPolicyHepMC3::CloseOutput(const CRMCoptions&)
{
  if (_writer) {
    delete _writer;
    delete fOut;
  }
  _writer = 0;
  fOut = 0;

  mSimManager -> Close();
}

void OutputPolicyHepMC3::EventPrint()
{
    cout << "===================== OutputPolicyHepMC3::EventPrint() ===================== " << endl;
    cout << " Saved Event Number       : " << mTmpEventNumber << endl;
    cout << " Event Process Id         : " << mTmpProcessId  << endl;
    cout << " Total Particle Number    : " << mParticleArr -> GetEntries() << endl;
    cout << " RHICf Hitted Particle    : " << mRHICfGammaIdx.size()+int(mIsNeuEvent) << endl;
    cout << " RHICf Pi0 Event          : " << TString((mIsPi0Event == true)? "TRUE" : "FALSE") << endl; 
    cout << " RHICf Neutron Event      : " << TString((mIsNeuEvent == true)? "TRUE" : "FALSE") << endl; 
    cout << "======================================================================== " << endl;
}

//--------------------------------------------------------------------
void OutputPolicyHepMC3::PrintTestEvent(const CRMCoptions& cfg)
{
}

//--------------------------------------------------------------------
//
// EOF
//
