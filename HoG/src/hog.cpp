#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <vector>

#define NUM_HIST_BIN 9

using namespace std;

static float* compute_cell_histogram(int top_left_corner_x, int top_left_corner_y, float** img, int M, int N){

	assert( top_left_corner_x > 0 );
	assert( top_left_corner_y >= 0 );
	assert( top_left_corner_x+8 < M );
	assert( top_left_corner_y+8 <= N );

	float* histogram = (float*) malloc(sizeof(float)*NUM_HIST_BIN);	
	memset(histogram,0,sizeof(float)*NUM_HIST_BIN);

	for( int i = top_left_corner_y; i < top_left_corner_y+8; i++){
		for( int j = top_left_corner_x; j < top_left_corner_x+8; j++){
		
			float diff = -img[i][j-1]+img[i][j+1];
			float mag = sqrt( diff*diff );
			float ang = atan( img[i][j-1]/img[i][j+1])/mag;

			int bin = (int)((int)ang%20);
			int weight = 1.0-(ang-(bin*10.0f))/20.0;

			histogram[bin] += weight*mag;
			if(bin < 9){
				histogram[bin+1] += (1.0-weight)*mag;
			}

		}
	}
	
	

}

vector<float> compute_HoG_180(float** img, int M, int N){



}

int main() {

	float** sample_row = (float**)malloc(sizeof(float*));
	sample_row[0] = (float*)malloc(sizeof(float)*10);
	

	compute_cell_histogram(1,0,sample_row,1,10);

	return 0;
}
