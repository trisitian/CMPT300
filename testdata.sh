#!/bin/bash
#bash script to generate a bunch of test data, creates quite a few files, adjust loop if you need less

rm -rf testData
mkdir testData
cd testData

for i in {1..100}
do 
    mkdir $i
    cd $i
    j=1
    for (( j=1; j<$i; j++))
    do
        touch $j.txt
    done
    cd ..
done
