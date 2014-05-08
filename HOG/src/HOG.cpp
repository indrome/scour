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


static float* compute_HOG(uchar** img, int i, int j){
	float div = 180.0/9.0;
	float* histogram = (float*) malloc(sizeof(float)*(NUM_HIST_BIN+1));	
	if( histogram == NULL ){
		printf("ERROR: Unable to allocate cell histogram\n");
		exit(1);
	}
	memset(histogram,0,sizeof(float)*(NUM_HIST_BIN+1));

	for( int k = i; k < i+CELL_HEIGHT; k++){
		for( int l = j; l < j+CELL_WIDTH; l++){

			float dx = (float)( -img[k][l-1]+img[k][l+1] );
			float dy = (float)( -img[k-1][l]+img[k+1][l] );
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

float** compute_cell_histogram(int top_left_corner_x, int top_left_corner_y, uchar** img, int window_width, int window_height){

	assert( top_left_corner_x > 0);
	assert( top_left_corner_y > 0);

	int cell_x = (window_width)/CELL_WIDTH;
	int cell_y = (window_height)/CELL_HEIGHT;
	float** hist_list = (float**) malloc(sizeof(float**)*cell_x*cell_y);

	int index = 0;
	
	for( int i = top_left_corner_y; i < top_left_corner_y+(cell_y*CELL_HEIGHT); i+= CELL_HEIGHT) {
		for( int j = top_left_corner_x; j < top_left_corner_x+(cell_x*CELL_WIDTH); j+=CELL_WIDTH) {
			hist_list[index++] = compute_HOG( img, i, j );
		}
	}

	return hist_list;
}

void get_block_vector( vector<float>& features, float** hist_list, int i, int j, int cell_x ){
	float sum_of_squares = 0.0;

	for(int k=0; k < 9; k++){
		features.push_back( hist_list[i*cell_x+j][k] );
		sum_of_squares += features.back()*features.back();
	}
	for(int k=0; k < 9; k++){
		features.push_back( hist_list[i*cell_x+j+1][k] );	
		sum_of_squares += features.back()*features.back();
	}
	for(int k=0; k < 9; k++){
		features.push_back( hist_list[(i+1)*cell_x+j][k] );	
		sum_of_squares += features.back()*features.back();
	}
	for(int k=0; k < 9; k++){
		features.push_back( hist_list[(i+1)*cell_x+j+1][k] );	
		sum_of_squares += features.back()*features.back();
	}

	for( unsigned int k = features.size()-9*4; k < (unsigned int)features.size(); k++) {
		features[k] = features[k]*1.0/sqrtf(sum_of_squares+eps*eps);
	}
}

vector<float> block_normalize(float** hist_list, int cells_x, int cells_y){

	//int num_features = 4*NUM_HIST_BIN*(cells_x-1)*(cells_y-1);
	//float* HOG_features = (float*) malloc( sizeof(float)*num_features );
	//if( HOG_features == NULL ){
	//	printf("ERROR: unable to allocate HOG feature vector\n");
	//	exit(1);
	//}
	//memset(HOG_features, 0, sizeof(float)*num_features);
	vector<float> HOG_features;

	for(int i = 0; i < cells_y-1; i++ ) {
		for( int j = 0; j < cells_x-1; j++) {
			get_block_vector(HOG_features, hist_list, i, j, cells_x);	
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


 //float*** compute_sliding_window(uchar** img, int M, int N){
 //	
 //	printf("Running a sliding window\n");
 //
 //	assert( M > WIN_HEIGHT );
 //	assert( N > WIN_WIDTH );
 //	
 //	int cells_x = WIN_WIDTH/CELL_WIDTH;
 //	int cells_y = WIN_HEIGHT/CELL_HEIGHT;
 //
 //	float*** hist_list = (float***) malloc(sizeof(float***)*cells_x*cells_y);
 //
 //	for(int i = 1; i+WIN_HEIGHT < M; i++ ){
 //		for( int j = 1; j+WIN_WIDTH < N; j++ ){
 //			hist_list[i*cells_x+j] = compute_cell_histogram( j, i, img, WIN_HEIGHT, WIN_WIDTH ); 		
 //		}
 //	}
 //
 //	return hist_list;	
 //}

