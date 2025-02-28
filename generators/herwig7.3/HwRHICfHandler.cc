#include <cmath>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

#include "HwRHICfHandler.h"
#include "Herwig/Utilities/Histogram.h"
#include "ThePEG/Repository/EventGenerator.h"
#include "ThePEG/EventRecord/Particle.h"
#include "ThePEG/Vectors/ThreeVector.h"
#include "ThePEG/EventRecord/Event.h"
#include "ThePEG/PDT/EnumParticles.h"
#include "ThePEG/Interface/Switch.h"
#include "ThePEG/Interface/Reference.h"
#include "ThePEG/Repository/UseRandom.h"
#include "ThePEG/Interface/Parameter.h"
#include "ThePEG/Interface/ClassDocumentation.h"
#include "ThePEG/Persistency/PersistentOStream.h"
#include "ThePEG/Persistency/PersistentIStream.h"
#include "ThePEG/EventRecord/SubProcess.h"
#include "ThePEG/EventRecord/SubProcessGroup.h"

using namespace std;
using namespace Herwig;

HwRHICfHandler::HwRHICfHandler()
: mRHICfRunType(kTL), mOutputPath("./"), mSaveRHCIfNeu(""), mIsSavedNeutron(false), mTmpEventNumber(0)
{
}

ClassDescription<HwRHICfHandler> HwRHICfHandler::initHwRHICfHandler;

void HwRHICfHandler::Init() {

    static ClassDocumentation<HwRHICfHandler> documentation
        ("The HwRHICfHandler class has been created for RHICf simulations");
    
    static Parameter<HwRHICfHandler, int> interfaceRHICfRunType
        ("RHICfRunType", 
        "RHICf Run Types", 
        &HwRHICfHandler::mRHICfRunType, 0, 0, 2, false, false, Interface::limited);

    static Parameter<HwRHICfHandler, string> interfaceOutputPath
        ("OutputPath",
        "The output path for the root files",
        &HwRHICfHandler::mOutputPath, "./", false, false);

    static Parameter<HwRHICfHandler, string> interfaceVersion
        ("SaveOption",
        "Save the RHICf Neutron event with RHICf pi0 event",
        &HwRHICfHandler::mSaveRHCIfNeu, "", false, false);
}

void HwRHICfHandler::persistentInput(PersistentIStream & is, int) 
{
    is >> mRHICfRunType >> mOutputPath >> mSaveRHCIfNeu;
}

void HwRHICfHandler::persistentOutput(PersistentOStream & os) const 
{
    os  << mRHICfRunType << mOutputPath << mSaveRHCIfNeu;
}

