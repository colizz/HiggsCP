#ifndef _HiggsTruthAna_hh_
#define _HiggsTruthAna_hh_

#include <marlin/Processor.h>
#include <TNtuple.h>
#include <TObject.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <EVENT/MCParticle.h>

class HiggsTruthAna : public marlin::Processor
{
    public:

        Processor*  newProcessor() { return new HiggsTruthAna ; }

        HiggsTruthAna();

        ~HiggsTruthAna() {};

        void init();

        void processEvent( LCEvent * evtP );

        void end();

    protected:
        std::string _treeFileName;
        std::string _treeName;
        int _overwrite;
        int _evtReminder;
        TTree *_outputTree;
        std::ostream *_output;
        
        int nMCP, nRecoP, nEvt;
        
        std::vector<int> *iStable_v;
        int ih, iTauP, iTauM, modeTauP, modeTauM;
        
        TLorentzVector *t_p4_h, *t_p4_Z, *t_p4_ZJet, *t_p4_TauM, *t_p4_TauMJet, *t_p4_TauP, *t_p4_TauPJet;
        TLorentzVector *t_p4_vtP, *t_p4_c1P, *t_p4_c2P, *t_p4_c3P, *t_p4_n1P, *t_p4_n2P, *t_p4_n3P;
        TLorentzVector *t_p4_vtM, *t_p4_c1M, *t_p4_c2M, *t_p4_c3M, *t_p4_n1M, *t_p4_n2M, *t_p4_n3M;


};

#endif


