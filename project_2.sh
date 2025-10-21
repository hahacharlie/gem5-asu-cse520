#!/bin/bash

set -e  # Exit on any error

# Target files
O3CPU_FILE="src/cpu/o3/O3CPU.py"
FUNC_UNIT_CONFIG_FILE="src/cpu/o3/FuncUnitConfig.py"


###
# Problem 1
###

scons -j 12 build/ARM/gem5.opt
./build/ARM/gem5.opt -d Project2-Submission/Problem1/dijkstra-Conf1 ./configs/example/se.py --cpu-type=DerivO3CPU --caches --l2cache -c ./benchmarks/dijkstra/dijkstra_small -o ./benchmarks/dijkstra/input.dat

sed -i 's/fetchWidth = Param.Unsigned(8, "Fetch width")/fetchWidth = Param.Unsigned(4, "Fetch width")/' $O3CPU_FILE
sed -i 's/decodeWidth = Param.Unsigned(8, "Decode width")/decodeWidth = Param.Unsigned(4, "Decode width")/' $O3CPU_FILE
sed -i 's/renameWidth = Param.Unsigned(8, "Rename width")/renameWidth = Param.Unsigned(4, "Rename width")/' $O3CPU_FILE
sed -i 's/dispatchWidth = Param.Unsigned(8, "Dispatch width")/dispatchWidth = Param.Unsigned(4, "Dispatch width")/' $O3CPU_FILE
sed -i 's/issueWidth = Param.Unsigned(8, "Issue width")/issueWidth = Param.Unsigned(4, "Issue width")/' $O3CPU_FILE
sed -i 's/wbWidth = Param.Unsigned(8, "Writeback width")/wbWidth = Param.Unsigned(4, "Writeback width")/' $O3CPU_FILE
sed -i 's/commitWidth = Param.Unsigned(8, "Commit width")/commitWidth = Param.Unsigned(4, "Commit width")/' $O3CPU_FILE
sed -i 's/squashWidth = Param.Unsigned(8, "Squash width")/squashWidth = Param.Unsigned(4, "Squash width")/' $O3CPU_FILE
scons -j 12 build/ARM/gem5.opt
./build/ARM/gem5.opt -d Project2-Submission/Problem1/dijkstra-Conf2 ./configs/example/se.py --cpu-type=DerivO3CPU --caches --l2cache -c ./benchmarks/dijkstra/dijkstra_small -o ./benchmarks/dijkstra/input.dat

sed -i 's/fetchWidth = Param.Unsigned(4, "Fetch width")/fetchWidth = Param.Unsigned(2, "Fetch width")/' $O3CPU_FILE
sed -i 's/decodeWidth = Param.Unsigned(4, "Decode width")/decodeWidth = Param.Unsigned(2, "Decode width")/' $O3CPU_FILE
sed -i 's/renameWidth = Param.Unsigned(4, "Rename width")/renameWidth = Param.Unsigned(2, "Rename width")/' $O3CPU_FILE
sed -i 's/dispatchWidth = Param.Unsigned(4, "Dispatch width")/dispatchWidth = Param.Unsigned(2, "Dispatch width")/' $O3CPU_FILE
sed -i 's/issueWidth = Param.Unsigned(4, "Issue width")/issueWidth = Param.Unsigned(2, "Issue width")/' $O3CPU_FILE
sed -i 's/wbWidth = Param.Unsigned(4, "Writeback width")/wbWidth = Param.Unsigned(2, "Writeback width")/' $O3CPU_FILE
sed -i 's/commitWidth = Param.Unsigned(4, "Commit width")/commitWidth = Param.Unsigned(2, "Commit width")/' $O3CPU_FILE
sed -i 's/squashWidth = Param.Unsigned(4, "Squash width")/squashWidth = Param.Unsigned(2, "Squash width")/' $O3CPU_FILE
scons -j 12 build/ARM/gem5.opt
./build/ARM/gem5.opt -d Project2-Submission/Problem1/dijkstra-Conf3 ./configs/example/se.py --cpu-type=DerivO3CPU --caches --l2cache -c ./benchmarks/dijkstra/dijkstra_small -o ./benchmarks/dijkstra/input.dat

# Change back to original values

