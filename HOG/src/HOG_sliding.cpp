#include "HOG.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "svm.h"

#define DEBUG

using namespace std;
using namespace cv;

static struct svm_model* model;

float predict( vector<float>& features ){
	
	double predicted_label;
	struct svm_node* x = (struct svm_node*)malloc(sizeof(struct svm_node)*features.size()+1);

	if(!x){
		fprintf(stderr, "Unable to allocate memory for SVM nodes\n");
		exit(1);
	}

	int i;
	for( i = 0; i < (int)features.size(); i++ ){
		x[i].value = features[i];
		x[i].index = i+1;
	}
	x[i].index = -1;

	predicted_label = svm_predict(model,x);
	
	return predicted_label;

}

int main(int argc, char* argv[]) {

	char* img_path = NULL;

	//if(argc != 3){
	//	printf("usage: %s path_to_image path_to_model\n",argv[0]);
	//}
	//else {

		if(strnlen(argv[1],255) < 255) {
			img_path = argv[1];
		}
		else {
			printf("path_to_image is too long\n");
			return -1;
		}

	//}

	//if((model=svm_load_model(argv[2]))==0){
	//	fprintf(stderr,"can't open model file %s\n",argv[2]);
	//	exit(1);
	//}

	// Read image and write row pointers
	Mat img = imread(img_path, CV_LOAD_IMAGE_GRAYSCALE);

	equalizeHist( img, img );

	//Mat img_tq;
	//Mat img_t;
	//Mat img_h;
	//Mat img_q;
	//resize(img, img_tq, Size(), 0.75, 0.75);
	//resize(img, img_h, Size(), 0.5, 0.5);
	//resize(img, img_t, Size(), 0.3, 0.3);
	//resize(img, img_q, Size(), 0.25, 0.25);
	//img = img_h;

	if( img.data == NULL ){
		printf("ERROR: Image could not be read\n");
		return -1;
	}

	int M = img.rows;
	int N = img.cols;

	//int gap_x = 32;
	//int gap_y= 64;
	int gap_x = 8;
	int gap_y= 8;


	int win_width = NUM_BLOCK_X*CELL_WIDTH; 
	int win_height = NUM_BLOCK_Y*CELL_HEIGHT;


	uchar* data = img.data;
	uchar** raw_data = (uchar**)malloc(sizeof(uchar*)*M);

	for(int i = 0; i < M; i++) {
		raw_data[i] = &data[i*N];	
	}

	vector<float> features;

	//const char* cmd = "~/src/libsvm/svm-scale -l 0 -u 1 -s range test.txt > test.txt.scale";
	FILE* fp = fopen("sliding_output.txt","w");

	int coord[((N-win_width-1)/gap_x+1)* ((M-win_height-1)/gap_y+1)][2];
	int index = 0;

	for( int i = 1; i < M-win_height-1; i += gap_y ){
		for( int j = 1; j < N-win_width-1; j += gap_x ){


			assert(j+win_width < N );
			assert(i+win_height < M );
			float** hist_list = compute_cell_histogram( j, i, img ); 		
			features = block_normalize( hist_list );
			//float result = predict(features);

			//if( result >= 1.0 )
			//{
			//	printf("========================================\n");
			//	printf("Person detected in: %d %d %f\n",i,j,result);
			//	printf("========================================\n");
			//}
			//else {
			//	cout << "Reuslt " << i << " "<< j << ": " << result << endl;
			//}
			//fprintf(fp,"0 ");
			//print_features( features, fp); 
			fprintf(fp,"0 ");
			print_features( features, fp); 
	
			coord[index][0] = j;
			coord[index][1] = i;
			index++;

		}
	}
	printf("index: %d\n",index);

	fclose(fp);
// /*
	system("svm_classify -v 3 sliding_output.txt svm_model.txt svm_output.txt");
	
	FILE* fp_out = fopen("svm_output.txt","r");
	float score = 0.0;

	index = 0;	

	namedWindow("HOG", WINDOW_AUTOSIZE );
	cvtColor( img, img, CV_GRAY2BGR );

	while( fscanf(fp_out,"%f\n", &score) > 0){
		if( score > 0.0 ){
			int xmin = coord[index][0];
			int ymin = coord[index][1];
			int xmax = xmin+win_width;
			int ymax = ymin+win_height;
			printf("%f %d %d %d %d\n",score, xmin, ymin, xmax, ymax );
			rectangle( img, cvPoint(xmin,ymin), cvPoint(xmax,ymax), CV_RGB(255,0,0),1,8);

		}
		index++;
	}

	fclose(fp);


	imshow("HOG", img);
	waitKey(0);
 //*/
	return 0;
}
