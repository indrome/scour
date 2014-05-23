#include "HOG.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG

using namespace std;
using namespace cv;


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
	//Mat img = imread(img_path, CV_LOAD_IMAGE_GRAYSCALE);
	Mat img = imread(img_path);

	int num_levels = 6.0;
	vector<Mat> multi_scale_images(num_levels); multi_scale_images[0] = img;
	vector<float> scales(num_levels); scales[0] = 1.0;
	float delta_s = (1.0-0.4)/(float)num_levels;


	for(int i = 1; i < num_levels; i++){
		scales[i] = scales[i-1]-delta_s;	
		resize( multi_scale_images[i-1], multi_scale_images[i], Size(), scales[i],scales[i]);
	}


	if( img.data == NULL ){
		printf("ERROR: Image could not be read\n");
		return -1;
	}



	int gap_x = 16;
	int gap_y= 32;
	//int gap_x = 8;
	//int gap_y= 8;


	int win_width = NUM_BLOCK_X*CELL_WIDTH; 
	int win_height = NUM_BLOCK_Y*CELL_HEIGHT;

	vector<float> features;

	//const char* cmd = "~/src/libsvm/svm-scale -l 0 -u 1 -s range test.txt > test.txt.scale";
	FILE* fp = fopen("sliding_output.txt","w");

	int M = multi_scale_images[0].rows;
	int N = multi_scale_images[0].cols;
	vector<int> px;
	vector<int> py;
	vector<float> scale_list;
	int index = 0;

	for(int loop = 0; loop < multi_scale_images.size(); loop++ ){

		img = multi_scale_images[loop];
		M = img.rows;
		N = img.cols;
		

		printf("Processing image at scale: %0.3f %d %d\n",scales[loop],M,N);
		
		for( int i = 1; i < M-win_height-1; i += gap_y ){
			for( int j = 1; j < N-win_width-1; j += gap_x ){


				float** hist_list = compute_cell_histogram( j, i, img ); 		
				features = block_normalize( hist_list );

				fprintf(fp,"0 ");
				print_features( features, fp); 
	
				px.push_back( j*(1.0/scales[loop]) );
				py.push_back( i*(1.0/scales[loop]) );
				scale_list.push_back( 1.0/scales[loop] );

				index++;

			}
		}
	}
	printf("index: %d\n",index);

	fclose(fp);
// /*

	img = multi_scale_images[0];
	N = img.rows;
	M = img.cols;

	system("svm_classify -v 3 sliding_output.txt svm_model.txt svm_output.txt");
	
	FILE* fp_out = fopen("svm_output.txt","r");
	float score = 0.0;

	index = 0;	

	namedWindow("HOG", WINDOW_AUTOSIZE );
	//cvtColor( img, img, CV_GRAY2BGR );
	
	while( fscanf(fp_out,"%f\n", &score) > 0){
		if( score > 0.0 ){
			int xmin = px[index]; 
			int ymin = py[index];
			int xmax = xmin+win_width*scale_list[index];
			int ymax = ymin+win_height*scale_list[index];
			printf("%f %d %d %d %d: %f\n",score, xmin, ymin, xmax, ymax, scale_list[index] );
			rectangle( img, cvPoint(xmin,ymin), cvPoint(xmax,ymax), CV_RGB(1.0/scale_list[index]*255,0,0),1,8);

		}
		index++;
	}

	fclose(fp);


	imshow("HOG", img);
	waitKey(0);
 //*/
	return 0;
}
