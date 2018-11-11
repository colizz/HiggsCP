#!/bin/bash
read -p "Sample: # " st
read -p "Events: # " skip

Marlin jobOption/run_ParticleList.xml  --global.Verbosity=WARNING \
  --global.LCIOInputFiles=/cefs/higgs/licq/sample/Reco.qqh.$( echo ${st} | awk '{printf("%05d\n",$0)}' ).slcio \
  --global.SkipNEvents=${skip} && \
less out.particle_list.txt