sed -i 's/fetchWidth = Param.Unsigned(2, "Fetch width")/fetchWidth = Param.Unsigned(8, "Fetch width")/' $O3CPU_FILE
sed -i 's/decodeWidth = Param.Unsigned(2, "Decode width")/decodeWidth = Param.Unsigned(8, "Decode width")/' $O3CPU_FILE
sed -i 's/renameWidth = Param.Unsigned(2, "Rename width")/renameWidth = Param.Unsigned(8, "Rename width")/' $O3CPU_FILE
sed -i 's/dispatchWidth = Param.Unsigned(2, "Dispatch width")/dispatchWidth = Param.Unsigned(8, "Dispatch width")/' $O3CPU_FILE
sed -i 's/issueWidth = Param.Unsigned(2, "Issue width")/issueWidth = Param.Unsigned(8, "Issue width")/' $O3CPU_FILE
sed -i 's/wbWidth = Param.Unsigned(2, "Writeback width")/wbWidth = Param.Unsigned(8, "Writeback width")/' $O3CPU_FILE
sed -i 's/commitWidth = Param.Unsigned(2, "Commit width")/commitWidth = Param.Unsigned(8, "Commit width")/' $O3CPU_FILE
sed -i 's/squashWidth = Param.Unsigned(2, "Squash width")/squashWidth = Param.Unsigned(8, "Squash width")/' $O3CPU_FILE


###
# Problem 2
###

scons -j 12 build/ARM/gem5.opt
./build/ARM/gem5.opt -d Project2-Submission/Problem2/dijkstra-Conf1 ./configs/example/se.py --cpu-type=DerivO3CPU --caches --l2cache -c ./benchmarks/dijkstra/dijkstra_small -o ./benchmarks/dijkstra/input.dat

sed -i '/class IntALU(FUDesc):/,/count = 6/{s/count = 6/count = 4/}' $FUNC_UNIT_CONFIG_FILE
sed -i '/class IntMultDiv(FUDesc):/,/count = 2/{s/count = 2/count = 1/}' $FUNC_UNIT_CONFIG_FILE
sed -i '/class FP_ALU(FUDesc):/,/count = 4/{s/count = 4/count = 4/}' $FUNC_UNIT_CONFIG_FILE
sed -i '/class FP_MultDiv(FUDesc):/,/count = 2/{s/count = 2/count = 1/}' $FUNC_UNIT_CONFIG_FILE
scons -j 12 build/ARM/gem5.opt
./build/ARM/gem5.opt -d Project2-Submission/Problem2/dijkstra-Conf2 ./configs/example/se.py --cpu-type=DerivO3CPU --caches --l2cache -c ./benchmarks/dijkstra/dijkstra_small -o ./benchmarks/dijkstra/input.dat

sed -i '/class IntALU(FUDesc):/,/count = 4/{s/count = 4/count = 8/}' $FUNC_UNIT_CONFIG_FILE
sed -i '/class IntMultDiv(FUDesc):/,/count = 1/{s/count = 1/count = 4/}' $FUNC_UNIT_CONFIG_FILE
sed -i '/class FP_ALU(FUDesc):/,/count = 4/{s/count = 4/count = 6/}' $FUNC_UNIT_CONFIG_FILE
sed -i '/class FP_MultDiv(FUDesc):/,/count = 1/{s/count = 1/count = 4/}' $FUNC_UNIT_CONFIG_FILE
scons -j 12 build/ARM/gem5.opt
./build/ARM/gem5.opt -d Project2-Submission/Problem2/dijkstra-Conf3 ./configs/example/se.py --cpu-type=DerivO3CPU --caches --l2cache -c ./benchmarks/dijkstra/dijkstra_small -o ./benchmarks/dijkstra/input.dat

# Change back to original values

sed -i '/class IntALU(FUDesc):/,/count = 8/{s/count = 8/count = 6/}' $FUNC_UNIT_CONFIG_FILE
sed -i '/class IntMultDiv(FUDesc):/,/count = 4/{s/count = 4/count = 2/}' $FUNC_UNIT_CONFIG_FILE
sed -i '/class FP_ALU(FUDesc):/,/count = 6/{s/count = 6/count = 4/}' $FUNC_UNIT_CONFIG_FILE
sed -i '/class FP_MultDiv(FUDesc):/,/count = 4/{s/count = 4/count = 2/}' $FUNC_UNIT_CONFIG_FILE
