#include "linear_classifier.h"

int LinearClassifier::data_size_with_bias = 2501;
unsigned int LinearClassifier::black = 0xFF000000;
unsigned int LinearClassifier::white = 0xFFFFFFFF;;

LinearClassifier::LinearClassifier(int recognize_bit, int data_size)
{
    this->recognize_bit = recognize_bit;
    LinearClassifier::data_size_with_bias = data_size + 1;

    weights_white = new int[data_size_with_bias];
    weights_black = new int[data_size_with_bias];

    for(int i = 0; i < data_size_with_bias; i++){
        weights_black[i] = 0;
        weights_white[i] = 0;
    }
}

LinearClassifier::~LinearClassifier(){
    delete[] weights_black;
    delete[] weights_white;
}

unsigned int LinearClassifier::check(int *input_pixels){
    int result_black = 0;
    int result_white = 0;

    for(int i = 0; i < data_size_with_bias; i++){
        result_black += weights_black[i] * input_pixels[i];
        result_white += weights_white[i] * input_pixels[i];
    }

    if(result_black > result_white){
        return black;
    }else{
        return white;
    }
}

void LinearClassifier::learn(int *input_pixels, int *image_original_pixels){
    static int result_black;
    static int result_white;
    result_black = 0;
    result_white = 0;

    for(int i = 0; i < data_size_with_bias; i++){
        result_black += weights_black[i] * input_pixels[i];
        result_white += weights_white[i] * input_pixels[i];
    }

    if(result_black > result_white){
        if(image_original_pixels[recognize_bit] != 1){
            for(int i = 0; i < data_size_with_bias; i++){
                weights_black[i] -= input_pixels[i];
                weights_white[i] += input_pixels[i];
            }
        }
    } else if(image_original_pixels[recognize_bit] != -1){
        for(int i = 0; i < data_size_with_bias; i++){
            weights_black[i] += input_pixels[i];
            weights_white[i] -= input_pixels[i];
        }
    }
}
