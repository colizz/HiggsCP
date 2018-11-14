#!/bin/bash

isample=1
sampleName=(qqh zz_sl0tau_down zz_sl0tau_up)
sampleNEvt=(3620 1623 1004)
slcioName=(higgs/qqh/E250-CDR_ws.Pqqh.eU.pU  4fermions/E250.Pzz_sl.e0.p0.whizard195/Pzz_sl0tau_down.e0.p0  4fermions/E250.Pzz_sl.e0.p0.whizard195/Pzz_sl0tau_up.e0.p0)
slcioNameSuffix=(.slcio  _sim.slcio  _sim.slcio)

subfileDir=sub_jobs/${sampleName[${isample}]}
mkdir -p ${subfileDir}

for ((i=0; i<=$(( ${sampleNEvt[${isample}]}/10 )); i++)); do
# for ((i=0; i<=1; i++)); do
    echo ". /afs/ihep.ac.cn/users/l/licq/licq/ArborGeneralLICH/env.sh" > ${subfileDir}/sub_$( echo $((10*$i)) | awk '{printf("%05d\n",$0)}' ).sh
    for ((j=1; j<=10; j++)); do
        numAddZero=$( echo $((10*$i + $j)) | awk '{printf("%05d\n",$0)}' )
        echo "Marlin /afs/ihep.ac.cn/users/l/licq/licq/ArborGeneralLICH/reco_001.xml  --global.LCIOInputFiles=/cefs/data/FullSim/CEPC250/CEPC_v1/${slcioName[$isample]}.${numAddZero}${slcioNameSuffix[$isample]}  --MyLCIOOutputProcessor.LCIOOutputFile=/cefs/higgs/licq/sample/${sampleName[$isample]}/Reco.${sampleName[$isample]}.$( echo $((10*$i + $j)) | awk '{printf("%05d\n",$0)}' ).slcio"  >> \
           ${subfileDir}/sub_$( echo $((10*$i)) | awk '{printf("%05d\n",$0)}' ).sh
        # echo "/afs/ihep.ac.cn/users/l/licq/licq/HiggsCP/sample/Reco.qqh.${numAddZero}.slcio" >> files.txt
    done
done
alias rms='rm -rf sub_*.sh'
