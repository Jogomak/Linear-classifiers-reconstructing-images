#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    learning_sample_size = 7;
    image_original_size = 50;
    image_pixels_original_count = image_original_size * image_original_size;

    this -> setMinimumSize(820, 460);
    image_width_height = 400;
    original_images = new QImage*[learning_sample_size];
    image_in = new QImage(image_original_size, image_original_size, QImage::Format_ARGB32);
    image_out = new QImage(image_original_size, image_original_size, QImage::Format_ARGB32);
    image_out_bits = reinterpret_cast<unsigned int *>(image_out->bits());
    image_in_bits = reinterpret_cast<unsigned int *>(image_in->bits());
    white = 0xFFFFFFFF;
    black = 0xFF000000;

    original_images_bits = new unsigned int*[learning_sample_size];
    char image_path[14] = {':', '/', 'i', 'm', 'a', 'g', 'e', 's', '/', '0', '.', 'p', 'n', 'g'};
    for(int i = 0; i < learning_sample_size; i++){
        image_path[9] = char(48 + i);
        original_images[i] = new QImage(image_path, "PNG");
        original_images_bits[i] = reinterpret_cast<unsigned int *>(original_images[i]->bits());
    }

    actual_image = 0;
    memcpy(image_in->bits(), original_images[actual_image]->bits(), 10000);
    button_next_image = new QPushButton(">", this);
    button_prev_image = new QPushButton("<", this);
    button_add_noise = new QPushButton("Add noise", this);
    button_clear_original_image = new QPushButton("Clear", this);
    button_next_image->setGeometry(410, 420, 90, 23);
    button_prev_image->setGeometry(320, 420, 90, 23);
    button_add_noise->setGeometry(230, 420, 90, 23);
    button_clear_original_image->setGeometry(500, 420, 90, 23);
    connect(button_next_image, SIGNAL(pressed()), this, SLOT(next_image()));
    connect(button_prev_image, SIGNAL(pressed()), this, SLOT(prev_image()));
    connect(button_add_noise, SIGNAL(pressed()), this, SLOT(add_noise()));
    connect(button_clear_original_image, SIGNAL(pressed()), this, SLOT(clear_original_image()));

    std::srand(static_cast<unsigned int>(time(nullptr)));

    int test = time(nullptr);
    initialize_linear_classifiers_and_prepare_data();
    learn_linear_classifiers();
    clear_image();
    std::cout << time(nullptr) - test;
}

MainWindow::~MainWindow()
{
    delete image_out;
    delete image_in;
    delete []original_images_bits;
    for(int i = 0; i < learning_sample_size; i++){
        delete original_images[i];
    }
    delete []original_images;
    delete button_next_image;
    delete button_prev_image;

    for(int i = 0; i < image_pixels_original_count; i++){
        delete linear_classifiers[i];
    }
    delete []learning_data;
    delete []input_data;
}

void MainWindow::initialize_linear_classifiers_and_prepare_data(){
    for(int i = 0; i < image_pixels_original_count; i++){
        linear_classifiers[i] = new LinearClassifier(i, image_pixels_original_count);
    }

    original_images_data = new int*[learning_sample_size];
    input_data = new int[image_pixels_original_count + 1];
    unsigned int *image_bits;

    for(int i = 0; i < learning_sample_size; i++){
        original_images_data[i] = new int[image_pixels_original_count + 1];
        image_bits = reinterpret_cast<unsigned int *>(original_images[i]->bits());

        for(int j = 0; j < image_pixels_original_count; j++){
            if(image_bits[j] == black){
                original_images_data[i][j] = 1;
            }else{
                original_images_data[i][j] = -1;
            }
        }
        original_images_data[i][image_pixels_original_count] = 1;
    }
}

/*void MainWindow::learn_linear_classifiers(){ //multi thread - not working
    for(int j = 0; j < 5; j++){
        threads[j] = std::thread(&MainWindow::learn_linear_classifiers_thread, this, j * image_pixels_original_count / 5, (j + 1) * image_pixels_original_count / 5);
    }

    for(int j = 0; j < 5; j++){
        threads[j].join();
    }
}

void MainWindow::learn_linear_classifiers_thread(int start, int stop){
    int image_number;
    int random_number;
    int learning_data_thread[image_pixels_original_count + 1];

    for(int i = 0; i < learning_sample_size * 400; i++){
        image_number = i % 7;
        memcpy(learning_data_thread, original_images_data[image_number], image_pixels_original_count + 1);

        for(int j = 0; j < 250; j++){
            random_number = (rand() + start) % image_pixels_original_count;
            learning_data_thread[random_number] = -learning_data_thread[random_number];
        }

        for(int j = start; j < stop; j++){
            linear_classifiers[j]->learn(learning_data_thread, original_images_data[image_number]);
        }
    }
}*/

