for ((i=0; i<=361; i++)); do
    echo ". /afs/ihep.ac.cn/users/l/licq/licq/ArborGeneralLICH/env.sh" > sub_$( echo $((10*$i)) | awk '{printf("%05d\n",$0)}' ).sh
    for ((j=1; j<=10; j++)); do
        # echo $((10*$i + $j))
        numAddZero=$( echo $((10*$i + $j)) | awk '{printf("%05d\n",$0)}' )
        # echo ${numAddZero}
        echo "Marlin /afs/ihep.ac.cn/users/l/licq/licq/ArborGeneralLICH/reco_001.xml  --global.LCIOInputFiles=/cefs/data/FullSim/CEPC250/CEPC_v1/higgs/qqh/E250-CDR_ws.Pqqh.eU.pU.${numAddZero}.slcio  --MyLCIOOutputProcessor.LCIOOutputFile=/afs/ihep.ac.cn/users/l/licq/licq/HiggsCP/sample/Reco.qqh.${numAddZero}.slcio" >> sub_$( echo $((10*$i)) | awk '{printf("%05d\n",$0)}' ).sh
    done
done
alias rms='rm -rf sub*.sh'
