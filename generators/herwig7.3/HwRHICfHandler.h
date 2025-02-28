
#ifndef HERWIG_HwRHICfHandler_H
#define HERWIG_HwRHICfHandler_H

#include "ThePEG/Repository/CurrentGenerator.h"
#include "ThePEG/Handlers/AnalysisHandler.h"
#include "ThePEG/Config/Pointers.h"
#include "Herwig/Utilities/Histogram.h"

#include <cmath>
#include <fstream>
#include <vector>
#include <map>

//Boost headers
#include <boost/algorithm/string.hpp>
#include <boost/tuple/tuple.hpp>

#include "TString.h"
#include "TClonesArray.h"
#include "RHICfSimManager.hh"

namespace Herwig 
{
    class HwRHICfHandler;
}

namespace ThePEG 
{
    ThePEG_DECLARE_POINTERS(Herwig::HwRHICfHandler,HwRHICfHandlerPtr);
}

namespace Herwig 
{
    using namespace ROOT;
    using namespace ThePEG;
    using namespace std;
    using namespace SimPars;
        
    class HwRHICfHandler: public AnalysisHandler 
    {
        public:
            inline HwRHICfHandler();

            static void Init(); // initialization for ThePEG interface

            void persistentOutput(PersistentOStream & os) const; // Ouput set parameter for ThePEG interface
            void persistentInput(PersistentIStream & is, int version); // Input set parameter for ThePEG interface

            virtual void analyze(tEventPtr event, long ieve, int loop, int state); // Main processing function for events

        protected:
            inline virtual void doinitrun(); // Run initialization with AnalysisHandler
            virtual void dofinish(); // Run finishing with AnalysisHandler
            
            inline virtual IBPtr clone() const;
            inline virtual IBPtr fullclone() const;

        private:
            void ClearEvent();
            void InitRHICfSim();
            void FinishRHICfSim();
            void EventPrint();

            int mRHICfRunType;
            string mOutputPath;
            string mSaveRHCIfNeu;
            bool mIsSavedNeutron;

            int mTmpEventNumber;
            TString mTmpEventProcName;
            int mFinalTrkNum;
            PVector mFinalTrkVector; // Final state particle array
            Lorentz5Momentum mTrkMom;
            LorentzPoint mTrkVertexPoint;
            LorentzVector<Length> mTrkPos;

            RHICfSimManager* mSimManager;
            TClonesArray* mParticleArr;
            TParticle* mParticle;

            vector<int> mRHICfGammaIdx;
            bool mIsPi0Event;
            bool mIsNeuEvent;


            static ClassDescription<HwRHICfHandler> initHwRHICfHandler;
            HwRHICfHandler & operator=(const HwRHICfHandler &);
    };
}

#include "ThePEG/Utilities/ClassTraits.h"

namespace ThePEG 
{

    /** @cond TRAITSPECIALIZATIONS */

    /** This template specialization informs ThePEG about the
    *  base classes of HwRHICfHandler. */
    template <>
    struct BaseClassTrait<Herwig::HwRHICfHandler,1> {
    /** Typedef of the first base class of HwRHICfHandler. */
    typedef AnalysisHandler NthBase;
    };

    /** This template specialization informs ThePEG about the name of
    *  the HwRHICfHandler class and the shared object where it is defined. */
    template <>
    struct ClassTraits<Herwig::HwRHICfHandler>
    : public ClassTraitsBase<Herwig::HwRHICfHandler> {
    /** Return a platform-independent class name */
    static string className() { return "Herwig::HwRHICfHandler"; }
    /** Return the name(s) of the shared library (or libraries) be loaded to get
    *  access to the HwRHICfHandler class and any other class on which it depends
    *  (except the base class). */
    static string library() { return "HwRHICfHandler.so"; }
    };
    /** @endcond */
}

#endif /* HERWIG_HwRHICfHandler_H */
