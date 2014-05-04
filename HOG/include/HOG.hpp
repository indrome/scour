
#define NUM_HIST_BIN 9
#define CELL_WIDTH 8
#define CELL_HEIGHT 8
#define WIN_WIDTH 64
#define WIN_HEIGHT 128

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>

using namespace std;

float** compute_cell_histogram(int top_left_corner_x, int top_left_corner_y, uchar** img, int M, int N);
vector<float> block_normalize(float** hist_list, int cells_x, int cells_y);
void print_features( vector<float>& features);
