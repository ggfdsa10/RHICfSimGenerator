#ifndef _OutputPolicyHepMC3_h_
#define _OutputPolicyHepMC3_h_

#include <HepMC3/GenEvent.h>
#include <HepMC3/GenParticle.h>
#include <HepMC3/GenVertex.h>
#include "OutputPolicyNone.h"
#include "CRMChepevt.h"
#include "CRMChepmc3.h"
#include "CRMCstat.h"
#include <HepMC3/WriterAscii.h>
#include <vector>
#include <boost/iostreams/filtering_stream.hpp>

#include "TClonesArray.h"
#include "TParticle.h"

#include "Parameters.hh"
#include "RHICfSimManager.hh"

class CRMCoptions;

class OutputPolicyHepMC3 : public OutputPolicyNone
{
    public:
        OutputPolicyHepMC3() {}
        void InitOutput(const CRMCoptions& cfg) override;
        void FillEvent(const CRMCoptions& cfg, const int nEvent) override;
        void CloseOutput(const CRMCoptions& cfg) override;

        void PrintTestEvent(const CRMCoptions& cfg) override;

    private:
        void EventPrint();

        boost::iostreams::filtering_ostream *fOut;
        CRMChepevt<HepMC3::GenParticlePtr,
            HepMC3::GenVertexPtr,
            HepMC3::FourVector,
            HepMC3::GenEvent> _hepevt;

        CRMChepmc3 _hepmc3;
        HepMC3::WriterAscii* _writer = 0;
        HepMC3::GenEvent _event;

        bool mIsSavedNeutron;

        RHICfSimManager* mSimManager;
        TClonesArray* mParticleArr;
        TParticle* mParticle;

        vector<int> mRHICfGammaIdx;
        bool mIsPi0Event;
        bool mIsNeuEvent;

        int mTmpEventNumber;
        int mTmpProcessId;
};


#endif

