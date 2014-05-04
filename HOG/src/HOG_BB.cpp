#include "HOG.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char* argv[]) {

	int x_min, y_min, x_max, y_max;
	char* img_path = NULL;

	if(argc != 6){
		printf("usage: %s path_to_image Xmin Ymin Xmax Ymax\n",argv[0]);
	}
	else {
		x_min = atoi(argv[2]);	
		y_min = atoi(argv[3]);	
		x_max = atoi(argv[4]);	
		y_max = atoi(argv[5]);	

		if(strnlen(argv[1],255) < 255)
			img_path = argv[1];
		else {
			printf("path_to_image is too long\n");
			return -1;
		}

		assert(x_min < x_max);
		assert(y_min < y_max);


	}

	// Read image and write row pointers
	Mat img = imread(img_path, CV_LOAD_IMAGE_GRAYSCALE);

	if( img.data == NULL ){
		printf("ERROR: Image could not be read\n");
		return -1;
	}

	int M = img.rows;
	int N = img.cols;

	assert( y_max < M );
	assert( x_max < N );

	int window_width = x_max - x_min;
	int window_height = y_max - y_min;

	int cell_x = window_width/CELL_WIDTH;
	int cell_y = window_height/CELL_HEIGHT;


	uchar* data = img.data;
	uchar** raw_data = (uchar**)malloc(sizeof(uchar*)*M);

	for(int i = 0; i < M; i++) {
		raw_data[i] = &data[i*N];	
	}


	float** hist_list = compute_cell_histogram( x_min, y_min, raw_data, window_height, window_width); 		
	vector<float> features = block_normalize( hist_list, cell_x, cell_y );
	print_features( features ); 

	return 0;
}
