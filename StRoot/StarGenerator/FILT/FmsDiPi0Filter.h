/*! \class FmsDiPi0Filter

  This class is just used to filter non DrellYan events
  The important bits are:
    *change it so that it inherits from StarFilterMaker, not StMCFilter
    *replace RejectGE() with Filter()
    *fix the filter function so that it takes a StarGenEvent rather than a StarGenParticleMaster
*/

#ifndef STAR_FmsDiPi0Filter
#define STAR_FmsDiPi0Filter

#include <vector>
#include <string>
#include "StarGenerator/FILT/StarFilterMaker.h"
#include "StarGenerator/EVENT/StarGenEvent.h"

class StarGenParticleMaster;
class StarGenParticle;
class StarGenEvent;

class FmsDiPi0Filter : public StarFilterMaker
{
 public:
  FmsDiPi0Filter(); ///constructor
  virtual ~FmsDiPi0Filter(){;};///destructor
  Int_t Filter( StarGenEvent *mEvent );

  ClassDef(FmsDiPi0Filter,0);  
};

#endif
