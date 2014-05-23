#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <vector>

#include "HOG.hpp"

#define eps 0.0001 

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

static inline uchar index( int i, int j, int stride){ return i*stride+j; }

static float* compute_HOG(Mat img, int y, int x){

	//uchar* img_data = img.data;

	float div = M_PI/9.0;
	float* histogram = (float*) malloc(sizeof(float)*(NUM_HIST_BIN+1));	
	if( histogram == NULL ){
		printf("ERROR: Unable to allocate cell histogram\n");
		exit(1);
	}
	memset(histogram,0,sizeof(float)*(NUM_HIST_BIN+1));

	for( int k = y; k < y+CELL_HEIGHT; k++){
		for( int l = x; l < x+CELL_WIDTH; l++){

			float dx = 0.0; float dy = 0.0;

			Vec3b dy_BGR_up = img.at<Vec3b>(k+1,l);
			Vec3b dy_BGR_down = img.at<Vec3b>(k-1,l);

			Vec3b dx_BGR_left = img.at<Vec3b>(k,l-1);
			Vec3b dx_BGR_right = img.at<Vec3b>(k,l+1);

			// TODO: check that the mast is correct

			dy = dy_BGR_up[0]-dy_BGR_down[0];
			dy = (dy < dy_BGR_up[1]-dy_BGR_down[1]) ? dy_BGR_up[1]-dy_BGR_down[1] : dy;
			dy = (dy < dy_BGR_up[2]-dy_BGR_down[2]) ? dy_BGR_up[2]-dy_BGR_down[2] : dy;

			dx = dx_BGR_left[0]-dx_BGR_right[0];
			dx = (dx < dx_BGR_left[1]-dx_BGR_right[1]) ? dx_BGR_left[1]-dx_BGR_right[1] : dx;
			dx = (dx < dx_BGR_left[2]-dx_BGR_right[2]) ? dx_BGR_left[2]-dx_BGR_right[2] : dx;

			dy += eps;
			dx += eps;

			float mag = sqrtf( dx*dx + dy*dy );
			float ang = fabs(atan(dy/dx) + M_PI/2.0);			
			int bin = (int)floorf(ang/div);
			 
			histogram[bin] += mag;

		}
	}
	
	for(int k = 0; k < 9; k++){
		histogram[9] += histogram[k];
	}

	return histogram;
}

float** compute_cell_histogram(int top_left_corner_x, int top_left_corner_y, Mat img){


	int win_width = NUM_BLOCK_X*CELL_WIDTH;
	int win_height = NUM_BLOCK_Y*CELL_HEIGHT;

	assert( top_left_corner_x >= 0 );
	assert( top_left_corner_y >= 0 );
	assert( top_left_corner_x + win_width <= img.cols ); 
	assert( top_left_corner_y + win_height <= img.rows ); 


	float** hist_list = (float**) malloc(sizeof(float**)*NUM_BLOCK_X*NUM_BLOCK_Y);

	int index = 0;
	
	for( int i = top_left_corner_y; i < top_left_corner_y+(NUM_BLOCK_Y*CELL_HEIGHT); i+= CELL_HEIGHT) {
		for( int j = top_left_corner_x; j < top_left_corner_x+(NUM_BLOCK_X*CELL_WIDTH); j+=CELL_WIDTH) {
			hist_list[index++] = compute_HOG( img, i, j );
		}
	}

	return hist_list;
}

void get_block_vector( vector<float>& features, float** hist_list, int i, int j ){
	float sum_of_squares = 0.0;

	for(int k=0; k < 9; k++){
		features.push_back( hist_list[i*NUM_BLOCK_X+j][k] );
		sum_of_squares += features.back()*features.back();
	}
	for(int k=0; k < 9; k++){
		features.push_back( hist_list[i*NUM_BLOCK_X+j+1][k] );	
		sum_of_squares += features.back()*features.back();
	}
	for(int k=0; k < 9; k++){
		features.push_back( hist_list[(i+1)*NUM_BLOCK_X+j][k] );	
		sum_of_squares += features.back()*features.back();
	}
	for(int k=0; k < 9; k++){
		features.push_back( hist_list[(i+1)*NUM_BLOCK_X+j+1][k] );	
		sum_of_squares += features.back()*features.back();
	}
	for( unsigned int k = features.size()-9*4; k < (unsigned int)features.size(); k++) {
		features[k] = features[k]*1.0/sqrtf(sum_of_squares+eps*eps);
	}
}

vector<float> block_normalize(float** hist_list){

	vector<float> HOG_features;

	for(int i = 0; i < NUM_BLOCK_Y-1; i++ ) {
		for( int j = 0; j < NUM_BLOCK_X-1; j++) {
			get_block_vector(HOG_features, hist_list, i, j);	
		} 
	} 

	return HOG_features;
}

void print_features( vector<float>& features ){
	for( unsigned int i = 0; i < (unsigned int)features.size()-1; i++){
		// Remember: SVM_light feature indices start at 1
		printf("%d:%f ", i+1, features[i]);
	}
	printf("\n");
}
void print_features( vector<float>& features, FILE* fp){
	for( unsigned int i = 0; i < (unsigned int)features.size()-1; i++){ // there are 9 features and 1 sum
		// Remember: SVM_light feature indices start at 1
		fprintf(fp,"%d:%f ", i+1, features[i]);
	}
	fprintf(fp,"\n");
}


