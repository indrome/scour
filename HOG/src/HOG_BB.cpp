#include "HOG.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char* argv[]) {

	int x_min, y_min, x_max, y_max;
	char* img_path = NULL;

	if(argc != 6){
		printf("usage: PROGAM path_to_image Xmin Ymin Xmax Ymax\n");
		exit(-1);
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
	//Mat img = imread(img_path, CV_LOAD_IMAGE_GRAYSCALE);
	Mat img = imread(img_path);

	if( img.data == NULL ){
		printf("Could not read image: %s\n", argv[1]);	
		exit(1);
	}

	assert( x_max < img.cols );
	assert( y_max < img.rows );

	//equalizeHist( img, img );

	if( img.data == NULL ){
		printf("ERROR: Image could not be read\n");
		return -1;
	}

	int win_width = NUM_BLOCK_X*CELL_WIDTH;
	int win_height = NUM_BLOCK_Y*CELL_HEIGHT; 

	float** hist_list = compute_cell_histogram( x_min, y_min, img); 		
	vector<float> features = block_normalize( hist_list );
	printf("0 ");
	print_features( features ); 

	return 0;

}