void MainWindow::learn_linear_classifiers(){ //single thread
    int image_number;
    int random_number;
    learning_data = new int[image_pixels_original_count + 1];

    std::srand(static_cast<unsigned int>(time(nullptr)));

    for(int i = 0; i < learning_sample_size * 1000; i++){
        image_number = i % learning_sample_size;
        memcpy(learning_data, original_images_data[image_number], static_cast<unsigned int>(image_pixels_original_count + 1));

        for(int j = 0; j < 350; j++){
            random_number = rand() % image_pixels_original_count;
            learning_data[random_number] = -learning_data[random_number];
        }

        for(int j = 0; j < image_pixels_original_count; j++){
            linear_classifiers[j]->learn(learning_data, original_images_data[image_number]);
        }
    }
}

void MainWindow::clear_image(int loop, int number_of_loops){
    if(loop < number_of_loops){
        if(loop == 0){
            memcpy(image_out_bits, image_in_bits, static_cast<unsigned int>(image_pixels_original_count));
        }

        for(int i = 0; i < image_pixels_original_count; i++){
            if(image_out_bits[i] == black){
                input_data[i] = 1;
            }else{
                input_data[i] = -1;
            }
        }
        input_data[image_pixels_original_count] = 1;

        for(int i = 0; i < image_pixels_original_count; i++){
            image_out_bits[i] = linear_classifiers[i]->check(input_data);
        }

        for(int i = 0; i < image_pixels_original_count; i++){
            if(image_out_bits[i] != original_images_bits[actual_image][i]){
                clear_image(loop + 1, number_of_loops);
                break;
            }
        }
    }
}

void MainWindow::paintEvent(QPaintEvent *){
    static QSize image_size = QSize(image_width_height, image_width_height);
    static int image_out_draw_start = image_width_height + 20;
    QPainter painter(this);

    painter.drawImage(0, 0, image_in->scaled(image_size, Qt::IgnoreAspectRatio, Qt::FastTransformation));
    painter.drawImage(image_out_draw_start, 0, (*image_out).scaled(image_size, Qt::IgnoreAspectRatio, Qt::FastTransformation));
}

void MainWindow::mousePressEvent(QMouseEvent *mouse_event){
    x = mouse_event -> x() / 8;        //division by 8, to get x, y from original image
    y = mouse_event -> y() / 8 * image_original_size; //scaling to set pixel (x + y), not (x + y * 50), make multiply here, not later
    mouse_button_pressed = mouse_event->button();

    if(mouse_button_pressed == 0x00000001){
        painting_color = black;
    }else{
        painting_color = white;
    }

    put_pixel();
}

void MainWindow::mouseMoveEvent(QMouseEvent *mouse_event){
    x = mouse_event -> x() / 8;
    y = mouse_event -> y() / 8 * image_original_size;

    put_pixel();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *){
    clear_image();
    update();
}

void MainWindow::put_pixel(){
    if(x >= 0 && x < image_original_size && y >= 0 && y <= image_pixels_original_count - image_original_size){
        image_in_bits[x + y] = painting_color;
        if(x < image_original_size - 1)
            image_in_bits[x + y + 1] = painting_color;
        if(x > 0)
            image_in_bits[x + y - 1] = painting_color;
        if(y < image_pixels_original_count - image_original_size)
            image_in_bits[x + y + image_original_size] = painting_color;
        if(y > 0)
            image_in_bits[x + y - image_original_size] = painting_color;
        if(x < image_original_size - 2)
            image_in_bits[x + y + 2] = painting_color;
        if(x > 1)
            image_in_bits[x + y - 2] = painting_color;
        if(y < image_pixels_original_count - 2 * image_original_size)
            image_in_bits[x + y + 2 * image_original_size] = painting_color;
        if(y > image_original_size)
            image_in_bits[x + y - 2 * image_original_size] = painting_color;
        if(x < image_original_size - 1 && y < image_pixels_original_count - image_original_size)
            image_in_bits[x + y + image_original_size + 1] = painting_color;
        if(x > 0 && y < image_pixels_original_count - image_original_size)
            image_in_bits[x + y + image_original_size - 1] = painting_color;
        if(y > 0 && x > 0)
            image_in_bits[x + y - image_original_size - 1] = painting_color;
        if(y > 0 && x < image_original_size - 1)
            image_in_bits[x + y - image_original_size + 1] = painting_color;
    }

    clear_image(0, 1);
    update();
}

void MainWindow::add_noise(){
    int random_pixel;
    for(int i = 0; i < 150; i++){
        random_pixel = rand() % image_pixels_original_count;
        if(image_in_bits[random_pixel] == black){
            image_in_bits[random_pixel] = white;
        }else{
            image_in_bits[random_pixel] = black;
        }
    }
    clear_image();
    update();
}

void MainWindow::clear_original_image(){
    memcpy(image_in->bits(), original_images[actual_image]->bits(), 10000);
    clear_image();
    update();
}

void MainWindow::next_image(){
    actual_image = (actual_image + 1) % learning_sample_size;
    memcpy(image_in->bits(), original_images[actual_image]->bits(), 10000);
    clear_image();
    update();
}

void MainWindow::prev_image(){
    actual_image = (actual_image - 1) % learning_sample_size;
    if(actual_image < 0){
        actual_image = learning_sample_size - 1;
    }
    memcpy(image_in->bits(), original_images[actual_image]->bits(), 10000);
    clear_image();
    update();
}
