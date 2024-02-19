// macro to instantiate the Geant3 from within
// STAR  C++  framework and get the starsim prompt
// To use it do
//  root4star starsim_pythia6.C

class St_geant_Maker;
St_geant_Maker *geant_maker = 0;

class St_db_Maker;
St_db_Maker *db_maker = 0;

class StarGenEvent;
StarGenEvent   *event       = 0;

class StarPrimaryMaker;
StarPrimaryMaker *primaryMaker = 0;

class StarPythia6;
StarPythia6 *pythia6 = 0;

class StarFilterMaker;
class FcsDYFilter;
class FcsJetFilter;
StarFilterMaker *dyfilter =0;
StarFilterMaker *dybgfilter=0;
StarFilterMaker *jetfilter=0;

TString LHAPDF_DATA_PATH="/star/u/akio/lhapdf";

// ----------------------------------------------------------------------------
void geometry( TString tag, Bool_t agml=true )
{
  TString cmd = "DETP GEOM "; cmd += tag;
  if ( !geant_maker ) geant_maker = (St_geant_Maker *)chain->GetMaker("geant");
  geant_maker -> LoadGeometry(cmd);
  //  if ( agml ) command("gexec $STAR_LIB/libxgeometry.so");
}
// ----------------------------------------------------------------------------
void command( TString cmd )
{
  if ( !geant_maker ) geant_maker = (St_geant_Maker *)chain->GetMaker("geant");
  geant_maker -> Do( cmd );
}
// ----------------------------------------------------------------------------
void trig( Int_t n=1 )
{
  for ( Int_t i=0; i<n; i++ ) {
    chain->Clear();
    chain->Make();
  }
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Pythia6( TString mode="pp:DY", Int_t tune=100)
{
  
  //  gSystem->Load( "libStarGeneratorPoolPythia6_4_23.so" );
  if ( LHAPDF_DATA_PATH.Contains("afs") ) {
     cout << "WARNING: LHAPDF_DATA_PATH points to an afs volume" << endl << endl;
     cout << "         You are advised to copy the PDF files you need into a local" << endl;
     cout << "         directory and set the LHAPDF_DATA_PATH to point to it."      << endl;
  }

  gSystem->Setenv("LHAPDF_DATA_PATH", LHAPDF_DATA_PATH.Data() );

  gSystem->Load( "/opt/star/$STAR_HOST_SYS/lib/libLHAPDF.so");
  gSystem->Load( "libPythia6_4_28.so");

  pythia6 = new StarPythia6("pythia6");
  pythia6->SetFrame("CMS", 200.0 );
  pythia6->SetBlue("proton");
  pythia6->SetYell("proton");
  pythia6->PyTune(tune);
  PySubs_t &pysubs = pythia6->pysubs();
  if ( mode == "pp:minbias" ) {
      pysubs.msel = 2;
  }else if( mode == "pp:qcd" ) {
      pysubs.msel = 1;
  }else if( mode == "pp:DY" ) {
      pysubs.msel = 0;
      pysubs.msub(1)=1; //qq->ll & qq->Z  
  }

  pythia6->Init();
  PyPars_t &pypars = pythia6->pypars();
  printf("PARP(90) was %f, replacing it with 0.213\n",pypars.parp(90));
  pypars.parp(90)=0.213;

  primaryMaker->AddGenerator(pythia6);
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void runPythia( Int_t nevents=3, Int_t run=1, char* particle="fmsdipi0", float vz=0.0, Int_t tune=100){ 
  cout<<"Random seed : "<<run<<endl;

  gROOT->ProcessLine(".L bfc.C");{
    TString simple = "y2015 geant gstar agml usexgeom ";
    bfc(0, simple );
  }
  
  gSystem->Load( "libVMC.so");
  gSystem->Load( "StarGeneratorUtil.so" );
  gSystem->Load( "StarGeneratorEvent.so" );
  gSystem->Load( "StarGeneratorBase.so" );
  gSystem->Load( "StarGeneratorFilt.so");
  gSystem->Load( "libMathMore.so"   );  
  gSystem->Load( "xgeometry.so"     );


  db_maker = (St_db_Maker *)chain->GetMaker("db");
  if(db_maker){
      db_maker->SetAttr("blacklist", "tpc");
      db_maker->SetAttr("blacklist", "svt");
      db_maker->SetAttr("blacklist", "ssd");
      db_maker->SetAttr("blacklist", "ist");
      db_maker->SetAttr("blacklist", "pxl");
      db_maker->SetAttr("blacklist", "pp2pp");
      db_maker->SetAttr("blacklist", "ftpc");
      db_maker->SetAttr("blacklist", "emc");
      db_maker->SetAttr("blacklist", "eemc");
      db_maker->SetAttr("blacklist", "mtd");
      db_maker->SetAttr("blacklist", "pmd");
      db_maker->SetAttr("blacklist", "tof");
  }

  // Setup RNG seed and map all ROOT TRandom here
  StarRandom::seed( run );
  StarRandom::capture();
  
  //
  // Create the primary event generator and insert it
  // before the geant maker
  //
  //  StarPrimaryMaker *
  primaryMaker = new StarPrimaryMaker();
  {
      primaryMaker -> SetFileName( Form("pythia.%s.vz%d.run%d.root",particle,int(vz),run));
      chain -> AddBefore( "geant", primaryMaker );
  }

  //
  // Setup an event generator
  //
  TString proc(particle);
  if(proc.Contains("mb")){
      Pythia6("pp:minbias", tune);
  }else if(proc.Contains("jet")){  //JET
      Pythia6("pp:qcd", tune);
      jetfilter = (StarFilterMaker *)new FcsJetFilter();
      primaryMaker->AddFilter(jetfilter);
  }else if( proc.Contains("dy") && !proc.Contains("dybg") ){ // DY signal
      Pythia6("pp:DY", tune);
      dyfilter = (StarFilterMaker *)new FcsDYFilter();
      primaryMaker->AddFilter(dyfilter);
  }else if(proc.Contains("dybg")){ //DY background via QCD
      Pythia6("pp:qcd", tune);
      dybgfilter = new FcsDYBGFilter();
      primaryMaker->AddFilter(dybgfilter);
  }else if(proc.Contains("fmsdipi0")){ //FMS dipi0
      Pythia6("pp:qcd", tune);
      fmsdipi0filter = new FmsDiPi0Filter();
      primaryMaker->AddFilter(fmsdipi0filter);
  }
//run2
//primaryMaker->SetAttr("FilterKeepAll", int(0) );
//run3
//primaryMaker->SetAttr("FilterKeepHeader", int(0));
//run4
primaryMaker->SetAttr("FilterKeepAll", int(0) );
primaryMaker->SetAttr("FilterKeepHeader", int(0));
  //
  // Setup cuts on which particles get passed to geant for
  // simulation.  
  //
  // If ptmax < ptmin indicates an infinite ptmax.
  // ptmin will always be the low pT cutoff.
  //
  //                    ptmin  ptmax
  primaryMaker->SetPtRange  (0.0,  -1.0);         // GeV
  //
  // If etamax < etamin, there is no cut in eta.
  // otherwise, particles outside of the specified range are cut.
  //
  //                    etamin etamax
  primaryMaker->SetEtaRange ( 0.0, +10.0 );
  //
  //  phirange will be mapped into 0 to 2 pi internally.
  //
  //                    phimin phimax
  primaryMaker->SetPhiRange ( 0., TMath::TwoPi() );
  
    // Setup a realistic z-vertex distribution:
  //   x = 0 gauss width = 1mm
  //   y = 0 gauss width = 1mm
  //   z = 0 gauss width = 30cm
  // 
  primaryMaker->SetVertex( 0., 0., vz );
  //primaryMaker->SetSigma( 0.1, 0.1, 30.0 );
  primaryMaker->SetSigma( 0., 0., 0. );

  //
  // Initialize primary event generator and all sub makers
  //
  primaryMaker -> Init();

  //chenging pypars(90) 
  if(tune==100){
      PyPars_t &pypars = pythia6->pypars();
      //printf("PARP(90) was %f, replacing it with 0.213\n",pypars.parp(90));
      //pypars.parp(90)=0.213;
/*****************************Broending test************************/
TPythia6 *myPythia6 = new TPythia6();
//myPythia6->SetPARP(90,0.213);//original from Akio
//1. change kT
//2. change initial stat parton shower
//3. change final state parton shower
//4. change pT frag
//1.1 set kT = 0, No IS PS, No FS PS, No pT frag
double default_PARP_91 = myPythia6->GetPARP(91);
double default_MSTP_61 = myPythia6->GetMSTP(61);
double default_MSTP_71 = myPythia6->GetMSTP(71);
double deafult_PARJ_21 = myPythia6->GetPARJ(21);
myPythia6->SetMSTP(91,0);//kT
// myPythia6->SetMSTP(91,1);//kT
// myPythia6->SetPARP(91,0.4); //Zheng Liang, https://arxiv.org/pdf/1403.2413.pdf
// myPythia6->SetPARP(91,0.25); //For kT, reference from Hekki
// myPythia6->SetMSTP(61,2);//ISPS
myPythia6->SetMSTP(61,0);//no ISPS
myPythia6->SetMSTP(71,0);//no FSPS
myPythia6->SetMSTJ(13,0);//no pT frag
myPythia6->SetPARJ(21,0);//pT frag width
printf("PARP(91) was %f, replacing it with %f\n",default_PARP_91, myPythia6->GetPARP(91));
printf("MSTP(61) was %f, replacing it with %f\n",default_MSTP_61, myPythia6->GetMSTP(61));
printf("MSTP(71) was %f, replacing it with %f\n",default_MSTP_71, myPythia6->GetMSTP(71));
printf("PARJ(21) was %f, replacing it with %f\n",deafult_PARJ_21, myPythia6->GetPARJ(21)); 
  }
  //
  // Setup geometry and set starsim to use agusread for input
  //
  //geometry("fwddev1a");
  //geometry("ftsref6a");
  //geometry("dev2021");
  //geometry("sitrver0");
  geometry("y2015a");
  command("gkine -4 0");
  command(Form("gfile o pythia.%s.vz%d.run%d.fzd",particle,int(vz),run));
  
  //
  // Trigger on nevents
  //
  trig( nevents );

  pythia6->PyStat(1);

  command("call agexit");  // Make sure that STARSIM exits properly
}
// ----------------------------------------------------------------------------

