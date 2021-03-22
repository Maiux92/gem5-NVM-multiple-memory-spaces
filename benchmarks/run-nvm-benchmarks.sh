#!/bin/bash
NVM=("SRAM" "FRAM" "MRAM" "RRAM" "STTRAM")
BENCHMARKS=("susan" "fft" "dijkstra")

DIR=`pwd`

if [ "$1" == "" ]; then
    echo "Please specify benchmark name"
    exit
else
    benchdir="$1"
    rm -rf results/$benchdir
    mkdir results/$benchdir
    
    echo "Compiling $benchdir"
    cd $benchdir
    make
    cd ..
fi

echo "Executing benchmarks for $benchdir"

cd ../gem5-20.1

for nvmtype in "${NVM[@]}"
do
    echo "Running benchmark $benchdir with $nvmtype"
    build/X86/gem5.opt configs/soc-project/NVM/conf-dynamic.py $nvmtype
    mv m5out ../benchmarks/results/$benchdir/$nvmtype
done

cd $DIR
