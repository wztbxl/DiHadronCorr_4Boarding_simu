#define genevents_cxx
#include "genevents.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TFile.h>
#include <iostream>   // std::cout
#include <algorithm>  // std::sort
#include <vector>     // std::vector

static const double PI    = TMath::Pi();
static const double twoPI = PI*2.0;
enum {kNPtBin=6};
  //static float ptcut[kNPtBin+1]={0.1,0.2,0.3,0.5,1.0,1.5,2.0};
static float ptcut[kNPtBin+1] = {0.5,1.0,1.5,2.0,2.5,3.0,5.0};
Int_t ptbin(float pt){
        if(pt < ptcut[0]) return -1;
        for(int i=0; i<kNPtBin; i++){
                if(pt >= ptcut[i] && pt < ptcut[i+1]) return i;
        }
        return kNPtBin;
}

struct myclass {
  bool operator() (TLorentzVector i, TLorentzVector j) { return i.Pt()>j.Pt();}
} myobject;

TLatex* drawLatex(Double_t x, Double_t y, TString text, Int_t textFont, Double_t textSize, Int_t colorIndex){
  TLatex *latex = new TLatex(x,y,text);
  latex->SetNDC();
  latex->SetTextFont(textFont);
  latex->SetTextSize(textSize);
  latex->SetTextColor(colorIndex);
  latex->Draw("same");
  return latex;
}

TH1D *getLogXBin(TString histname, TString histaxis, 
		int bin_num, double bin_min, double bin_max){
		const int aa = bin_num+1;
		double x_edge[aa];
		for(int i(0); i<=bin_num; i++){
			double step = (log10(bin_max)-log10(bin_min))/bin_num;
			x_edge[i]   = pow(10, log10(bin_min)+i*step);
		}

		TH1D *htemp = new TH1D(histname, histaxis, bin_num, x_edge);
		return htemp;
}

TH2D *getLogXYBin(TString histname, TString histaxis, 
		int bin_num_X, double bin_min_X, double bin_max_X,
		int bin_num_Y, double bin_min_Y, double bin_max_Y){
		const int aa = bin_num_X+1;
		double x_edge[aa];
		  //double x_edge[bin_num_X+1];
		for(int i(0); i<=bin_num_X; i++){
			double step = (log10(bin_max_X)-log10(bin_min_X))/bin_num_X;
			x_edge[i]   = pow(10, log10(bin_min_X)+i*step);
		}
		const int bb = bin_num_Y+1;
		double y_edge[bb];
		  //double y_edge[bin_num_Y+1];
		for(int i(0); i<=bin_num_Y; i++){
			double step = (log10(bin_max_Y)-log10(bin_min_Y))/bin_num_Y;
			y_edge[i]   = pow(10, log10(bin_min_Y)+i*step);
		}


		TH2D *htemp = new TH2D(histname, histaxis, bin_num_X, x_edge, bin_num_Y, y_edge);
		return htemp;
}

double wrapAround(double phi){
    double res = phi;
    while(res>=1.5*PI) {res-=twoPI;}
    while(res<-0.5*PI) {res+=twoPI;}
    return res;
}

