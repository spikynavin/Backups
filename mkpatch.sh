#!/bin/bash
#name=uboot-imx
#if [[ -e $name.patch || -L $name.patch ]] ; 
#then
#    i=0
#    while [[ -e $i-$name.patch || -L $i-$name.patch ]] ; 
#do
#        let i++
#    done
#    name=$i-$name
#fi
#touch -- "$name".patch
#diff -Naur a/ b/ > "$name".patch
version="v2015.04_4.1.15_1.0.0_ga"
diff -Naur a/ b/ > `ls * | wc -l`u-boot-imx"$version".patch
