For kT, reference from Hekki
https://arxiv.org/pdf/1309.6963.pdf, Fig5
pp: kT = 0.25 GeV/c
pAu: kT = 1.0 GeV/c

Default:
MSTP(91) was 1.000000
MSTP(61) was 2.000000
	= 0, off
	= 1, on
	= 2, on for QCD radiation in hadronic events and QED radiation in leptonic ones.
	= 2, on for QCD and QED radiation in hadronic events and QED radiation in leptonic ones.
MSTP(71) was 1.000000
PARJ(21) was 0.330000


0606:
1.	myPythia6->SetMSTP(91,1);//kT cannot be 0, so we set it to be 1 first
2.	myPythia6->SetMSTP(61,0);//No ISPS
3.	myPythia6->SetMSTP(71,0);//No FSPS
4.	myPythia6->SetPARJ(21,0);//No pT frag

0612:
1. change tune to 100
2. Kt=ISP=FSP=frag pT=0 
