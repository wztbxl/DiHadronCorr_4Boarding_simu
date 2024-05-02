
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
//#include "sys/types.h"
//#include "dirent.h"
#include "math.h"
#include "string.h"

#include "TROOT.h"
#include "TFile.h"
#include "TString.h"
#include "TChain.h"
#include "TMath.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2D.h"
#include "TH3.h"
#include "TH3D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TTimer.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TUnixSystem.h"
#include "TVector2.h"
#include "TVector2.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TPDF.h"
#include "TPaveStats.h"
#include "TMath.h"
#include "TLegend.h"
#include "TList.h"
#include "TLatex.h"
#include "TColor.h"

#include "/Users/wztbxl/Documents/macros/function.C"

using namespace std;

static const double PI=TMath::Pi();

void DrawDiffCase()
{
    cout << "-3-3-3" << endl;

    vector<TString> colors = {"#104680","#6DADD1","#F6B293","#B72230","#6D011F"};
    cout << "-2-2-2" << endl;
    vector<int> markers = {8,4,21,25,34,28};

    map< TString, TFile*> mFiles;
    map< TString, TH1D*> mTH1;
    map< TString, TH2D*> mTH2;
    cout << "-1-1-1" << endl;

    vector<TString> tag_name;
    vector<TString> File_name;
    vector<TString> TH1_name;
    vector<TString> TH1_title;
    cout << "000000" << endl;

    // tag_name.push_back("case1");
    // TH1_title.push_back("no k_{T}, IS off, FS off, p_{T}^{frag} off");
    // tag_name.push_back("case2");
    // TH1_title.push_back("k_{T} = 0.25, IS off, FS off, p_{T}^{frag} off");
    // tag_name.push_back("case3");
    // TH1_title.push_back("no k_{T}, IS on, FS off, p_{T}^{frag} off");
    // tag_name.push_back("case8");
    // TH1_title.push_back("k_{T} = 0.25, IS on, FS off, p_{T}^{frag} off");
    // // tag_name.push_back("case7");
    // TH1_title.push_back("k_{T} = 1, IS off, FS off, p_{T}^{frag} off");

    //checking kT scan, now pT set to 0.36
    tag_name.push_back("case6_pA_0p4");
    TH1_title.push_back("k_{T} = 0.4, IS on, FS on, p_{T}^{frag} on, p_{T}^{frag} = 0.36");
    tag_name.push_back("case6_pA_1");
    TH1_title.push_back("k_{T} = 1, IS on, FS on, p_{T}^{frag} on, p_{T}^{frag} = 0.36");
    tag_name.push_back("case6_pp_0p25");
    TH1_title.push_back("k_{T} = 0.25, IS on, FS on, p_{T}^{frag} on, p_{T}^{frag} = 0.36");
    tag_name.push_back("case6_pp_0p25_STAR");
    TH1_title.push_back("k_{T} = 0.25, IS on, FS on, p_{T}^{frag} on, p_{T}^{frag} = 0.36, STAR Tune");
    cout << "111111" << endl;

    for (size_t i = 0; i < tag_name.size(); i++)
    {
        TString name;
        TString title;
        File_name.push_back(Form("/Users/wztbxl/Desktop/SDUPwg/STAR/ColdQCD/Dihadron_Boarding_simu/%s/output.root",tag_name[i].Data()));
        TFile* tem = new TFile(Form("/Users/wztbxl/Desktop/SDUPwg/STAR/ColdQCD/Dihadron_Boarding_simu/%s/output.root",tag_name[i].Data()));
        // name = Form("pi0deltaphi_allpT");
        // mTH1[tag_name[i]] = (TH1D*)tem->Get("pi0deltaphi_allpT");
        //read the bin p^{trig}_{T}=2.0-2.5 GeV/c, p^{asso}_{T}=1.0-1.5 GeV/c
        name = Form("pi0deltaphi_31");
        mTH1[tag_name[i]] = (TH1D*)tem->Get("pi0deltaphi_31");
        name = Form("pi0deltaphi_31_%s",tag_name[i].Data());
        mTH1[tag_name[i]]->SetName(name.Data());
        // mTH1[tag_name[i]]->Scale(1./mTH1[tag_name[i]]->GetEntries());
    }
    
    TCanvas* c1 = new TCanvas("c1","c1",1600,1200);
    TH2D* frame = new TH2D("frame","frame",100,-0.5*PI,1.5*PI,100,0,0.003);
    frame->SetTitle(";#Delta #phi;counts");
    frame->SetStats(0);
    frame->GetYaxis()->SetRangeUser(0,0.045);
    frame->Draw();
    TLegend* leg = new TLegend(0.6,0.7,0.89,0.89);
    leg->SetLineColor(0);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);

    // mTH1[tag_name[0]]->GetYaxis()->SetRangeUser(0,0.06);
    // mTH1[tag_name[0]]->Draw("pel");
    for (size_t i = 0; i < mTH1.size(); i++)
    {
        cout << "222222" << endl;
        setHisto(mTH1[tag_name[i]],markers[i],1,TColor::GetColor(colors[i]),TColor::GetColor(colors[i]),1);
        leg->AddEntry(mTH1[tag_name[i]],TH1_title[i].Data(),"pl");
        mTH1[tag_name[i]]->Draw("pelsame");
    }
    leg->DrawClone("same");
    cout << "333333" << endl;


    
}
