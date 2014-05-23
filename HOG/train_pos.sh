#!/bin/bash
rm -f svm_pos.txt 
touch svm_pos.txt 

#for f in $(readlink -f ~/INRIA_FIXED/train_64x128_H96/pos/*)
for f in $(readlink -f ~/pedestrians128x64/*)
do
	echo "$f"
	#./bin/HOG_BB "$f" 3 3 67 131 1 >> svm_pos.txt
	./bin/HOG_BB "$f" 0 0 63 127 1 >> svm_pos.txt
done

#echo '============================='
#echo 'Classifying'
#echo '============================='
#svm_classify svm_retrain.txt svm_model.txt svm_retrain_output.txt
