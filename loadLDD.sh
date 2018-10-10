#! /bin/bash

unset MARLIN_DLL
export MARLIN_DLL=$(cd $(dirname $BASH_SOURCE); pwd)/lib/libHiggsTruthAna.so
echo "Set MARLIN_DLL = $MARLIN_DLL"