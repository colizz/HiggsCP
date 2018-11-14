#include <HiggsCPSimAna.hh>
#include <Utils.hh>
#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include <EVENT/LCFloatVec.h>
#include <EVENT/MCParticle.h>
#include <EVENT/ReconstructedParticle.h>
#include <IMPL/MCParticleImpl.h>
#include <values.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <EVENT/LCFloatVec.h>
#include <EVENT/LCParameters.h>
#include <stdexcept>
#include <TFile.h> 
#include <TTree.h>
#include <TRandom.h>
#include <Rtypes.h> 
#include <sstream>        
#include <cmath>
#include <vector>
#include <queue>
#include <algorithm>
#include <TMath.h>

using namespace std;
using namespace lcio;
using namespace marlin;

HiggsCPSimAna a_ExportEvt_instance;

HiggsCPSimAna::HiggsCPSimAna()
    : Processor("HiggsCPSimAna"),
    _output(0)
{
    _description = "Analysis Higgs CP property on simulation" ;

    registerProcessorParameter( "TreeOutputFile" , 
            "The name of the file to which the ROOT tree will be written" ,
            _treeFileName ,
            _treeFileName);

    registerProcessorParameter( "TreeName" , 
            "The name of the ROOT tree" ,
            _treeName ,
            _treeName);

    registerProcessorParameter( "OverwriteFile" , 
            "If zero an already existing file will not be overwritten." ,
            _overwrite ,
            _overwrite);

    registerProcessorParameter( "EvtReminder" , 
            "Remind after every n events." ,
            _evtReminder ,
            _evtReminder);

}

void HiggsCPSimAna::init() {

    printParameters();

    TFile *tree_file=new TFile(_treeFileName.c_str(), (_overwrite ? "RECREATE" : "UPDATE"));
    if (!tree_file->IsOpen()) {
        delete tree_file;
        tree_file=new TFile(_treeFileName.c_str(), "NEW");
    }
    _outputTree = new TTree(_treeName.c_str(), _treeName.c_str());
    
    ////////////////////////////////////////
    
    _outputTree->Branch("nEvt", &nEvt, "nEvt/I"); // event number
    _outputTree->Branch("nMCP", &nMCP, "nMCP/I");  // # MC particle
    _outputTree->Branch("nRecoP", &nRecoP, "nRecoP/I"); // # reco particle
    
    // _outputTree->Branch("r_i1", "vector<int>", &r_i1);
    // _outputTree->Branch("r_i2", "vector<int>", &r_i2);
    // _outputTree->Branch("r_i3", "vector<int>", &r_i3);
    // _outputTree->Branch("r_iDis1", "vector<float>", &r_iDis1);
    // _outputTree->Branch("r_iDis2", "vector<float>", &r_iDis2);
    // _outputTree->Branch("r_iDis3", "vector<float>", &r_iDis3);
    
    _outputTree->Branch("iStable_v", "vector<int>", &iStable_v); // particles that reach the detectors, (MCParticle*)->int
    // _outputTree->Branch("iTauP", &iTauP, "iTauP/I"); // Tau+ pointer
    // _outputTree->Branch("iTauM", &iTauM, "iTauM/I"); // Tau- pointer
    _outputTree->Branch("modeTauP", &modeTauP, "modeTauP/I"); // Tau+ decay mode, notation defined below
    _outputTree->Branch("modeTauM", &modeTauM, "modeTauM/I"); // Tau+ decay mode
    
    // _outputTree->Branch("ptr_TauMJet_v", "vector<long>", &ptr_TauMJet_v); // stable Tau+ decay products
    // _outputTree->Branch("ptr_TauPJet_v", "vector<long>", &ptr_TauPJet_v); // stable Tau- decay products
    // _outputTree->Branch("ptr_ZjJet_v", "vector<long>", &ptr_ZjJet_v); // stable Z decay products
    
    t_p4_h = new TLorentzVector(0,0,0,0);
    t_p4_Z = new TLorentzVector(0,0,0,0);
    t_p4_ZJet = new TLorentzVector(0,0,0,0);
    t_p4_TauM = new TLorentzVector(0,0,0,0);
    t_p4_TauMJet = new TLorentzVector(0,0,0,0);
    t_p4_TauP = new TLorentzVector(0,0,0,0);
    t_p4_TauPJet = new TLorentzVector(0,0,0,0);
    _outputTree->Branch("t_p4_h", "TLorentzVector", &t_p4_h); // higgs 4-momentum
    _outputTree->Branch("t_p4_Z", "TLorentzVector", &t_p4_Z); // Z 4-momentum
    _outputTree->Branch("t_p4_ZJet", "TLorentzVector", &t_p4_ZJet); // 4-mom from Z decay truth product
    _outputTree->Branch("t_p4_TauM", "TLorentzVector", &t_p4_TauM); // 4-mom of Tau- 
    _outputTree->Branch("t_p4_TauMJet", "TLorentzVector", &t_p4_TauMJet); // 4-mom from Tau- truth product 
    _outputTree->Branch("t_p4_TauP", "TLorentzVector", &t_p4_TauP); // 4-mom of Tau+
    _outputTree->Branch("t_p4_TauPJet", "TLorentzVector", &t_p4_TauPJet); // 4-mom from Tau+ truth product 
    
    // TauP/M decay product in following notation 
    // c123=charged hadron, n123=neutral pions, vt=tau neutrino. For leptonic decay c1=e/mu, n1=ve/vm
    // modeTau       vt  c1  c2  c3  n1  n2  n3
    //   1   1h0pi   √   √ 
    //   2   1h1pi   √   √           √
    //   3   1h2pi   √   √           √   √
    //   4   1h3pi   √   √           √   √   v
    //  11   3h0pi   √   √   √   √ 
    //  12   3h1pi   √   √   √   √   √
    //  13   3h2pi   √   √   √   √   √   √   √
    //  -1   e       √   √           √
    //  -2   mu      √   √           √
    t_p4_vtP = new TLorentzVector(0,0,0,0);
    t_p4_c1P = new TLorentzVector(0,0,0,0);
    t_p4_c2P = new TLorentzVector(0,0,0,0);
    t_p4_c3P = new TLorentzVector(0,0,0,0);
    t_p4_n1P = new TLorentzVector(0,0,0,0);
    t_p4_n2P = new TLorentzVector(0,0,0,0);
    t_p4_n3P = new TLorentzVector(0,0,0,0);
    t_p4_vtM = new TLorentzVector(0,0,0,0);
    t_p4_c1M = new TLorentzVector(0,0,0,0);
    t_p4_c2M = new TLorentzVector(0,0,0,0);
    t_p4_c3M = new TLorentzVector(0,0,0,0);
    t_p4_n1M = new TLorentzVector(0,0,0,0);
    t_p4_n2M = new TLorentzVector(0,0,0,0);
    t_p4_n3M = new TLorentzVector(0,0,0,0);
    _outputTree->Branch("t_p4_vtP", "TLorentzVector", &t_p4_vtP); 
    _outputTree->Branch("t_p4_c1P", "TLorentzVector", &t_p4_c1P); 
    _outputTree->Branch("t_p4_c2P", "TLorentzVector", &t_p4_c2P); 
    _outputTree->Branch("t_p4_c3P", "TLorentzVector", &t_p4_c3P); 
    _outputTree->Branch("t_p4_n1P", "TLorentzVector", &t_p4_n1P); 
    _outputTree->Branch("t_p4_n2P", "TLorentzVector", &t_p4_n2P); 
    _outputTree->Branch("t_p4_n3P", "TLorentzVector", &t_p4_n3P); 
    _outputTree->Branch("t_p4_vtM", "TLorentzVector", &t_p4_vtM); 
    _outputTree->Branch("t_p4_c1M", "TLorentzVector", &t_p4_c1M); 
    _outputTree->Branch("t_p4_c2M", "TLorentzVector", &t_p4_c2M); 
    _outputTree->Branch("t_p4_c3M", "TLorentzVector", &t_p4_c3M); 
    _outputTree->Branch("t_p4_n1M", "TLorentzVector", &t_p4_n1M); 
    _outputTree->Branch("t_p4_n2M", "TLorentzVector", &t_p4_n2M); 
    _outputTree->Branch("t_p4_n3M", "TLorentzVector", &t_p4_n3M); 
}

