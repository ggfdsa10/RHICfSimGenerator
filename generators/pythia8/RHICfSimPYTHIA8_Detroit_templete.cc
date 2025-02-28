#include <algorithm>
#include <cstring>
#include "Pythia8/Pythia.h"

#include "TString.h"
#include "TH2Poly.h"

#include "RHICfSimManager.hh"
#include "SimDataEvent.hh"
#include "SimDataTrk.hh"
#include "SimDataRHICf.hh"

using namespace Pythia8;
using namespace std;
using namespace SimPars;

int main(int argc, char* argv[]) {
    // ========================= parameters ============================
    TString arg_runType = "";
    TString arg_eventNum = "";
    TString arg_outfilePath = "";
    TString arg_RHICfPi0 = "";

    for(int i=1; i<argc; i++){
        TString ss = argv[i];

        if(ss=="-r" || ss=="--runtype"){
            arg_runType = argv[i+1];
            strcpy(argv[i], "");
        }
        if(ss=="-n" || ss=="--eventnum"){
            arg_eventNum = argv[i+1];
            strcpy(argv[i], "");
        }
        if(ss=="-p" || ss=="--path"){
            arg_outfilePath = argv[i+1];
            strcpy(argv[i], "");
        }
        if(ss=="-o" || ss=="--rhicfpi0"){
            arg_RHICfPi0 = argv[i+1];
            strcpy(argv[i], "");
        }
    }

    // Generator. Shorthand for the event.
    Pythia pythia;
    Event& event = pythia.event;

    // Read in commands from external file.
    pythia.readFile("Diffractive_STARDetroit.cmnd");

    int RHICfRunType = -1; // 0=TS, 1=TL, 2=TOP
    int eventNum = 100;

    if(arg_runType != ""){RHICfRunType = arg_runType.Atoi();}
    else{cout << "There is no RHICf run type !!!, terminated." << endl; return 0;}
    if(arg_eventNum != ""){eventNum = arg_eventNum.Atoi();}
    
    RHICfSimManager* mSimManager = new RHICfSimManager();
    mSimManager -> SetRHICfRunType(rhicfRunType);
    mSimManager -> SetSimModel(tmpModelIdx); 
    mSimManager -> SetFilePath(cfg.fDataSavePath);

    bool mIsSavedNeutron = false;
    if(arg_RHICfPi0 == "N" || arg_RHICfPi0 == "NEUTRON"){
        mIsSavedNeutron = true;
        mSimManager -> SavePi0AndNeutron();
    }

    mSimManager -> Init();
    TClonesArray* mParticleArr = mSimManager -> GetParticleArr();

    // Extract settings to be used in the main program.
    // int    nEvent    = pythia.mode("Main:numberOfEvents");
    int nAbort = pythia.mode("Main:timesAllowErrors");

    // Initialize.
    pythia.init();

    // temprory RHICf particle arrays used in event loop
    int tmpRHICfGammaNum = 0;
    int tmpRHICfGammaIdx[10];
    int tmpRHICfGammaTrkId[10];
    int tmpRHICfGammaType[10];

    int tmpPi0Num = 0;
    int tmpPi0Idx[3];
    int tmpPi0Type[3];
    int tmpPi0DaugIdx[3][2];

    int tmpNeuNum = 0;
    int tmpNeuIdx[3];
    int tmpNeuType[3];

    // Begin event loop.
    int iAbort = 0;
    int tmpEvent = 0;
    while(tmpEvent <= eventNum)
    {
        if(tmpEvent%1000 == 0){cout << "event: " << tmpEvent << " / " << eventNum << endl;}

        // Generate events. Quit if too many failures.
        if (!pythia.next()) {
            if (++iAbort < nAbort) continue;
            cout << " Event generation aborted prematurely, owing to error!\n";
            break;
        }

        simManager -> InitEvent();

        int processId = pythia.info.code();


        // for RHICf particles
        tmpRHICfGammaNum = 0;
        memset(tmpRHICfGammaIdx, 0, sizeof(tmpRHICfGammaIdx));
        memset(tmpRHICfGammaTrkId, 0, sizeof(tmpRHICfGammaTrkId));
        memset(tmpRHICfGammaType, 0, sizeof(tmpRHICfGammaType));

        tmpNeuNum = 0;
        memset(tmpNeuIdx, 0, sizeof(tmpNeuIdx));
        memset(tmpNeuType, 0, sizeof(tmpNeuType));

        tmpPi0Num = 0;
        memset(tmpPi0Idx, 0, sizeof(tmpPi0Idx));
        memset(tmpPi0Type, 0, sizeof(tmpPi0Type));
        memset(tmpPi0DaugIdx, 0, sizeof(tmpPi0DaugIdx));

        // particles
        for (int i = 1; i < event.size(); ++i){
            bool isFinal = event[i].isFinal();

            int pid = event[i].id();

            double pos[3];
            pos[0] = event[i].xProd();
            pos[1] = event[i].yProd();
            pos[2] = event[i].zProd();

            double mom[4];
            mom[0] = event[i].px();
            mom[1] = event[i].py();
            mom[2] = event[i].pz();
            mom[3] = event[i].e();

            if(!isFinal){continue;}

            bool isFinalCharged = Parameters::IsFinalCharged(pid);
            int charge = Parameters::GetCharge(pid);

            simTrk = simManager -> CreateSimTrk();
            simTrk -> SetIsFinal(isFinalCharged);
            simTrk -> SetCharge(charge);
            simTrk -> SetPID(pid);
            simTrk -> SetPos(pos);
            simTrk -> Set4Vec(mom);

            simManager -> GetGeom() -> SetTrk(simManager->GetSimTrkNum()-1, pid, pos, mom);

            int type = simManager -> GetGeom() -> GetRHICfType(pid, pos, mom);
            if(type > 0){
                if(pid == kGamma){
                    if(tmpRHICfGammaNum < 10){
                        tmpRHICfGammaType[tmpRHICfGammaNum] = type;
                        tmpRHICfGammaIdx[tmpRHICfGammaNum] = i;
                        tmpRHICfGammaTrkId[tmpRHICfGammaNum] = simManager->GetSimTrkNum()-1;
                        tmpRHICfGammaNum++;
                    }
                }
                if(pid == kNeutron){
                    if(tmpNeuNum < 3){
                        tmpNeuType[tmpNeuNum] = type;
                        tmpNeuIdx[tmpNeuNum] = i;
                        tmpNeuNum++;
                    }
                }
            }
        }

        // find a pi0 from two gamma
        for(int i=0; i<tmpRHICfGammaNum; i++){
            int gamma1Idx = tmpRHICfGammaIdx[i];
            int gamma1Type = tmpRHICfGammaType[i];

            int gammaMot1Idx = event[gamma1Idx].mother1();
            int gammaMot1PID = event[gammaMot1Idx].id();
            if(gammaMot1PID != kPi0){continue;}

            for(int j=i+1; j<tmpRHICfGammaNum; j++){
                int gamma2Idx = tmpRHICfGammaIdx[j];
                int gamma2Type = tmpRHICfGammaType[j];

                int gammaMot2Idx = event[gamma2Idx].mother1();
                int gammaMot2PID = event[gammaMot2Idx].id();
                if(gammaMot2PID != kPi0){continue;}

                if(gammaMot1Idx == gammaMot2Idx){
                    if(tmpPi0Num >= 3){break;}
                    int pi0Type = (gamma1Type == gamma2Type)? 2 : 1;

                    tmpPi0Idx[tmpPi0Num] = gammaMot2Idx;
                    tmpPi0Type[tmpPi0Num] = pi0Type;
                    tmpPi0DaugIdx[tmpPi0Num][0] = i;
                    tmpPi0DaugIdx[tmpPi0Num][1] = j;
                    tmpPi0Num++;
                }
            }
        }

        // ======== SimDataRHICf ==========
        for(int i=0; i<tmpPi0Num; i++){
            int Idx = tmpPi0Idx[i];
            int type = tmpPi0Type[i];

            int pid;
            double pos[3];
            double mom[4];

            int motherTrkId = -1;

            // RHICf pi0 mother particles
            int motherIdx = event[Idx].mother1();
            if(motherIdx > 0){
                pid = event[motherIdx].id();
                pos[0] = event[motherIdx].xProd();
                pos[1] = event[motherIdx].yProd();
                pos[2] = event[motherIdx].zProd();
                mom[0] = event[motherIdx].px();
                mom[1] = event[motherIdx].py();
                mom[2] = event[motherIdx].pz();
                mom[3] = event[motherIdx].e();

                bool isFinalCharged = Parameters::IsFinalCharged(pid);
                int charge = Parameters::GetCharge(pid);

                simTrk = simManager -> CreateSimTrk();
                simTrk -> SetIsFinal(isFinalCharged);
                simTrk -> SetCharge(charge);
                simTrk -> SetPID(pid);
                simTrk -> SetPos(pos);
                simTrk -> Set4Vec(mom);

                motherTrkId = simManager->GetSimTrkNum()-1;
            }

            pid = event[Idx].id();
            pos[0] = event[Idx].xProd();
            pos[1] = event[Idx].yProd();
            pos[2] = event[Idx].zProd();
            mom[0] = event[Idx].px();
            mom[1] = event[Idx].py();
            mom[2] = event[Idx].pz();
            mom[3] = event[Idx].e();

            simRHICf = simManager -> CreateSimRHICf();
            simRHICf -> SetIsPi0(true);
            simRHICf -> SetType(type);
            simRHICf -> SetMotherTrkId(motherTrkId);
            simRHICf -> SetPos(pos);
            simRHICf -> Set4Vec(mom);

            int gammaTrkId1 = tmpRHICfGammaTrkId[tmpPi0DaugIdx[i][0]];
            int gammaTrkId2 = tmpRHICfGammaTrkId[tmpPi0DaugIdx[i][1]];

            simRHICf -> SetDaughterTrkId(0, gammaTrkId1);
            simRHICf -> SetDaughterTrkId(1, gammaTrkId2);
        }

        for(int i=0; i<tmpNeuNum; i++){
            int Idx = tmpNeuIdx[i];
            int type = tmpNeuType[i];
            
            int pid;
            double pos[3];
            double mom[4];

            int motherTrkId = -1;

            // RHICf Neutron mother particles
            int motherIdx = event[Idx].mother1();
            if(motherIdx > 0){
                pid = event[motherIdx].id();
                pos[0] = event[motherIdx].xProd();
                pos[1] = event[motherIdx].yProd();
                pos[2] = event[motherIdx].zProd();
                mom[0] = event[motherIdx].px();
                mom[1] = event[motherIdx].py();
                mom[2] = event[motherIdx].pz();
                mom[3] = event[motherIdx].e();

                bool isFinalCharged = Parameters::IsFinalCharged(pid);
                int charge = Parameters::GetCharge(pid);

                simTrk = simManager -> CreateSimTrk();
                simTrk -> SetIsFinal(isFinalCharged);
                simTrk -> SetCharge(charge);
                simTrk -> SetPID(pid);
                simTrk -> SetPos(pos);
                simTrk -> Set4Vec(mom);

                motherTrkId = simManager->GetSimTrkNum()-1;
            }

            pid = event[Idx].id();
            pos[0] = event[Idx].xProd();
            pos[1] = event[Idx].yProd();
            pos[2] = event[Idx].zProd();
            mom[0] = event[Idx].px();
            mom[1] = event[Idx].py();
            mom[2] = event[Idx].pz();
            mom[3] = event[Idx].e();
            
            simRHICf = simManager -> CreateSimRHICf();
            simRHICf -> SetIsPi0(false);
            simRHICf -> SetType(type);
            simRHICf -> SetMotherTrkId(motherTrkId);
            simRHICf -> SetPos(pos);
            simRHICf -> Set4Vec(mom);
        }

        simEvent = simManager -> CreateSimEvent();
        simEvent -> SetProcessId(processId);
        simEvent -> SetTrkNum(simManager->GetSimTrkNum());
        simEvent -> SetRHICfPi0Num(tmpPi0Num);
        simEvent -> SetRHICfNeuNum(tmpNeuNum);

        simManager -> Fill();
        tmpEvent++;
    }

    simManager -> Close();
    return 0;
}
