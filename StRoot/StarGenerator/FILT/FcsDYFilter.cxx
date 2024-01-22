#include "FcsDYFilter.h"

#include "StarGenerator/EVENT/StarGenParticle.h"
#include "StarGenerator/EVENT/StarGenEvent.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>

static const float ZFCS    = 710.16 + 13.9 + 15.0;
static const float XFCSMin = 16.69;
static const float XFCSMax = 16.69 + 22.0 * 5.542;
static const float YFCS    = 34.0/2.0 * 5.542;
static const float ETCUT   = 1.0;
static const float MASSCUT = 2.5;

static int ntot=0;
static int ngood=0;

using namespace std;
//_______________________________________________________________
FcsDYFilter::FcsDYFilter():StarFilterMaker("fcsDYFilter"){
    cout<<"FCS DY filter is used!!!"<<endl;
}
//_______________________________________________________________
Int_t FcsDYFilter::Filter( StarGenEvent *mEvent){
    ntot++;
    // Get a reference to the current event 
    StarGenEvent& event = *mEvent;
    
    //event.Print();
    if(event.GetNumberOfParticles() <= 0) {return kError;}
    
    // apply DY conditions for events
    TIter Iterator = event.IterAll();
    StarGenParticle *p = 0;
    vector<StarGenParticle*> forwardParticles;
    while( ( p = (StarGenParticle*)Iterator.Next() ) ){
	if(p->GetStatus() != 1)continue;
	int pid = abs(p->GetId());
	if(pid!=11) continue; //  e+ & e- only
	if(p->GetPz()<0.0) continue; // +z direction only
	if(p->pt()<ETCUT) continue; 
	//simple box cut around FCS  (vertex here is in [mm])
	float x = fabs (p->GetVx()/10.0 + p->GetPx() / p->GetPz() * (ZFCS - p->GetVz()/10.0));
	if(x<XFCSMin || x>XFCSMax) continue;
	float y = fabs (p->GetVy()/10.0 + p->GetPy() / p->GetPz() * (ZFCS - p->GetVz()/10.0));
	if(y>YFCS) continue;	
	forwardParticles.push_back(p);	
    }
    unsigned int size=forwardParticles.size();
    if(size<2) return StarGenEvent::kReject;
    for(unsigned int i=0; i<size-1; i++){
	StarGenParticle *p1=forwardParticles.at(i);
	for(unsigned int j=i + 1; j<size; j++){
	    StarGenParticle *p2=forwardParticles.at(j);
	    TLorentzVector pair = p1->momentum() + p2->momentum();
	    if(pair.M()>MASSCUT) {
		ngood++;
		cout << Form("FcsDYFilter : N_Genearted=%6d  N_Accepted=%6d  R=%6.4f",
			     ntot,ngood,float(ngood)/float(ntot)) <<endl;
		return (StarGenEvent::kAccept | 0x08);
	    }
	}
    }
    return StarGenEvent::kReject;
}
