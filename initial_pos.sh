#!/bin/bash
jobid=0

#filenames=("commands_21Rotations.txt" "commands_21Rotations.txt" "commands_21Rotations_Exchange.txt" "commands_21Rotations_Exchange.txt")
#filenames=( "commands_21Rotations.txt" "commands_21Rotations_Exchange.txt" )
#solvers=(0 1 0 1)

#filenames=("commands_d1_10_OtherEnd_21Rotations_10T.txt" "commands_d1_10_OtherEnd_21Rotations_Exchange_10T.txt" )
#filenames=("commands_d1_10_OtherEnd_5Rotations_Exchange_10T_Combination_xytheta_minusminuspi.txt")
#filenames=( "commands_21Rotations.txt" "commands_21Rotations_Exchange.txt" )
filenames=("commands_N10_V2.txt")
solvers=(0)

filename=${filenames[$jobid]}
solver=${solvers[jobid]}

echo "txt $filename solver $solver"
while read line; do
# reading each line and pass arguments to chrono
		echo "Line: $line"
		./my_demo ${solver} ${line}
done < $filename

#output_freqsolver=1
#while read line; do
## reading each line and pass arguments to chrono
#		echo "Line: $line"
#		../build/my_demo ${solver} ${line}
#done < $filename
#
#filename='commands_21Rotations_Exchange.txt'
#solver=0
#while read line; do
## reading each line and pass arguments to chrono
#		echo "Line: $line"
#		../build/my_demo ${solver} ${line}
#done < $filename
#
#solver=1
#while read line; do
## reading each line and pass arguments to chrono
#		echo "Line: $line"
#		../build/my_demo ${solver} ${line}
#done < $filename