void HwRHICfHandler::analyze(tEventPtr event, long, int, int) 
{
    // Event Header
    int eventIdx = event -> number() - 1;
    int eventWeight = event -> weight();

    if(eventIdx%2000 == 0){cout << "Event: " << eventIdx << endl;}

    ClearEvent();

    double collisionVtxX = 0.; // [mm]
    double collisionVtxY = 0.; // [mm]
    double collisionVtxZ = 0.; // [mm]
    mSimManager -> GetVertexRandom(collisionVtxX, collisionVtxY, collisionVtxZ);

    tSubProPtr primarySubProc = event -> primarySubProcess();
    mTmpEventProcName = EventConfig::nameHandler(primarySubProc->handler());

    // Pythia format process id
    int processId = 0;
    if(mTmpEventProcName == "MEDiffractionLeft" || mTmpEventProcName == "MEDiffractionRight"){processId = 103;} // Left SD
    if(mTmpEventProcName == "MEDiffractionDeltaLeft" || mTmpEventProcName == "MEDiffractionDeltaRight"){processId = 104;} // Right SD
    if(mTmpEventProcName == "MEDiffractionDouble" || mTmpEventProcName == "MEDiffractionDeltaDouble"){processId = 105;} // DD
    if(mTmpEventProcName == "MEMinBias"){processId = 101;} // ND

    //Primary collision
    StepVector::const_iterator sit =event->primaryCollision()->steps().begin();
    StepVector::const_iterator send=event->primaryCollision()->steps().end(); 
    ParticleSet partW=(**sit).all();

    // Set the Event information
    double sHat = -999;
    double tHat = -999;
    double uHat = -999;
    mSimManager -> SetEventInfo(processId, sHat, tHat, uHat);

    // Find a last particle number
    int lastNumber = 0;
    event -> getFinalState(mFinalTrkVector); 
    for(int i=0; i<mFinalTrkVector.size(); i++){
        int num = mFinalTrkVector[i] -> number();
        if(lastNumber < num){lastNumber = num;}
    }
    mParticleArr -> ExpandCreate(lastNumber);

    // Set the incident particles
    const auto& incidentParticles = event->primaryCollision()->incoming();
    mTrkVertexPoint = incidentParticles.first -> vertex();
    mTrkPos = mTrkVertexPoint;
    mTrkMom = incidentParticles.first -> momentum();

    double primaryVx = mTrkPos.x()/mm + collisionVtxX; // created position X of partocle [mm]
    double primaryVy = mTrkPos.y()/mm + collisionVtxY; // created position Y of partocle [mm]
    double primaryVz = mTrkPos.z()/mm + collisionVtxZ; // created position Z of partocle [mm]
    double primaryT = mTrkPos.t()/mm * 0.001; // created position Z of partocle [mm/c]
    double primaryPx = mTrkMom.x()/GeV;
    double primaryPy = mTrkMom.y()/GeV;
    double primaryPz = mTrkMom.z()/GeV;
    double primaryE = mTrkMom.e()/GeV; 

    mParticle = (TParticle*)mParticleArr -> At(0);
    mParticle -> SetPdgCode(incidentParticles.first->id());
    mParticle -> SetStatusCode(4);
    mParticle -> SetProductionVertex(primaryVx, primaryVy, primaryVz, primaryT);
    mParticle -> SetMomentum(primaryPx, primaryPy, primaryPz, primaryE);
    mParticle -> SetFirstMother(0);
    mParticle -> SetLastMother(0);
    mParticle -> SetCalcMass(incidentParticles.first -> mass()/GeV);

    mTrkVertexPoint = incidentParticles.second -> vertex();
    mTrkPos = mTrkVertexPoint;
    mTrkMom = incidentParticles.second -> momentum();

    primaryVx = mTrkPos.x()/mm + collisionVtxX; // created position X of partocle [mm]
    primaryVy = mTrkPos.y()/mm + collisionVtxY; // created position Y of partocle [mm]
    primaryVz = mTrkPos.z()/mm + collisionVtxZ; // created position Z of partocle [mm]
    primaryT = mTrkPos.t()/mm * 0.001; // created position Z of partocle [mm/c]
    primaryPx = mTrkMom.x()/GeV;
    primaryPy = mTrkMom.y()/GeV;
    primaryPz = mTrkMom.z()/GeV;
    primaryE = mTrkMom.e()/GeV; 

    mParticle = (TParticle*)mParticleArr -> At(1);
    mParticle -> SetPdgCode(incidentParticles.second->id());
    mParticle -> SetStatusCode(4);
    mParticle -> SetProductionVertex(primaryVx, primaryVy, primaryVz, primaryT);
    mParticle -> SetMomentum(primaryPx, primaryPy, primaryPz, primaryE);
    mParticle -> SetFirstMother(0);
    mParticle -> SetLastMother(0);
    mParticle -> SetCalcMass(incidentParticles.second -> mass()/GeV);

    mFinalTrkNum = 0;
    for (const auto& collision : event -> collisions()) {
        const StepVector& steps = collision->steps();
        for (const auto& step : steps) {
            TString ProcessName2 = EventConfig::nameHandler(step->handler());

            const ParticleSet& particles = step->all();
            for (const auto& particle : particles) {
                int number = particle -> number() -1;
                int pid = particle -> id();
                double mass = particle -> mass()/GeV;
                int parentSize = particle -> parents().size();
                int daughterSize = particle -> children().size();

                int status = 0;
                if(parentSize == 1 && daughterSize == 0){status = 1;} // final state
                else if(parentSize == 1 && daughterSize > 1){status = 2;} // decayed particle
                else if(parentSize == 1 && daughterSize == 1){status = 3;} // transported particle
                else{status = 81;}
                if(pid == 81){status = 81;}
                if(number < 2){status = 4;} // incident particle

                int parentNumberIdx1 = -1;
                int parentNumberIdx2 = -1;
                if(parentSize == 1){
                    parentNumberIdx1 = particle -> parents()[0]->number();
                    parentNumberIdx2 = 0;
                }

                int daughterNumberIdx1 = -1;
                int daughterNumberIdx2 = -1;
                if(status == 2){
                    daughterNumberIdx1 = particle -> children()[0]->number();
                    daughterNumberIdx2 = particle -> children()[1]->number();
                }
                if(status == 3){daughterNumberIdx1 = particle -> children()[0]->number();}
                
                mTrkVertexPoint = particle -> vertex();
                mTrkPos = mTrkVertexPoint;
                mTrkMom = particle -> momentum();

                double vx = mTrkPos.x()/mm + collisionVtxX; // created position X of partocle [mm]
                double vy = mTrkPos.y()/mm + collisionVtxY; // created position Y of partocle [mm]
                double vz = mTrkPos.z()/mm + collisionVtxZ; // created position Z of partocle [mm]
                double t = mTrkPos.t()/mm * 0.001; // created position Z of partocle [mm/c]
                double px = mTrkMom.x()/GeV;
                double py = mTrkMom.y()/GeV;
                double pz = mTrkMom.z()/GeV;
                double e = mTrkMom.e()/GeV; 

                mParticle = (TParticle*)mParticleArr -> At(number);
                mParticle -> SetPdgCode(pid);
                mParticle -> SetStatusCode(status);
                mParticle -> SetProductionVertex(vx, vy, vz, t); // [mm, mm, mm, mm/c]
                mParticle -> SetMomentum(px, py, pz, e); // [GeV/c]
                mParticle -> SetCalcMass(mass); // [GeV/c^2]
                mParticle -> SetFirstMother(parentNumberIdx1);
                mParticle -> SetLastMother(parentNumberIdx2);
                mParticle -> SetFirstDaughter(daughterNumberIdx1);
                mParticle -> SetLastDaughter(daughterNumberIdx2);

                // Find a RHICf hit particles
                if(status == 1 && ProcessName2.Index("Decay") != -1){ // final state particle 
                    int hit = mSimManager -> GetRHICfGeoHit(vx, vy, vz, px, py, pz, e);
                    if(hit < 0){continue;}

                    if(pid == 22){ // final state gamma and neutron
                        mRHICfGammaIdx.push_back(number);
                    }
                    if(pid == 2112){
                        mIsNeuEvent = true;
                    }
                }
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

        if(gamma1MotherPID != 111){continue;}

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

void HwRHICfHandler::doinitrun() 
{
    std::cout << "HwRHICfHandler Initialization..." << endl;

    AnalysisHandler::doinitrun();

    InitRHICfSim();
    ClearEvent();
}

void HwRHICfHandler::dofinish() 
{
    AnalysisHandler::dofinish();
    
    FinishRHICfSim();
}

IBPtr HwRHICfHandler::clone() const 
{
  return new_ptr(*this);
}

IBPtr HwRHICfHandler::fullclone() const 
{
  return new_ptr(*this);
}

void HwRHICfHandler::ClearEvent()
{
    mSimManager -> Clear();
    mFinalTrkVector.clear();

    mRHICfGammaIdx.clear();
    mIsPi0Event = false;
    mIsNeuEvent = false;
}

void HwRHICfHandler::InitRHICfSim() 
{   
    mSimManager = new RHICfSimManager();
    mSimManager -> SetRHICfRunType(mRHICfRunType);
    mSimManager -> SetSimModel(1); // HERWIG7.3
    mSimManager -> SetFilePath(mOutputPath);

    TString tmpOption = mSaveRHCIfNeu;
    tmpOption.ToUpper();
    if(tmpOption == "N" || tmpOption == "NEUTRON"){
        mIsSavedNeutron = true;
        mSimManager -> SavePi0AndNeutron();
    }

    mSimManager -> Init();
    mParticleArr = mSimManager -> GetParticleArr();
}

void HwRHICfHandler::FinishRHICfSim() 
{ 
    mSimManager -> Close();
}

void HwRHICfHandler::EventPrint()
{
    cout << "===================== HwRHICfHandler::EventPrint() ===================== " << endl;
    cout << " Saved Event Number       : " << mTmpEventNumber << endl;
    cout << " Event Process Id         : " << mTmpEventProcName  << endl;
    cout << " Final state Track Number : " << mFinalTrkVector.size()  << endl;
    cout << " Total Particle Number    : " << mParticleArr -> GetEntries() << endl;
    cout << " RHICf Hitted Particle    : " << mRHICfGammaIdx.size()+int(mIsNeuEvent) << endl;
    cout << " RHICf Pi0 Event          : " << TString((mIsPi0Event == true)? "TRUE" : "FALSE") << endl; 
    cout << " RHICf Neutron Event      : " << TString((mIsNeuEvent == true)? "TRUE" : "FALSE") << endl; 
    cout << "======================================================================== " << endl;
}