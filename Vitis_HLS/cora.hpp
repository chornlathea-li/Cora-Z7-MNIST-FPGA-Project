#include "ap_fixed.h"

// Replace float with fixed_t
typedef ap_fixed<8,4> fixed_t;

#define N_INPUTS 100
#define N_L1 16
#define N_L2 8
#define N_L3 10

// Function prototypes
void layer1(fixed_t input[N_INPUTS], const fixed_t weights[N_INPUTS][N_L1], fixed_t output[1][N_L1]);
void relu1(fixed_t input[1][N_L1], fixed_t output[1][N_L1]);
void layer2(fixed_t input[1][N_L1], const fixed_t weights[N_L1][N_L2], fixed_t output[1][N_L2]);
void relu2(fixed_t input[1][N_L2], fixed_t output[1][N_L2]);
void layer3(fixed_t input[1][N_L2], const fixed_t weights[N_L2][N_L3], fixed_t output[1][N_L3]);
void softmax_argmax(fixed_t input[1][N_L3], int &pred);
int nn_inference(fixed_t input[N_INPUTS]);

namespace weights {
    extern const fixed_t w1[N_INPUTS][N_L1];
    extern const fixed_t w2[N_L1][N_L2];
    extern const fixed_t w3[N_L2][N_L3];
}