void HiggsCPSimAna::processEvent( LCEvent * evtP ) 
{        

    if (evtP) {        
        try{            
            LCCollection* col_MCP = evtP->getCollection( "MCParticlesSkimmed" ) ;
            LCCollection* col_RecoP = evtP->getCollection( "ArborPFOs" );
            nMCP = col_MCP->getNumberOfElements();
            nRecoP = col_RecoP->getNumberOfElements();    
            nEvt = evtP->getEventNumber();
            
            std::vector<ReconstructedParticle *> recop;
            std::vector<MCParticle *> mcp;
            for (int i=0; i<nRecoP; i++)
                recop.push_back( dynamic_cast<ReconstructedParticle*>(col_RecoP->getElementAt(i)) );
            for (int i=0; i<nMCP; i++)
                mcp.push_back( dynamic_cast<MCParticle*>(col_MCP->getElementAt(i)) );
            
            if (nEvt % _evtReminder == 0)  message<MESSAGE>( log()<<"+++++++++++++++  Processing Evt # "<<nEvt<<"  +++++++++++++++" );
            
            
            // 1. Fastjet: Cluster tau jet and judge isHtt, cluster Z jet then
            ClusterTauJet()
            isTruthHtt = 
            isHtt = 
            
            // 2. If isHtt=true, find Zmode, TauMmode, TauPmode
            Zmode = 
            TauMmode = 
            TauPmode = 
            
            
            // 3. MINUIT: Reconstruct higgs 4-mom
            
            
            // 4. MINUIT: Reconstruct nuetrinos via impact parameters
            
            
            // 5. 
            
            _outputTree->Fill();
        } 
        catch (lcio::DataNotAvailableException err) { 
            // message<DEBUG>( log()<<err.what() ); 
        }
    } 
} 

void HiggsCPSimAna::end()
{

    if (_outputTree) {

        TFile *tree_file = _outputTree->GetCurrentFile(); //just in case we switched to a new file
        tree_file->cd();
        tree_file->Write();
        tree_file->Close();
    }

}
