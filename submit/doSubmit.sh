#!/bin/bash
for ((i=0; i<=361; i++)); do
    hep_sub sub_$( echo $((10*$i)) | awk '{printf("%05d\n",$0)}' ).sh
done
