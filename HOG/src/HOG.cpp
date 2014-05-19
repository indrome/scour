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

static float* compute_HOG(Mat img, int i, int j){

	uchar* img_data = img.data;


	float div = 180.0/9.0;
	float* histogram = (float*) malloc(sizeof(float)*(NUM_HIST_BIN+1));	
	if( histogram == NULL ){
		printf("ERROR: Unable to allocate cell histogram\n");
		exit(1);
	}
	memset(histogram,0,sizeof(float)*(NUM_HIST_BIN+1));

	for( int k = i; k < i+CELL_HEIGHT; k++){
		for( int l = j; l < j+CELL_WIDTH; l++){

			float dx = 0.0; float dy = 0.0;

			// Treat boundaries as the mask [-1,1] instead of [-1,0,1] and vice versa 
			if(k==0){
				dy = (float)( -img_data[ index(k,l,img.cols) ] +img_data[ index(k+1,l,img.cols) ]);
			}
			else if(k == img.rows){
				dy = (float)( -img_data[ index(k-1,l,img.cols) ] +img_data[ index(k,l,img.cols) ]);
			}
			else{
				dy = (float)( -img_data[ index(k-1,l,img.cols) ] +img_data[ index(k+1,l,img.cols) ]);
			}

			// Treat boundaries as the mask [-1,1] instead of [-1,0,1] and vice versa 
			if(l==0){
				dx = (float)( -img_data[ index(k,l,img.cols ) ]+img_data[ index(k,l+1,img.cols) ]);
			}
			else if(l == img.cols ){
				dx = (float)( -img_data[ index(k,l-1,img.cols ) ]+img_data[ index(k,l,img.cols) ]);
			}
			else {
				dx = (float)( -img_data[ index(k,l-1,img.cols ) ]+img_data[ index(k,l+1,img.cols) ]);
			}

			float mag = sqrtf( dx*dx + dy*dy + eps );

			dx = dx/mag;
			dy = dy/mag;

			float ang = atan((dy+eps)/(dx+eps));			
			ang = M_PI/2.0 + ang;
			ang = ang*(180.0/M_PI);

			int bin = (int)(ang/div);

			assert(bin < NUM_HIST_BIN);
			 
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
	for( unsigned int i = 0; i < (unsigned int)features.size(); i++){
		// Remember: SVM_light feature indices start at 1
		printf("%d:%f ", i+1, features[i]);
	}
	printf("\n");
}
void print_features( vector<float>& features, FILE* fp){
	for( unsigned int i = 0; i < (unsigned int)features.size(); i++){
		// Remember: SVM_light feature indices start at 1
		fprintf(fp,"%d:%f ", i+1, features[i]);
	}
	fprintf(fp,"\n");
}


