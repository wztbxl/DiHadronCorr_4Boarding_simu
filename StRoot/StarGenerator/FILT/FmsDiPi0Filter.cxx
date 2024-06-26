#include "FmsDiPi0Filter.h"

#include "StarGenerator/EVENT/StarGenParticle.h"
#include "StarGenerator/EVENT/StarGenEvent.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>

static const float ZFms    = 706.3 + 13;
static const float XFmsMin = 20;
static const float XFmsMax = 100;
static const float YFmsMin = 20;
static const float YFmsMax = 100;
static const float ETCUT   = 1.0;

static int ntot=0;
static int ngood=0;

using namespace std;
//_______________________________________________________________
FmsDiPi0Filter::FmsDiPi0Filter():StarFilterMaker("fmsDiPi0Filter"){
    cout<<"FMS DiPi0 filter is used!!!"<<endl;
}
//_______________________________________________________________
Int_t FmsDiPi0Filter::Filter( StarGenEvent *mEvent){
    ntot++;
    // Get a reference to the current event 
    StarGenEvent& event = *mEvent;
    
    //event.Print();
    if(event.GetNumberOfParticles() <= 0) {return kError;}
    
    TIter Iterator = event.IterAll();
    StarGenParticle *p = 0;
    vector<StarGenParticle*> forwardParticles;
    while( ( p = (StarGenParticle*)Iterator.Next() ) ){
	int pid = abs(p->GetId());
	// if(p->GetStatus() != 1)continue; //stable, but pi0 is not stable
	if(pid!=111) continue; 	     // not pi0
	//if((pid!=211) && (pid!=-211) && (pid!=321) && (pid!=-321) && (pid!=2212) && (pid!=-2212)) continue; //not charged hadron
	if(p->GetPz()<0.0) continue; // +z direction only
	if(p->pt()<ETCUT) continue;  // Et cut
	//project to FMS
	float x = fabs (p->GetVx()/10.0 + p->GetPx() / p->GetPz() * (ZFms - p->GetVz()/10.0));
	float y = fabs (p->GetVy()/10.0 + p->GetPy() / p->GetPz() * (ZFms - p->GetVz()/10.0));
	if(x>XFmsMax) continue; //outside
	if(y>XFmsMax) continue; //outside
	if(x<XFmsMin && y<YFmsMin) continue; //inside
	forwardParticles.push_back(p);	
    }
    unsigned int size=forwardParticles.size();
    if(size<2) return StarGenEvent::kReject;  //require 2 pi0
    return StarGenEvent::kAccept;

    //no mass cut
    /*
    for(unsigned int i=0; i<size-1; i++){
	StarGenParticle *p1=forwardParticles.at(i);
	for(unsigned int j=i + 1; j<size; j++){
	    StarGenParticle *p2=forwardParticles.at(j);
	    TLorentzVector pair = p1->momentum() + p2->momentum();
	    if(pair.M()>MASSCUT) {
		ngood++;
		cout << Form("FmsDiPi0Filter : N_Genearted=%6d  N_Accepted=%6d  R=%6.4f",
			     ntot,ngood,float(ngood)/float(ntot)) <<endl;
		return (StarGenEvent::kAccept | 0x08);
	    }
	}
    }
    return StarGenEvent::kReject;
    */
}
