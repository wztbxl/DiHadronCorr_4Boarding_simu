#!/bin/bash

field=-5.0 # kG
echo "setting field=$field"
dir=/star/u/wangzhen/data01/QA/ColdQCD/DiHardon_Simulation
listdir=`pwd`
#using the following two lines for test
#daqdir=$HOME/public/daq
#listdir=$dir
#xml=embedContainer.xml
xml=embedding.xml
mkdir -p $dir/scripts
for runnumber in 1 
do
		outdir=$dir/output/$runnumber
		echo "********************"
		echo $outdir
		mkdir -p $outdir
		mkdir -p $outdir/log
		mkdir -p $outdir/err
		echo star-submit-template -template $xml -entities runnumber=$runnumber,field=$field,listdir=$listdir,outdir=$outdir
#		star-submit-template -template embedding.xml -entities runnumber=$runnumber,field=$field,listdir=$listdir,daqdir=$daqdir,outdir=$outdir
		star-submit-template -template $xml -entities runnumber=$runnumber,field=$field,listdir=$listdir,outdir=$outdir,
#                sleep 120
done
