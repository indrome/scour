#!/bin/bash
rm -f svm_retrain.txt
touch svm_retrain.txt

for f in $(readlink -f ~/INRIA_subset/Train/neg/*)
do
	echo "$f"
	./bin/HOG_sliding "$f" 
	cat sliding_output.txt >> svm_retrain.txt
done

echo '============================='
echo 'Classifying'
echo '============================='
svm_classify svm_retrain.txt svm_model.txt svm_retrain_output.txt

echo '============================='
echo 'Filtering output'
echo '============================='
rm -f svm_retrain_filtered.txt
touch svm_retrain_filtered.txt
for line in $(cat svm_retrain_output.txt | awk '{if($1 > 0.5) print NR}')
do
	cat svm_retrain.txt | awk -v l="$line" 'NR==l {print $0; exit}' >> svm_retrain_filtered.txt
done

