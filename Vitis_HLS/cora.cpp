#include "cora.hpp"
#include "weights.hpp"
#include "ap_int.h"
#include "ap_fixed.h"

// ---------------- Layer 1 ----------------
void layer1(fixed_t input[N_INPUTS], const fixed_t weights[N_INPUTS][N_L1], fixed_t output[1][N_L1]){
    col: for(int j = 0; j < N_L1; j++){
        fixed_t sum = 0;

        row: for(int i = 0; i < N_INPUTS; i++){
            fixed_t prod = input[i] * weights[i][j];

            // Use DSP for layer 1 multipliers
            #pragma HLS RESOURCE variable=prod core=DSP48

            sum += prod;
        }

        output[0][j] = sum;
    }
}

// ReLU for layer1
void relu1(fixed_t input[1][N_L1], fixed_t output[1][N_L1]){
    loop: for(int i = 0; i < N_L1; i++){
        #pragma HLS PIPELINE
        output[0][i] = (input[0][i] < fixed_t(0)) ? fixed_t(0) : input[0][i];
    }
}

// ---------------- Layer 2 ----------------
void layer2(fixed_t input[1][N_L1], const fixed_t weights[N_L1][N_L2], fixed_t output[1][N_L2]){
    col: for(int j = 0; j < N_L2; j++){
        fixed_t sum = 0;

        row: for(int i = 0; i < N_L1; i++){
            fixed_t prod = input[0][i] * weights[i][j];

            // Keep layer 2 multipliers in LUTs
            #pragma HLS RESOURCE variable=prod core=Mul_LUT

            sum += prod;
        }

        output[0][j] = sum;
    }
}

// ReLU for layer2
void relu2(fixed_t input[1][N_L2], fixed_t output[1][N_L2]){
    loop: for(int i = 0; i < N_L2; i++){
        #pragma HLS PIPELINE
        output[0][i] = (input[0][i] < fixed_t(0)) ? fixed_t(0) : input[0][i];
    }
}

// ---------------- Layer 3 ----------------
void layer3(fixed_t input[1][N_L2], const fixed_t weights[N_L2][N_L3], fixed_t output[1][N_L3]){
    col: for(int j = 0; j < N_L3; j++){
        fixed_t sum = 0;

        row: for(int i = 0; i < N_L2; i++){
            fixed_t prod = input[0][i] * weights[i][j];

            // Keep layer 3 multipliers in LUTs
            #pragma HLS RESOURCE variable=prod core=Mul_LUT

            sum += prod;
        }

        output[0][j] = sum;
    }
}

// ---------------- Softmax Argmax ----------------
void softmax_argmax(fixed_t input[1][N_L3], int &pred){
    fixed_t max_val = fixed_t(-1024);
    int max_idx = -1;

    loop: for(int i = 0; i < N_L3; i++){
        #pragma HLS PIPELINE
        if(input[0][i] > max_val){
            max_val = input[0][i];
            max_idx = i;
        }
    }

    pred = max_idx;
}

// ---------------- Full Inference ----------------
int nn_inference(fixed_t input[N_INPUTS]){
    fixed_t out1[1][N_L1], out2[1][N_L2], out3[1][N_L3];
    int prediction;

    layer1(input, weights::w1, out1);
    relu1(out1, out1);

    layer2(out1, weights::w2, out2);
    relu2(out2, out2);

    layer3(out2, weights::w3, out3);
    softmax_argmax(out3, prediction);

    return prediction;
}
