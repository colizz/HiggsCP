### Not yet finished, wait a moment...
------------------
# `HiggsCP` -- A Higgs CP analysis tool on CEPC

`HiggsCP` is an analysis tool based on `iLCSoft` (basically `Marlin`) and `ROOT`, providing a framework with novel methods on precise Higgs CP measurement via h -> \tau\tau channel on the future collider CEPC.

The recommanded workflow would be:
-  Get the input files, i.e. signal/backgroud `.slcio` files containing MC truth data and reconstructed information.
-  Setup and build the package.
-  Run the package via `Marlin` in several stage to extract & define variables.
-  Apply further analysis & draw plots using `ROOT`.

Detailed steps will be introduced as follow.

## Get input files
The origial input files are stantard `.slcio` files stored in `sample` folder. In IHEP environment you could simply find the files here:
```
/cefs/higgs/licq/HiggsCP/sample/Reco_qqh_*.slcio
```

They contain important collections like `MCParticles` and `ArborUFO`, i.e. the MC truth information for each event, and the reconstructed results obtained from [*Arbor* -- a specific CEPC-based Particle Flow Algorithm](https://arxiv.org/abs/1403.4784). You can also generate more personalized input files through the procedures below, or you could safely skip this for the first run.

#### Generate input files (if you like)

First you need to have simulated MC files. You can find all freshly prepared signal/background channel under `/cefs/data/FullSim/CEPC250/CEPC_v1/`. Then simply apply the *Arbor* framework (a copy can be accessed from `/cefs/higgs/licq/ArborGeneralLICH`) :
>- Load Marlin environment `source env.sh`.
>- Establish steering file `reco_001.xml`: specify paths of input MC file under `LCIOInputFiles` and output file `LCIOOutputFile`
>- Run `Marlin reco_001.xml` and wait for years.

## Setup
Like normal [`Marlin` framework](https://github.com/iLCSoft/Marlin), codes are established under `src/`, head files under `include/`, and a steering file `steer.xml` is required to define Marlin Processor. To setup the package, run the code under a machine where `Marlin` is already installed.

```sh
git clone https://github.com/colizz/HiggsCP.git HiggsCP
cd HiggsCP
mkdir build run && cd build

# install iLCSoft environment, this command is only availble on IHEP,
# please check the cmake file. Otherwise you need to download essential
# [iLCSoft](https://github.com/iLCSoft) software and modify the directories in your cmake file.
cmake -C ../ILCSoft.cmake .. 

# build the package
make install
```
Before the first run, please specify your own absolute path in `loadLDD.sh` and `steer.xml` by substituting paths containing `.../licq/HiggsCP/...`. Then try this:
```ssh
source ../loadLDD.sh  ## load libraries, please always remember to do this
Marlin steer.xml
```
Here's a trick: you can limit the events number for the first try. `Marlin` provides an easy way to overwrite the processor parameter. Just redefine them in command line:
```ssh
source ../loadLDD.sh
Marlin steer.xml  --global.MaxRecordNumber=5  --MyExportEvt.TreeOutputFile=/the/path/you/like
```

After some while you will find output root files under ......

**Note: Comments and descriptions for each variable can be found in corresponding `src/*.cc` file where they are defined, please have a look.**


To be continue...
------------

Draft:
The possible bkg:
- ee->ZZ, Z->tautau, Z->ll(jj)
- ee->Zh, Z->tautau, h->bb
- ee->Zh, Z->tautau, h->lvlv(l=e,mu,tau)

for ee->ZZ, dominant bkg in CPEC terminology are
- zz_sl_tau_up(down)  200889
- zz_l_taumu  88577 -->/cefs/higgs/xuyue/Reco_SMbkg/Reco_zz/taumu
- sze_l_tau  758207



