#include "cora.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    // Initialize input
	fixed_t input_img[N_INPUTS];

	for (int i = 0; i < N_INPUTS; i++) {
	    input_img[i] = static_cast<fixed_t>(std::rand()) / RAND_MAX;
	}

    // Random input for testing
    std::srand(std::time(0));
    for (int i = 0; i < n_inputs; i++) {
        input_img[i] = static_cast<float>(std::rand()) / RAND_MAX; // 0 to 1
    }

    // Call the neural network inference
    int prediction = nn_inference(input_img);

    // Print input
    std::cout << "Input: ";
    for (int i = 0; i < n_inputs; i++) {
        std::cout << input_img[i] << " ";
    }
    std::cout << std::endl;

    // Print predicted class
    std::cout << "Predicted class: " << prediction << std::endl;

    return 0;
}
