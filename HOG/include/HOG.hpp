
#define NUM_HIST_BIN 9
#define CELL_WIDTH 8
#define CELL_HEIGHT 8
#define NUM_BLOCK_X 8
#define NUM_BLOCK_Y 16

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>

using namespace std;
using namespace cv;

float** compute_cell_histogram( int top_left_corner_x, int top_left_corner_y, Mat img );
vector<float> block_normalize(float** hist_list);
void print_features( vector<float>& features);
void print_features( vector<float>& features, FILE* fp);
