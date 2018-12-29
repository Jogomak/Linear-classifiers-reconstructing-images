#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QString>
#include <QPushButton>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <unistd.h>
#include "linear_classifier.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int learning_sample_size;
    int image_original_size;
    int image_pixels_original_count;

    QImage *image_in, *image_out;
    QImage **original_images;
    int image_width_height;
    unsigned int white, black;
    unsigned int painting_color;
    unsigned int **original_images_bits;
    unsigned int *image_in_bits, *image_out_bits;
    unsigned int mouse_button_pressed;
    int x, y;

    QPushButton *button_next_image;
    QPushButton *button_prev_image;
    QPushButton *button_add_noise;
    QPushButton *button_clear_original_image;
    int actual_image;

    LinearClassifier *linear_classifiers[2500];
    int **original_images_data;
    int *learning_data;
    int *input_data;

    std::thread threads[5];

    void initialize_linear_classifiers_and_prepare_data();
    void learn_linear_classifiers();
    void learn_linear_classifiers_thread(int, int);
    void clear_image(int = 0, int = 12);
    void put_pixel();

private slots:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void add_noise();
    void next_image();
    void prev_image();
    void clear_original_image();
};

#endif // MAINWINDOW_H
