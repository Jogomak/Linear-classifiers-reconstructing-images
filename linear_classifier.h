#ifndef LINEAR_CLASSIFIER_H
#define LINEAR_CLASSIFIER_H

class LinearClassifier
{
public:
    LinearClassifier(int, int data_size);
    ~LinearClassifier();

    static int data_size_with_bias;
    static unsigned int white, black;

    int recognize_bit;
    int *weights_white; //weights[data_size + 1] = bias
    int *weights_black;

    void learn(int *, int *);
    unsigned int check(int*);
};

#endif // LINEAR_CLASSIFIER_H