void genevents::Loop()
{
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  gStyle->SetOptDate(0);
  //   In a ROOT session, you can do:
  //      Root > .L genevents.C
  //      Root > genevents t
  //      Root > t.GetEntry(12); // Fill t data members with entry number 12
  //      Root > t.Show();       // Show values of entry 12
  //      Root > t.Show(16);     // Read and show values of entry 16
  //      Root > t.Loop();       // Loop on all entries
  //

  //     This is the loop skeleton where:
  //    jentry is the global entry number in the chain
  //    ientry is the entry number in the current Tree
  //  Note that the argument to GetEntry must be:
  //    jentry for TChain::GetEntry
  //    ientry for TTree::GetEntry and TBranch::GetEntry
  //
  //       To read only selected branches, Insert statements like:
  // METHOD1:
  //    fChain->SetBranchStatus("*",0);  // disable all branches
  //    fChain->SetBranchStatus("branchname",1);  // activate branchname
  // METHOD2: replace line
  //    fChain->GetEntry(jentry);       //read all branches
  //by  b_branchname->GetEntry(ientry); //read only this branch
  if (fChain == 0) return;
  TH1D* mmass[kNPtBin];
  TH1D* mpt[kNPtBin][kNPtBin];
  TH1D* mphi[kNPtBin][kNPtBin];
  TH1D* mphi_pTAll;
  TH1D* mX1[kNPtBin][kNPtBin];
  TH1D* mX2[kNPtBin][kNPtBin];
  TH1D* mPthat[kNPtBin][kNPtBin];
  int nTriggerPi0_pT[kNPtBin][kNPtBin];
  //THnSparse *test[kNPtBin][kNPtBin];
  TGaxis::SetMaxDigits(3);
  char c[100];
  for(int i=0; i<kNPtBin; i++)
  {
    sprintf(c,"pi0mass_%1d",i); mmass[i] = new TH1D(c,c,100,-1000,1000);
    for(int j=0; j<=i; j++)
	{
      sprintf(c,"pi0avgpT_%1d%1d",i,j); mpt[i][j]     = new TH1D(c,c,100,-4,4.1);
      sprintf(c,"pi0deltaphi_%1d%1d",i,j); mphi[i][j] = new TH1D(c,c,100,-0.5*PI,1.5*PI);
      sprintf(c,"x1_%1d%1d",i,j); mX1[i][j]           = getLogXBin(c,c,32,1e-5,1.1);
      sprintf(c,"x2_%1d%1d",i,j); mX2[i][j]           = getLogXBin(c,c,32,1e-5,1.1);
      sprintf(c,"pthat_%1d%1d",i,j); mPthat[i][j]     = getLogXBin(c,c,32,0.05,12);
    }
  }
  mphi_pTAll  = new TH1D("pi0deltaphi_allpT",";#delta #phi;counts",100,-0.5*PI,1.5*PI);
  TH1D*    nPion       = new TH1D("nPion","nPion",100,0,100);
  Long64_t nentries    = fChain->GetEntriesFast();
  Long64_t nbytes      = 0, nb = 0;
  int      nEvents     = 0;
  int      nTriggerPi0 = 0;

  cout << "total events  = " << (long long)nentries << endl;
  for (Long64_t jentry=0; jentry<nentries;jentry++) 
  {
    std::vector<TLorentzVector> Pi0;
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    if (jentry%100000==0) cout<<jentry<<" events done"<<endl;
	//why 3456?

    double id4 = mParticles_mId[3];
    double id5 = mParticles_mId[4];
    double id6 = mParticles_mId[5];
    double id7 = mParticles_mId[6];

	int nPi0 = 0;
	for(int i=1;i<mNumParticles+1;i++){
        TLorentzVector vPi0;
	    // if ((abs(mParticles_mId[i])==2212)||(abs(mParticles_mId[i])==211)||(abs(mParticles_mId[i])==321)){// charge hadron pi/k/p
	    if (mParticles_mId[i]==111){ //pi0
		vPi0.SetPxPyPzE(mParticles_mPx[i],mParticles_mPy[i],mParticles_mPz[i],mParticles_mEnergy[i]);
		if(vPi0.E()<=1||vPi0.Pt()<=1||vPi0.Eta()>=4||vPi0.Eta()<2.5)continue;  //fms
		  // if(vPi0.E()<=1||vPi0.Pt()<=1||vPi0.Eta()>=1||vPi0.Eta()<-1)continue; //mid rapidity
		if(vPi0.Pt()<1.e-5) continue;
		Pi0.push_back(vPi0);
		nPi0++;
		}
	}//particle loop
  //QA for the vector
  // for (auto i : Pi0)
  // {
  // 	cout << "pT = " << i.Pt() << endl;
  // }


  //sort pi0 by pT
if(nPi0>1){
	std::sort(Pi0.begin(), Pi0.end(), myobject);
	  // std::sort(Pi0.begin(), Pi0.end(), [](TLorentzVector* a, TLorentzVector* b) {
	  // return b.Pt() < a.Pt();
	  // });
           }

int nPion_tigger = 0;
if (nPi0>1){//evts with at least two pi0s
	int ptbin0, ptbin1;
        for(unsigned int i=0; i<Pi0.size(); i++){ //loop over 1st pair
		TLorentzVector vPi1   = Pi0[i];
		               ptbin0 = ptbin(vPi1.Pt());
		if(ptbin0>5)continue;
		mmass[ptbin0]->Fill(vPi1.M());
		for(unsigned int j=i+1; j<Pi0.size(); j++){
			TLorentzVector vPi2   = Pi0[j];
			               ptbin1 = ptbin(vPi2.Pt());
			if(ptbin0>5||ptbin1>5)continue;
			double avgpt = (vPi1.Pt()+vPi2.Pt())/2.;
			double phi   = wrapAround(vPi1.DeltaPhi(vPi2));
			mphi_pTAll->Fill(phi);
			mpt[ptbin0][ptbin1]->Fill(avgpt);
			mphi[ptbin0][ptbin1]->Fill(phi);
   double x2       = (pow(TMath::E(),-1*vPi1.Eta())*vPi1.Pt()+pow(TMath::E(),-1*vPi2.Eta())*vPi2.Pt())/200.;
   double xparton1 = xParton1>xParton2?xParton1:xParton2;
   double xparton2 = xParton1<xParton2?xParton1:xParton2;
                        if(xparton2<x2/5.)continue;
			mX1[ptbin0][ptbin1]->Fill(xparton1);
			mX2[ptbin0][ptbin1]->Fill(xparton2);
			mPthat[ptbin0][ptbin1]->Fill(ptHat);
		}//acco
	   nTriggerPi0++;
	   nPion_tigger++;
           } //trig
    //cout<<" Pi1 - Pi2 "<<vPi1.Pt()-vPi2.Pt()<<endl;
 }  //npi0>1
      Pi0.clear();
	  nPion->Fill(nPion_tigger);
	  nEvents++;
   }//event loop
	cout << "total events  = " << nEvents << endl;
  //Draw
double xx = 0.18, yy = 0.84, dy = 0.09, size = 0.08;

TCanvas* cvs = new TCanvas("cvs", "cvs", 1200, 800);
cvs->Divide(kNPtBin-2,kNPtBin-2);
TLegend *leg = new TLegend(0.15,yy-5*dy,0.9,yy-2*dy);
int N_pair;
for(int i=2; i<kNPtBin; i++){
cout<<mmass[i]->GetEntries()<<endl;
	for(int j=1; j<i; j++){
		  //if(i==5&&j==4)continue;
		TVirtualPad *pad = cvs->cd((j-1)*(kNPtBin-2)+i-1);
		pad->SetRightMargin(0.001);
		pad->SetLeftMargin(0.14);
		pad->SetBottomMargin(0.15);
		pad->SetTopMargin(0.07);
		  //pad->SetLogx();
		pad->Draw();
		  //mX1[i][j]->Scale(1./mX1[i][j]->GetEntries());
		  //mX2[i][j]->Scale(1./mX2[i][j]->GetEntries());
		mpt[i][j]->GetXaxis()->SetLabelSize(0.06);
		mpt[i][j]->GetYaxis()->SetLabelSize(0.06);
		mpt[i][j]->GetYaxis()->SetNdivisions(505);
		  //mpt[i][j]->SetLineColor(2);
		  //mpt[i][j]->SetLineStyle(3);
		mpt[i][j]->Draw();
		N_pair = mpt[i][j]->GetEntries();
		drawLatex(xx, yy, Form("p^{trig}_{T}=%3.1f-%3.1f GeV/c",ptcut[i],ptcut[i+1]), 42,size,1);
                drawLatex(xx, yy-1.3*dy, Form("p^{asso}_{T}=%3.1f-%3.1f GeV/c",ptcut[j],ptcut[j+1]), 42,size,1);
		drawLatex(xx, yy-3*dy, Form("N^{h^{+/-} pair}=%3.0d",N_pair), 42,size,1);
		mphi[i][j]->Scale(1./nTriggerPi0);
		  // mphi[i][j]->Scale(1./(mmass[i]->GetEntries()));
	}
}
mpt[2][1]->GetYaxis()->SetTitle("counts");
mpt[2][1]->GetYaxis()->SetTitleOffset(0.9);
mpt[2][1]->GetYaxis()->SetTitleSize(0.08);
mpt[5][4]->GetXaxis()->SetTitle("p^{avg}_{T} [GeV]");
mpt[5][4]->GetXaxis()->SetTitleOffset(0.8);
mpt[5][4]->GetXaxis()->SetTitleSize(0.08);
mpt[5][4]->GetXaxis()->CenterTitle();

cvs -> cd(14);
drawLatex(xx, yy-1*dy, "#sqrt{s} = 200 GeV", 42, size+0.03,kBlue);
drawLatex(xx, yy-2.6*dy, "p+p#rightarrowh^{+/-}h^{+/-}X",42, size+0.03,kBlue);
drawLatex(xx, yy-4.2*dy, "2.6<#eta^{h^{+/-}}<4.0", 42, size+0.03,kBlue);
drawLatex(xx, yy-5.8*dy, "p_{T}^{h^{+/-}}>0.2 GeV/c", 42, size+0.03,kBlue);

name = Form("%s/avg_pt_h.pdf",dir);
cvs->SaveAs(name);

TCanvas* cvsphi = new TCanvas("cvsphi", "cvsphi", 1200, 800);
cvsphi->Divide(kNPtBin-2,kNPtBin-2);
for(int i=2; i<kNPtBin; i++){
	for(int j=1; j<i; j++){
		TVirtualPad *pad = cvsphi->cd((j-1)*(kNPtBin-2)+i-1);
		pad->SetRightMargin(0.001);
		pad->SetLeftMargin(0.14);
		pad->SetBottomMargin(0.15);
		pad->SetTopMargin(0.07);
		pad->Draw();
		mphi[i][j]->GetXaxis()->SetLabelSize(0.06);
		mphi[i][j]->GetYaxis()->SetLabelSize(0.06);
		mphi[i][j]->GetYaxis()->SetNdivisions(505);
		  //mpt[i][j]->SetLineColor(2);
		  //mpt[i][j]->SetLineStyle(3);
		mphi[i][j]->Draw();
		mphi[i][j]->SetTitle(Form("p^{trig}_{T}=%3.1f-%3.1f GeV/c",ptcut[i],ptcut[i+1]));
		drawLatex(xx, yy, Form("p^{trig}_{T}=%3.1f-%3.1f GeV/c",ptcut[i],ptcut[i+1]), 42,size,1);
                drawLatex(xx, yy-1.3*dy, Form("p^{asso}_{T}=%3.1f-%3.1f GeV/c",ptcut[j],ptcut[j+1]), 42,size,1);
	}
}
mphi[2][1]->GetYaxis()->SetTitle("counts");
mphi[2][1]->GetYaxis()->SetRangeUser(0,0.09);
mphi[2][1]->GetYaxis()->SetTitleOffset(0.9);
mphi[2][1]->GetYaxis()->SetTitleSize(0.08);
mphi[5][4]->GetXaxis()->SetTitle("#Delta#phi [rad]");
mphi[5][4]->GetXaxis()->SetTitleOffset(0.8);
mphi[5][4]->GetXaxis()->SetTitleSize(0.08);
mphi[5][4]->GetXaxis()->CenterTitle();

cvsphi -> cd(14);
drawLatex(xx, yy-1*dy, "#sqrt{s} = 200 GeV", 42, size+0.03,kBlue);
drawLatex(xx, yy-2.6*dy, "p+p#rightarrowh^{+/-}h^{+/-}X",42, size+0.03,kBlue);
drawLatex(xx, yy-4.2*dy, "2.6<#eta^{h^{+/-}}<4.0", 42, size+0.03,kBlue);
drawLatex(xx, yy-5.8*dy, "p_{T}^{h^{+/-}}>0.2 GeV/c", 42, size+0.03,kBlue);
drawLatex(xx, yy-7.8*dy, "k_{T}=0, No PS, p_{T}^{frag}=0",42, size+0.03,kBlue);

name = Form("%s/phi_h_case2_Tune100_fmsDipi0.pdf",dir);
cvsphi->SaveAs(name);


TCanvas* cvsx = new TCanvas("cvsx", "cvsx", 1200, 800);
cvsx->Divide(kNPtBin-2,kNPtBin-2);
double x1mean, x2mean, x1peak, x2peak, q2mean, q2peak;
for(int i=2; i<kNPtBin; i++){
        for(int j=1; j<i; j++){
                TVirtualPad *pad = cvsx->cd((j-1)*(kNPtBin-2)+i-1);
                pad->SetRightMargin(0.001);
                pad->SetLeftMargin(0.14);
                pad->SetBottomMargin(0.15);
                pad->SetTopMargin(0.07);
                pad->SetLogx();
                pad->Draw();
                mX1[i][j]->GetXaxis()->SetLabelSize(0.06);
                mX1[i][j]->GetYaxis()->SetLabelSize(0.06);
                mX1[i][j]->GetYaxis()->SetNdivisions(505);
                mX2[i][j]->SetLineColor(2);
                mX1[i][j]->SetLineStyle(3);
                mX1[i][j]->Draw();
                mX2[i][j]->Draw("same");
                x1mean = mX1[i][j]->GetMean();
                x2mean = mX2[i][j]->GetMean();
                x1peak = mX1[i][j]->GetBinCenter(mX1[i][j]->GetMaximumBin());
                x2peak = mX2[i][j]->GetBinCenter(mX2[i][j]->GetMaximumBin());
                drawLatex(xx, yy, Form("p^{trig}_{T}=%3.1f-%3.1f GeV/c",ptcut[i],ptcut[i+1]), 42,size,1);
                drawLatex(xx, yy-1.3*dy, Form("p^{asso}_{T}=%3.1f-%3.1f GeV/c",ptcut[j],ptcut[j+1]), 42,size,1);
                drawLatex(xx, yy-2.6*dy, Form("#LTx_{1}#GT=%3.3f",x1mean), 42,size,1);
                drawLatex(xx, yy-3.8*dy, Form("#LTx_{2}#GT=%3.3f",x2mean), 42,size,2);

        }
}
mX1[2][1]->GetYaxis()->SetTitle("counts");
mX1[2][1]->GetYaxis()->SetTitleOffset(0.9);
mX1[2][1]->GetYaxis()->SetTitleSize(0.08);
mX1[5][4]->GetXaxis()->SetTitle("x");
mX1[5][4]->GetXaxis()->SetTitleOffset(0.78);
mX1[5][4]->GetXaxis()->SetTitleSize(0.08);
mX1[5][4]->GetXaxis()->CenterTitle();

cvsx -> cd(14);
drawLatex(xx, yy-1*dy, "#sqrt{s} = 200 GeV", 42, size+0.03,kBlue);
drawLatex(xx, yy-2.6*dy, "p+p#rightarrowh^{+/-}h^{+/-}X",42, size+0.03,kBlue);
drawLatex(xx, yy-4.2*dy, "2.6<#eta^{h^{+/-}}<4.0", 42, size+0.03,kBlue);
drawLatex(xx, yy-5.8*dy, "p_{T}^{h^{+/-}}>0.2 GeV/c", 42, size+0.03,kBlue);

name = Form("%s/x.pdf",dir);
cvsx->SaveAs(name);

TCanvas* cvsQ2 = new TCanvas("cvsQ2", "cvsQ2", 1200, 800);
cvsQ2->Divide(kNPtBin-2,kNPtBin-2);
for(int i=2; i<kNPtBin; i++){
        for(int j=1; j<i; j++){
                TVirtualPad *pad1 = cvsQ2->cd((j-1)*(kNPtBin-2)+i-1);
                pad1->SetRightMargin(0.001);
                pad1->SetLeftMargin(0.14);
                pad1->SetBottomMargin(0.15);
                pad1->SetTopMargin(0.07);
                pad1->SetLogx();
                pad1->Draw("e");
                q2mean = mPthat[i][j]->GetMean();
                q2peak = mPthat[i][j]->GetBinCenter(mPthat[i][j]->GetMaximumBin());
                mPthat[i][j]->GetXaxis()->SetLabelSize(0.06);
                mPthat[i][j]->GetYaxis()->SetLabelSize(0.06);
                mPthat[i][j]->GetYaxis()->SetNdivisions(505);
                mPthat[i][j]->Draw();
                drawLatex(xx, yy, Form("p^{trig}_{T}=%3.1f-%3.1f GeV/c",ptcut[i],ptcut[i+1]), 42,size,1);
                drawLatex(xx, yy-1.4*dy, Form("p^{asso}_{T}=%3.1f-%3.1f GeV/c",ptcut[j],ptcut[j+1]), 42,size,1);
                drawLatex(xx, yy-2.8*dy, Form("#LTQ^{2}#GT=%3.1f GeV^{2}",q2mean), 42,size,1);
        }
}
mPthat[2][1]->GetYaxis()->SetTitle("counts");
mPthat[2][1]->GetYaxis()->SetTitleSize(0.08);
mPthat[2][1]->GetYaxis()->SetTitleOffset(0.9);
mPthat[5][4]->GetXaxis()->SetTitle("Q^{2} [GeV^{2}]");
mPthat[5][4]->GetXaxis()->SetLabelOffset(0.000);
mPthat[5][4]->GetXaxis()->SetTitleOffset(0.82);
mPthat[5][4]->GetXaxis()->SetTitleSize(0.08);
mPthat[5][4]->GetXaxis()->CenterTitle();

cvsQ2 -> cd(14);
drawLatex(xx, yy-1*dy, "#sqrt{s} = 200 GeV", 42, size+0.03,kBlue);
drawLatex(xx, yy-2.6*dy, "p+p#rightarrowh^{+/-}h^{+/-}X",42, size+0.03,kBlue);
drawLatex(xx, yy-4.2*dy, "2.6<#eta^{h^{+/-}}<4.0", 42, size+0.03,kBlue);
drawLatex(xx, yy-5.8*dy, "p_{T}^{h^{+/-}}>0.2 GeV/c", 42, size+0.03,kBlue);

name = Form("%s/Q2.pdf",dir);
cvsQ2->SaveAs(name);

       name    = Form("%s/output.root",dir);
TFile* outfile = new TFile(name.Data(),"recreate");
for(int i=0; i<kNPtBin; i++){
sprintf(c,"pi0mass_%1d",i); mmass[i] = new TH1D(c,c,100,-1000,1000);
for(int j=0; j<=i; j++){
	mpt[i][j]->Write();
	mphi[i][j]->Write();
	mX1[i][j]->Write();
    mX2[i][j]->Write();
    mPthat[i][j]->Write();
	}
}
mphi_pTAll->Scale(1./nTriggerPi0);
mphi_pTAll->Write();
nPion->Write();



}
