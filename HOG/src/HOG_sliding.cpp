#include "HOG.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char* argv[]) {

	char* img_path = NULL;

	if(argc != 2){
		printf("usage: %s path_to_image\n",argv[0]);
	}
	else {

		if(strnlen(argv[1],255) < 255)
			img_path = argv[1];
		else {
			printf("path_to_image is too long\n");
			return -1;
		}
	}

	// Read image and write row pointers
	Mat img = imread(img_path, CV_LOAD_IMAGE_GRAYSCALE);

	if( img.data == NULL ){
		printf("ERROR: Image could not be read\n");
		return -1;
	}

	int M = img.rows;
	int N = img.cols;

	int window_gap = 20;

	int window_width = 64; 
	int window_height = 128;

	int cell_x = window_width/CELL_WIDTH;
	int cell_y = window_height/CELL_HEIGHT;


	uchar* data = img.data;
	uchar** raw_data = (uchar**)malloc(sizeof(uchar*)*M);

	for(int i = 0; i < M; i++) {
		raw_data[i] = &data[i*N];	
	}

	vector<float> features;

	for( int i = 1; i < M-window_height-1; i += window_gap ){
		for( int j = 1; j < N-window_width-1; j += window_gap ){
			assert(j+window_width < N );
			assert(i+window_height < M );
			float** hist_list = compute_cell_histogram( j, i, raw_data, window_width, window_height); 		
			features = block_normalize( hist_list, cell_x, cell_y );
			printf("0 ");
			print_features( features ); 
			//features.clear();
		}
	}

	return 0;
}
