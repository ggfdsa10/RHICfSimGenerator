#ifndef Parameters_HH
#define Parameters_HH

#include "TString.h"

namespace SimPars{
    // simulation model index
    static const int kModelNum = 5;
    static const int kPYTHIA8 = 0;
    static const int kHERWIG7 = 1;
    static const int kEPOSLHC = 2;
    static const int kQGSJETII04 = 3;
    static const int kQGSJETIII = 4;
    static const TString kModelName[kModelNum] = {"Pythia8"
                                                , "Herwig7"
                                                , "EPOSLHC"
                                                , "QGSJETII04"
                                                , "QGSJETIII"};

    // general process index
    static const int kSingleDiff = 1;
    static const int kDoubleDiff = 2;
    static const int kNonDiff = 3;
    static const int kCentralDiff = 4;
    static const int kElastic = 5;
    static const int kDeltaSingleDiff = -1;
    static const int kDeltaDoubleDiff = -2;

    // RHICf run type
    static const int kRHICfRunTypeNum = 3;
    static const int kTL = 0;
    static const int kTS = 1;
    static const int kTOP = 2;
    static const TString kRHICfRunTypeName[kRHICfRunTypeNum] = {"TL"
                                                               ,"TS"
                                                               ,"TOP"};

    // RHICf Pi0 type
    static double kRHICfEnergyCut = 20.; // [GeV]
    static const int kType1 = 1;
    static const int kType2 = 2;

    // particle PDG 
    static const int kElectron = 11;
    static const int kPositron = -11;
    static const int kNeutrinoElectron = 12;
    static const int kAntiNeutrinoElectron = -12;
    static const int kMuon = 13;
    static const int kAntiMuon = -13;
    static const int kNeutrinoMuon = 14;
    static const int kAntiNeutrinoMuon = -14;
    static const int kNeutrinoTau = 16;
    static const int kAntiNeutrinoTau = 16;
    static const int kGamma = 22;
    static const int kPi0 = 111;
    static const int kPlusPion = 211;
    static const int kMinusPion = -211;
    static const int kPlusKion = 321;
    static const int kMinusKion = -321;
    static const int kKion0 = 130;
    static const int kProton = 2212;
    static const int kAntiProton = -2212;
    static const int kNeutron = 2112;
    static const int kLambda0 = 3122;
    static const int kSigma0 = 3212;


    class Parameters
    {
    public:
        Parameters(){;}
        ~Parameters(){;}

        static bool IsFinalCharged(int pid){if(pid == SimPars::kElectron
                                 || pid == SimPars::kPositron
                                 || pid == SimPars::kMuon
                                 || pid == SimPars::kAntiMuon
                                 || pid == SimPars::kGamma
                                 || pid == SimPars::kPlusPion
                                 || pid == SimPars::kMinusPion
                                 || pid == SimPars::kPlusKion
                                 || pid == SimPars::kMinusKion
                                 || pid == SimPars::kProton
                                 || pid == SimPars::kAntiProton)
                                 {return true;}
                                 return false; }

        static int GetCharge(int pid){if(!IsFinalCharged(pid)){return 0;}
                                else if(pid == SimPars::kGamma){return 0;}
                                else if(pid > 0){return 1;}
                                else if(pid < 0){return -1;}
                                return 0;}
    };
};

#endif