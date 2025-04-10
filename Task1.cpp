
#include "TMT_Final_Project.h"


using namespace cv;
using namespace std;

void task1() {

    // Reading the source image
    string path("C:\\Users\\tomma\\Desktop\\rgb\\03.jpg");
    //string path("...\\test\\input_images\\01.jpg");
    Mat src = imread(path, IMREAD_COLOR);

    if (src.empty()) {
        std::cout << "Could not open or find the image!\n" << std::endl;
        return exit(0);
    }

    //--------------------------------------------------------------------------------------------------
    //              Reading from file txt
    //--------------------------------------------------------------------------------------------------

    string fileName = "C:\\Users\\tomma\\Desktop\\Univeristy\\1.2 - Computer Vision\\CV_workspace\\Computer-Vision\\TMT_Final_Project\\test\\bounding_boxes\\03_output.txt";
    ifstream myfile(fileName.c_str(), ios_base::in);

    if (!myfile.is_open())
    {
        cout << " Failed to read file " << endl;
    }

    vector<vector<int>> window_coord;       // (x, y, width, height)
    vector<int> tmp;

    int num;
    int count = 0;

    while (myfile >> num) {
        //whatever you need to do
        tmp.push_back(num);
        count++;
        if (count == 4)
        {
            window_coord.push_back(tmp);
            tmp.clear();
            count = 0;
        }
    }

    for (int i = 0; i < window_coord.size(); i++)
    {
        for (int j = 0; j < window_coord[i].size(); j++)
            cout << window_coord[i][j] << " ";
        cout << '\n';
    }
    myfile.close();  
    
    ////----------------------------------------------------------------------------------------
    ////-----------		K - MEANS blurring the image before and increasing the contrast
    ////----------------------------------------------------------------------------------------
    
    Mat image_copy = src.clone();
    const unsigned int singleLineSize = image_copy.rows * image_copy.cols;
    const unsigned int K = 5;
    int MAX_ITERATIONS = 10;

    Mat blur_image;
    int Gaussian_kernel = 7;
    blur(image_copy, blur_image, Size(Gaussian_kernel, Gaussian_kernel));

    Mat blur_image_pre_gamma = Mat::zeros(blur_image.size(), blur_image.type());
    double alpha = 1.3;     //< Simple contrast control 
    int beta = 0.02;        //< Simple brightness control

    for (int y = 0; y < blur_image.rows; y++) {
        for (int x = 0; x < blur_image.cols; x++) {
            for (int c = 0; c < blur_image.channels(); c++) {
                blur_image_pre_gamma.at<Vec3b>(y, x)[c] =
                    saturate_cast<uchar>(alpha * blur_image.at<Vec3b>(y, x)[c] + beta);
            }
        }
    }
    //imshow("Original Image", blur_image);
    //imshow("New Image", blur_image_pre_gamma);


    Mat data_blur = blur_image_pre_gamma.reshape(1, singleLineSize);
    data_blur.convertTo(data_blur, CV_32F);
    std::vector<int> labels;
    cv::Mat1f colors2;
    cv::kmeans(data_blur, K, labels, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.), MAX_ITERATIONS, cv::KMEANS_RANDOM_CENTERS, colors2);
    for (unsigned int i = 0; i < singleLineSize; i++) {
        data_blur.at<float>(i, 0) = colors2(labels[i], 0);
        data_blur.at<float>(i, 1) = colors2(labels[i], 1);
        data_blur.at<float>(i, 2) = colors2(labels[i], 2);
    }
    Mat outputImage_blur = data_blur.reshape(3, blur_image.rows);
    outputImage_blur.convertTo(outputImage_blur, CV_8U);


    //--------------------------------------------------------------------------------------------------
    //              Image cropping
    //--------------------------------------------------------------------------------------------------
    unsigned int x_angle;
    unsigned int y_angle;
    unsigned int width;
    unsigned int height;
    vector<Mat> image_crop;
    vector<string> name_windows = { "Crop 1", "Crop 2", "Crop 3", "Crop 4", "Crop 5" };
    Mat temp;

    for (int i = 0; i < window_coord.size(); i++)
    {
        x_angle = window_coord[i][0];
        y_angle = window_coord[i][1];
        width = window_coord[i][2];
        height = window_coord[i][3];

        Rect crop_window(x_angle, y_angle, width, height);

        // Draw it on image
        image_crop.push_back(outputImage_blur(crop_window));
        //cvtColor(seg_image(crop_window), temp, COLOR_BGR2GRAY);
        //image_crop_bnw.push_back(temp);

        rectangle(image_copy, crop_window, Scalar(255, 0, 255), 2);
        namedWindow(name_windows[i], WINDOW_NORMAL);
        imshow(name_windows[i], image_crop[i]);
    }

    //--------------------------------------------------------------------------------------------------
    //              Print images
    //--------------------------------------------------------------------------------------------------

    namedWindow("Source", WINDOW_NORMAL);
    resizeWindow("Source", 740, 540);
    imshow("Source", image_copy);


    namedWindow("K-means", WINDOW_NORMAL);
    resizeWindow("K-means", 740, 540);
    imshow("K-means", outputImage_blur);
    

    cv::waitKey(0);
}
