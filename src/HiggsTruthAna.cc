#include <HiggsTruthAna.hh>
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

HiggsTruthAna a_ExportEvt_instance;

HiggsTruthAna::HiggsTruthAna()
    : Processor("HiggsTruthAna"),
    _output(0)
{
    _description = "Print MC Truth" ;

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

void HiggsTruthAna::init() {

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
    _outputTree->Branch("modeTauP", &modeTauP, "modeTauP/I"); // Tau+ decay mode
    _outputTree->Branch("modeTauM", &modeTauM, "modeTauM/I"); // Tau+ decay mode
    
    // _outputTree->Branch("ptr_TauMJet_v", "vector<long>", &ptr_TauMJet_v); // Stable Tau+ decayed objects
    // _outputTree->Branch("ptr_TauPJet_v", "vector<long>", &ptr_TauPJet_v); // Stable Tau- decayed objects
    // _outputTree->Branch("ptr_ZjJet_v", "vector<long>", &ptr_ZjJet_v); // Stable Z decayed objects
    
    t_p4_h = new TLorentzVector(0,0,0,0);
    t_p4_Z = new TLorentzVector(0,0,0,0);
    t_p4_ZJet = new TLorentzVector(0,0,0,0);
    t_p4_TauM = new TLorentzVector(0,0,0,0);
    t_p4_TauMJet = new TLorentzVector(0,0,0,0);
    t_p4_TauP = new TLorentzVector(0,0,0,0);
    t_p4_TauPJet = new TLorentzVector(0,0,0,0);
    _outputTree->Branch("t_p4_h", "TLorentzVector", &t_p4_h); 
    _outputTree->Branch("t_p4_Z", "TLorentzVector", &t_p4_Z); 
    _outputTree->Branch("t_p4_ZJet", "TLorentzVector", &t_p4_ZJet); 
    _outputTree->Branch("t_p4_TauM", "TLorentzVector", &t_p4_TauM); 
    _outputTree->Branch("t_p4_TauMJet", "TLorentzVector", &t_p4_TauMJet); 
    _outputTree->Branch("t_p4_TauP", "TLorentzVector", &t_p4_TauP); 
    _outputTree->Branch("t_p4_TauPJet", "TLorentzVector", &t_p4_TauPJet); 
    
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

void HiggsTruthAna::processEvent( LCEvent * evtP ) 
{        

    if (evtP) {        
        try{            
            //gSystem->Load("../lib/loader_C.so");
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
            
            if (nEvt % _evtReminder == 0) cout<<"+++ Evt "<<nEvt<<endl;
            
            int HDecayTo = -99, ih = -99;
            for (int i=0; i<nMCP; i++)
                if ( mcp[i]->getPDG()==25 ){
                    if (mcp[i]->getDaughters()[0]->getPDG()==25) continue;
                    HDecayTo = abs( mcp[i]->getDaughters()[0]->getPDG() );
                    ih = i;
                    break;
                }
            if (HDecayTo!=15)  throw lcio::DataNotAvailableException("Higgs not decay to tau!");
            
            
            iTauP = -99, iTauM = -99; // tau+ tau-
            // r_i1->clear(); r_i2->clear(); r_i3->clear();
            // r_iDis1->clear(); r_iDis2->clear(); r_iDis3->clear();
            for (int i=nMCP>50?50:(nMCP-1) ; i >= 0; i--){ 
            // find htt event from bottom to top, find where tau+/- appears the last time
                if (abs(mcp[i]->getPDG()) == 15 && mcp[i]->getDaughters().size() > 1){
                    if ( mcp[i]->getPDG() == 15 && iTauM<0 )
                        iTauM = i;
                    if ( mcp[i]->getPDG() == -15 && iTauP<0 )
                        iTauP = i;
                }
                if(iTauP>=0 && iTauM>=0) break;
            }
            if (iTauM<0 || iTauP<0)  throw lcio::DataNotAvailableException("Can't find MC tau");
            cout<<"Bingo!!!  Evt "<<nEvt<<endl;
            
            
            // find tau+/- decay mode. For each mode, identify muon/elec/pions... as decay product
            modeTauP = -99;  modeTauM = -99;
            t_p4_vtP->SetXYZT(0,0,0,0); t_p4_c1P->SetXYZT(0,0,0,0); t_p4_c2P->SetXYZT(0,0,0,0); t_p4_c3P->SetXYZT(0,0,0,0); t_p4_n1P->SetXYZT(0,0,0,0); t_p4_n2P->SetXYZT(0,0,0,0); t_p4_n3P->SetXYZT(0,0,0,0);     t_p4_vtM->SetXYZT(0,0,0,0); t_p4_c1M->SetXYZT(0,0,0,0); t_p4_c2M->SetXYZT(0,0,0,0); t_p4_c3M->SetXYZT(0,0,0,0); t_p4_n1M->SetXYZT(0,0,0,0); t_p4_n2M->SetXYZT(0,0,0,0); t_p4_n3M->SetXYZT(0,0,0,0);
    
            MCParticle* ivtP=nullptr, *ic1P=nullptr, *ic2P=nullptr, *ic3P=nullptr, *in1P=nullptr, *in2P=nullptr, *in3P=nullptr, *ivtM=nullptr, *ic1M=nullptr, *ic2M=nullptr, *ic3M=nullptr, *in1M=nullptr, *in2M=nullptr, *in3M=nullptr;
            
            vector<int> tdecay; // save PDG of decay objects
            vector<MCParticle*> tptr; // save pointer
            for (int l=0; l<=1; l++){
                int *iTau, *modeTau;
                MCParticle **ivt, **ic1, **ic2, **ic3, **in1, **in2, **in3;
                if(l==0){
                    iTau=&iTauM; modeTau=&modeTauM; ivt=&ivtM; ic1=&ic1M; ic2=&ic2M; ic3=&ic3M; in1=&in1M; in2=&in2M; in3=&in3M;
                }
                else{
                    iTau=&iTauP; modeTau=&modeTauP; ivt=&ivtP; ic1=&ic1P; ic2=&ic2P; ic3=&ic3P; in1=&in1P; in2=&in2P; in3=&in3P;
                }
                
                tdecay.clear();
                tptr.clear();
                for (unsigned ii=0; ii < mcp[*iTau]->getDaughters().size(); ii++) {
                    MCParticle* daup = mcp[*iTau]->getDaughters()[ii]; // ii-th decay object
                    if ( abs( daup->getPDG() )==213 || abs( daup->getPDG() )==113 || abs( daup->getPDG() )==223 || abs( daup->getPDG() )==20213){// rho+-0 omega(223) a1+(20213)
                        for(unsigned iii=0; iii<daup->getDaughters().size(); iii++){
                            if(daup->getDaughters()[iii]->getPDG()!=22) {// not gamma
                                tdecay.push_back( abs( daup->getDaughters()[iii]->getPDG() ) );
                                tptr.push_back( daup->getDaughters()[iii] );
                            }
                        }
                    }
                    else if ( daup->getPDG()!=22 ) {
                        tdecay.push_back( abs( daup->getPDG() ) );
                        tptr.push_back( daup );
                    }
                }
                
                if (tdecay.size() > 1) // sort tdecay and tptr in increasing order
                    for (unsigned ii=0; ii<tdecay.size()-1; ii++)
                        for (unsigned jj=tdecay.size()-1; jj>ii; jj--)
                            if (tdecay[jj] < tdecay[jj-1]){
                                swap(tdecay[jj], tdecay[jj-1]);
                                swap(tptr[jj], tptr[jj-1]);
                            }
                
                // 1 prong
                if(tdecay.size()==3 && tdecay[0]==11 && tdecay[1]==12 && tdecay[2]==16 ){
                    *modeTau = -1; // ta- > vt e- ve-
                    *ivt = tptr[2]; *ic1=tptr[0]; *in1=tptr[1];
                }
                else if(tdecay.size()==3 && tdecay[0]==13 && tdecay[1]==14 && tdecay[2]==16 ){
                    *modeTau = -2; // ta- > vt mu- vm-
                    *ivt = tptr[2]; *ic1=tptr[0]; *in1=tptr[1];
                }
                else if(tdecay.size()==2 && tdecay[0]==16 && tdecay[1]==211){
                    *modeTau = 1; // ta- > vt pi-
                    *ivt = tptr[0]; *ic1=tptr[1];
                }
                else if(tdecay.size()==3 && tdecay[0]==16 && tdecay[1]==111 && tdecay[2]==211){
                    *modeTau = 2; // ta- > vt pi- pi0
                    *ivt = tptr[0]; *ic1=tptr[2]; *in1=tptr[1];
                }
                else if(tdecay.size()==4 && tdecay[0]==16 && tdecay[1]==111 && tdecay[2]==111 && tdecay[3]==211){
                    *modeTau = 3; // ta- > vt pi- pi0 pi0
                    *ivt = tptr[0]; *ic1=tptr[3]; *in1=tptr[1]; *in2=tptr[2];
                }
                else if(tdecay.size()==5 && tdecay[0]==16 && tdecay[1]==111 && tdecay[2]==111 && tdecay[3]==111 && tdecay[4]==211){
                    *modeTau = 4; // ta- > vt pi- pi0 pi0 pi0
                    *ivt = tptr[0]; *ic1=tptr[4]; *in1=tptr[1]; *in2=tptr[2]; *in3=tptr[3];
                }
                
                // 3 prong
                else if(tdecay.size()==4 && tdecay[0]==16 && tdecay[1]==211 && tdecay[2]==211 && tdecay[3]==211){
                    *modeTau = 11; // ta- > 3h+-
                    *ivt = tptr[0]; *ic1=tptr[1]; *ic2=tptr[2]; *ic3=tptr[3];
                }
                else if(tdecay.size()==5 && tdecay[0]==16 && tdecay[1]==111 && tdecay[2]==211 && tdecay[3]==211 && tdecay[4]==211){
                    *modeTau = 12; // ta- > vt 3h+- pi0
                    *ivt = tptr[0]; *ic1=tptr[2]; *ic2=tptr[3]; *ic3=tptr[4]; *in1=tptr[1];
                }
                else if(tdecay.size()==6 && tdecay[0]==16 && tdecay[1]==111 && tdecay[2]==111 && tdecay[3]==211 && tdecay[4]==211 && tdecay[5]==211){
                    *modeTau = 13; // ta- > vt 3h+- pi0 pi0
                    *ivt = tptr[0]; *ic1=tptr[3]; *ic2=tptr[4]; *ic3=tptr[5]; *in1=tptr[1]; *in2=tptr[2];
                }
                else if(tdecay.size()==7 && tdecay[0]==16 && tdecay[1]==111 && tdecay[2]==111 && tdecay[3]==111 && tdecay[4]==211 && tdecay[5]==211 && tdecay[6]==211){
                    *modeTau = 14; // ta- > vt 3h+- pi0 pi0
                    *ivt = tptr[0]; *ic1=tptr[4]; *ic2=tptr[5]; *ic3=tptr[6]; *in1=tptr[1]; *in2=tptr[2]; *in3=tptr[3];
                }
                else  *modeTau = 100; //others (kaon)
                /* if(*modeTau==100){
                    for(int p=0; p<tdecay.size(); p++) cout<<tdecay[p]<<" ";
                    cout<<nFile<<" "<<nEvt<<endl;
                } */
            }
            if (ivtP)  *t_p4_vtP = TLorentzVector( ivtP->getMomentum(), ivtP->getEnergy() );
            if (ic1P)  *t_p4_c1P = TLorentzVector( ic1P->getMomentum(), ic1P->getEnergy() );
            if (ic2P)  *t_p4_c2P = TLorentzVector( ic2P->getMomentum(), ic2P->getEnergy() );
            if (ic3P)  *t_p4_c3P = TLorentzVector( ic3P->getMomentum(), ic3P->getEnergy() );
            if (in1P)  *t_p4_n1P = TLorentzVector( in1P->getMomentum(), in1P->getEnergy() );
            if (in2P)  *t_p4_n2P = TLorentzVector( in2P->getMomentum(), in2P->getEnergy() );
            if (in3P)  *t_p4_n3P = TLorentzVector( in3P->getMomentum(), in3P->getEnergy() );
            
            if (ivtM)  *t_p4_vtM = TLorentzVector( ivtM->getMomentum(), ivtM->getEnergy() );
            if (ic1M)  *t_p4_c1M = TLorentzVector( ic1M->getMomentum(), ic1M->getEnergy() );
            if (ic2M)  *t_p4_c2M = TLorentzVector( ic2M->getMomentum(), ic2M->getEnergy() );
            if (ic3M)  *t_p4_c3M = TLorentzVector( ic3M->getMomentum(), ic3M->getEnergy() );
            if (in1M)  *t_p4_n1M = TLorentzVector( in1M->getMomentum(), in1M->getEnergy() );
            if (in2M)  *t_p4_n2M = TLorentzVector( in2M->getMomentum(), in2M->getEnergy() );
            if (in3M)  *t_p4_n3M = TLorentzVector( in3M->getMomentum(), in3M->getEnergy() );
            
            
            // find stable particles (can be detected)
            iStable_v = new vector<int>();
            for(int i=0; i<nMCP; i++){
                TVector3 vex = mcp[i]->getVertex();
                TVector3 edp = mcp[i]->getEndpoint();
                if((vex.Perp()<1600 && fabs(vex.Z())<1000) && (edp.Perp()>1600 || fabs(edp.Z())>2000))
                    iStable_v->push_back( i );
            }
            
            
            // find TauJet segments that can be detected 
            t_p4_TauM->SetXYZT(0,0,0,0); t_p4_TauMJet->SetXYZT(0,0,0,0);
            t_p4_TauP->SetXYZT(0,0,0,0); t_p4_TauPJet->SetXYZT(0,0,0,0);
            vector<MCParticle*> ptr_TauPJet_v;
            vector<MCParticle*> ptr_TauMJet_v;
            std::queue<MCParticle*> ptr_TauP_q, ptr_TauM_q;
            ptr_TauP_q.push( dynamic_cast<MCParticle*>(col_MCP->getElementAt(iTauP)) ); 
            ptr_TauM_q.push( dynamic_cast<MCParticle*>(col_MCP->getElementAt(iTauM)) );
            while (ptr_TauP_q.empty()==0){
                MCParticle* ip = ptr_TauP_q.front();
                ptr_TauP_q.pop();
                TVector3 edp = ip->getEndpoint();
                if (edp.Perp()>1600 || fabs(edp.Z())>2000){
                    ptr_TauPJet_v.push_back( ip ); //yes can be detected!
                }
                else {
                    if (ip->getDaughters().size()==0) {
                        // cout<<"Wrong in TauPJet!! No daughters coming out?"<<nEvt<<" "<<ip<<endl;
                    }
                    else // can still decay to secondary particles.
                        for (unsigned k=0; k<ip->getDaughters().size(); k++)
                            ptr_TauP_q.push( ip->getDaughters()[k] );
                }
            }
            while (ptr_TauM_q.empty()==0){
                MCParticle* ip = ptr_TauM_q.front();
                ptr_TauM_q.pop();
                TVector3 edp = ip->getEndpoint();
                if (edp.Perp()>1600 || fabs(edp.Z())>2000){
                    ptr_TauMJet_v.push_back( ip );
                }
                else {
                    if (ip->getDaughters().size()==0) {
                        // cout<<"Wrong in TauMJet!! No daughters coming out?"<<nEvt<<" "<<ip<<endl;
                    }
                    else
                        for(unsigned k=0; k<ip->getDaughters().size(); k++)
                            ptr_TauM_q.push( ip->getDaughters()[k] );
                }
            } 
            std::sort(ptr_TauPJet_v.begin(), ptr_TauPJet_v.end()); // sort tau+ decay product in id increasing order
            std::sort(ptr_TauMJet_v.begin(), ptr_TauMJet_v.end());
            
            *t_p4_TauP = TLorentzVector( mcp[iTauP]->getMomentum(), mcp[iTauP]->getEnergy() ); // mc tau+ 4-momentum
            *t_p4_TauM = TLorentzVector( mcp[iTauM]->getMomentum(), mcp[iTauM]->getEnergy() );
            
            MCParticle* ptemp = nullptr;
            for (unsigned k=0; k<ptr_TauPJet_v.size(); k++) // mc tau+ jet 4-momentum
                if (ptemp!=ptr_TauPJet_v[k]) {
                    ptemp = ptr_TauPJet_v[k];
                    *t_p4_TauPJet += TLorentzVector( ptr_TauPJet_v[k]->getMomentum(), ptr_TauPJet_v[k]->getEnergy() );
                }
            ptemp = nullptr;
            for (unsigned k=0; k<ptr_TauMJet_v.size(); k++)
                if (ptemp!=ptr_TauMJet_v[k]) {
                    ptemp = ptr_TauMJet_v[k];
                    *t_p4_TauMJet += TLorentzVector( ptr_TauMJet_v[k]->getMomentum(), ptr_TauMJet_v[k]->getEnergy() );
                }
            cout<<"Tau+   "; PrintTLorentzVector(*t_p4_TauP); cout<<endl;
            cout<<"Tau+Jet"; PrintTLorentzVector(*t_p4_TauPJet); cout<<endl;
            cout<<"Tau-   "; PrintTLorentzVector(*t_p4_TauM); cout<<endl;
            cout<<"Tau-Jet"; PrintTLorentzVector(*t_p4_TauMJet); cout<<endl;
            
            
            // Z -> jj, find the last possible iZj1, iZj2
            t_p4_h->SetXYZT(0,0,0,0); t_p4_Z->SetXYZT(0,0,0,0); t_p4_ZJet->SetXYZT(0,0,0,0); 
            vector<MCParticle*> ptr_ZjJet_v;
            MCParticle *ptr_Zj1 = nullptr, *ptr_Zj2 = nullptr;
            for (int i=0; i<10; i++)
                if ( mcp[i]->getPDG()!=22 && mcp[i]->getPDG()!=25 ){
                    if( mcp[i]->getPDG()>0 )  {
                        ptr_Zj1 = dynamic_cast<MCParticle*>(mcp[i]);
                        ptr_Zj2 = dynamic_cast<MCParticle*>(mcp[i+1]);
                    }
                    break;
                }
            while(1){
                MCParticle* daup = ptr_Zj1->getDaughters()[0];
                if (daup->getPDG() == ptr_Zj1->getPDG()) { // skip the foremost generator particles
                    ptr_Zj1 = ptr_Zj1->getDaughters()[0];
                    ptr_Zj2 = ptr_Zj2->getDaughters()[0];
                }
                else if (daup->getPDG()>90){ // what? they merge again
                    if (daup->getDaughters()[0]->getPDG() > 0) {
                        ptr_Zj1 = daup->getDaughters()[0];
                        ptr_Zj2 = daup->getDaughters()[1];
                    }
                    else {
                        ptr_Zj1 = daup->getDaughters()[1];
                        ptr_Zj2 = daup->getDaughters()[0];
                    }
                    break;
                }
                else break;
            }
            // find Z jet segments
            queue<MCParticle*> ptr_Zj_q;
            ptr_Zj_q.push( ptr_Zj1 );
            ptr_Zj_q.push( ptr_Zj2 ); 
            while (ptr_Zj_q.empty()==0){
                MCParticle* ip = ptr_Zj_q.front();
                ptr_Zj_q.pop();
                if (((TVector3)ip->getEndpoint()).Perp()>1600 || fabs( ((TVector3)ip->getEndpoint()).Z())>2000){
                    ptr_ZjJet_v.push_back( ip );
                }
                else{
                    if(ip->getDaughters().size()>0){
                        for(unsigned k=0; k<ip->getDaughters().size(); k++){
                            ptr_Zj_q.push(ip->getDaughters()[k]);
                        }
                    }
                    // else  cout<<"Wrong in ZJet!! "<<i<<endl;
                }
            }
            std::sort(ptr_ZjJet_v.begin(), ptr_ZjJet_v.end());
            *t_p4_h = TLorentzVector( mcp[ih]->getMomentum(), mcp[ih]->getEnergy() ); // mc higgs 4-momentum
            *t_p4_Z = TLorentzVector( ptr_Zj1->getMomentum(), ptr_Zj1->getEnergy() ) + 
                      TLorentzVector( ptr_Zj2->getMomentum(), ptr_Zj2->getEnergy() );
            ptemp = nullptr;
            for (unsigned k=0; k<ptr_ZjJet_v.size(); k++) // mc tau+ jet 4-momentum
                if (ptemp!=ptr_ZjJet_v[k]) {
                    ptemp = ptr_ZjJet_v[k];
                    *t_p4_ZJet += TLorentzVector( ptr_ZjJet_v[k]->getMomentum(), ptr_ZjJet_v[k]->getEnergy() );
                }
            
            // cout<<"Higgs "; PrintTLorentzVector(*t_p4_h); cout<<" mass: "<<t_p4_h->M()<<endl;
            // cout<<"Z     "; PrintTLorentzVector(*t_p4_Z); cout<<" mass: "<<t_p4_Z->M()<<endl;
            // cout<<"Z Jet "; PrintTLorentzVector(*t_p4_ZJet); cout<<" mass: "<<t_p4_ZJet->M()<<endl;
            _outputTree->Fill();
        } 
        catch (lcio::DataNotAvailableException err) { /*  cout<<err.what()<<endl;*/ }
    } 
} 

void HiggsTruthAna::end()
{

    if (_outputTree) {

        TFile *tree_file = _outputTree->GetCurrentFile(); //just in case we switched to a new file
        tree_file->cd();
        tree_file->Write();
        tree_file->Close();
    }

}
