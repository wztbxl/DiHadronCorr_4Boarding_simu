# Code for the Di hadron correlation boarding simulation.
## Download the code to your local path
```
git clone git@github.com:wztbxl/DiHadronCorr_4Boarding_simu.git
```


## how to run the code 
The main macro is 
runPythia.C

For kT, reference from Hekki
https://arxiv.org/pdf/1309.6963.pdf, Fig.5

pp: $k_T$ = 0.25 GeV/c

pAu: $k_T$ = 1.0 GeV/c

From reference, https://arxiv.org/pdf/1403.2413.pdf

$k_T$ = 0.4 GeV/c

Default:

MSTP(91) default is 1.000 (D = 1) (C) primordial $k_T$ distribution in hadron.

	= 0 no primordial $k_T$ 
 	= 1 Gaussian, with given in PARP(91), upper cut-off in PARP(93).
 	= 2 exponential, width given in PARP(92), upper cut-off in PARP(93). Not available in the new model.

PARJ(91) is the width of Gaussian $k_T$ distribution, defaule setting is 2 GeV/c

MSTP(61) : (D = 2) (C) master switch for initial-state QCD and QED radiation.

	= 0, off
	= 1, on
	= 2, on for QCD radiation in hadronic events and QED radiation in leptonic ones.
	= 2, on for QCD and QED radiation in hadronic events and QED radiation in leptonic ones.
 
MSTP(71) : (D = 1) (C) master switch for final-state QCD and QED radiation.
	
 	= 0 off
 	= 1 on

MSTJ(13) : (D = 0) generation of transverse momentum for endpoint quark(s) of single quark jet or qq jet system (in multijet events no endpoint transverse momentum
is ever allowed for).

	= 0 : no transverse momentum for endpoint quarks.
	= 1 : endpoint quarks obtain transverse momenta like ordinary qq pairs produced	in the field (see PARJ(21)); for 2-jet systems the endpoints obtain balancing transverse momenta.

PARJ(21) : (D = 0.36 GeV) corresponds to the width $\sigma$ in the Gaussian $p_x$ and $p_y$ transverse momentum distributions for primary hadrons. 


0612:
1. change tune to 100
2. Kt=ISP=FSP=frag pT=0

2024.01.29

For different simulation setting :

case1 :
- no $k_T$, IS off, FS off, $p_{T}^{frag}$ off

case2 :
- $k_T$ = 0.25, IS off, FS off, $p_{T}^{frag}$ off

case3 :
- no $k_T$, IS on(2), FS off, $p_{T}^{frag}$ off

case4 : 
- no $k_T$, IS on(2), FS on(1), $p_{T}^{frag}$ off

case 5:
- no $k_T$, IS on(2), FS on(1), $p_{T}^{frag}$ on(default)

case 6:
- $k_T$ = 0.25, IS on(2), FS on(1), $p_{T}^{frag}$ on(default)

case7 :
- $k_T$ = 1., IS off, FS off, $p_{T}^{frag}$ off

case8 :
- $k_T$ = 0.25, IS on, FS off, $p_{T}^{frag}$ off

case9 :
- $k_T$ = 0.4, IS on, FS off, $p_{T}^{frag}$ off

case10 :
- $k_T$ = 1, IS on, FS off, $p_{T}^{frag}$ off


## Job submit log
2024.2.19 
- DiHardon_Simulation submit case1
- DiHardon_Simulation_2 submit case2

