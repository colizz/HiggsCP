#ifndef _ParticleList_hh_
#define _ParticleList_hh_

#include <string>
#include <iostream>
#include <fstream>
#include <marlin/Processor.h>
#include <TNtuple.h>
#include <TObject.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>

class TTree;

class ParticleList  : public marlin::Processor
{
   public:
   
    Processor*  newProcessor() { return new ParticleList ; }
    ParticleList();
    ~ParticleList() {};
    void init();
    void processEvent( LCEvent * evtP );
    void end();

   protected:
   
    std::ostream *_output;
    
    std::string _MCCollectionName;
    std::string _recoCollectionName;
    unsigned int _eventNr;
		
};
#endif




