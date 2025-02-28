#ifndef RHICfSimManager_H
#define RHICfSimManager_H

#include <iostream> 
#include <fstream>
#include <string>

#include "TROOT.h"
#include "TSystem.h"
#include "TSystemFile.h"
#include "TSystemDirectory.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TParticle.h"
#include "TH2Poly.h"
#include "TRandom3.h"

#include "TMath.h"
#include "TString.h"

#include "Parameters.hh"

using namespace std;
using namespace SimPars;

class RHICfSimManager
{
    public:
        RHICfSimManager();
        ~RHICfSimManager();

        void Init();

        void SavePi0AndNeutron(){mSavePi0Neutron = true;}

        void SetSimModel(int model);
        void SetFilePath(TString path);
        void SetRHICfRunType(int type);

        void Clear();
        void SetEventInfo(int processId, double sHat, double tHat, double uHat);
        TClonesArray* GetParticleArr(){return mParticleArr;}
        void Fill();
        void Close();

        Int_t GetRHICfGeoHit(double posX, double posY, double posZ, double px, double py, double pz, double e);

        void GetVertexRandom(double& vx, double& vy, double& vz);

    private:
        void InitWrite();
        void InitRHICfGeometry();

        Int_t mModel;
        Int_t mRHICfRunType;
        Bool_t mSavePi0Neutron;

        TString mFilePath;
        TString mFileName;

        TFile* mFile;
        TTree* mTree;
        TClonesArray* mParticleArr;
        Int_t mEventProcess;
        Double_t mEvent_sHat;
        Double_t mEvent_tHat;
        Double_t mEvent_uHat;

        TH2Poly* mRHICfPoly;

        TRandom3* mRndEngine;
        double vertexXSigma = 1.; // [mm]
        double vertexYSigma = 1.; // [mm]
        double vertexZSigma = 300.; // [mm]

        int mCurrEventIdx;

};

#endif
