//--------------------------------------------------------------------------------------------------
//              AUTHOR: MARTONE TOMMASO     ID: 2016972
//--------------------------------------------------------------------------------------------------

#include "TMT_Final_Project.h"

using namespace cv;
using namespace std;


void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if (event == EVENT_LBUTTONDOWN) {
        Mat image = *(Mat*)userdata;
        Mat image_out = image.clone();
        Mat img_hsv;
        Rect rect(x, y, 1, 1);
        Scalar mean = cv::mean(image_out(rect));
        cout << "Blue: " << mean[0] << "  Green: " << mean[1] << "  Red: " << mean[2] << endl;
        cvtColor(image_out, img_hsv, COLOR_BGR2HSV);
        Scalar mean_hsv = cv::mean(img_hsv(rect));
        cout << "H: " << mean_hsv[0] << "  S: " << mean_hsv[1] << "  V: " << mean_hsv[2] << endl;
    }
}

void hsv() {

    //string path("C:\\Users\\tomma\\Desktop\\rgb\\05.jpg");
    string path("..\rgb\01.jpg");
    Mat src = imread(path, IMREAD_COLOR);

    if (src.empty()) {
        std::cout << "Could not open or find the image!\n" << std::endl;
        return exit(0);
    }

    //--------------------------------------------------------------------------------------------------
    //                               Reading from file txt
    //--------------------------------------------------------------------------------------------------

    string fileName = "C:\\Users\\tomma\\Desktop\\Univeristy\\1.2 - Computer Vision\\CV_workspace\\Computer-Vision\\TMT_Final_Project\\test\\bounding_boxes\\01_output.txt";
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
        tmp.push_back(num);
        count++;
        if (count == 4)
        {
            window_coord.push_back(tmp);
            tmp.clear();
            count = 0;
        }
    }
    myfile.close();

    Mat image_copy = src.clone();                   
    Mat image_windows = image_copy.clone();

    //--------------------------------------------------------------------------------------------------
    //                 Equalize light brightness respect to the hands
    //--------------------------------------------------------------------------------------------------
    int x_angle;
    int y_angle;
    int width;
    int height;
    vector<Mat> image_crop;
    vector<string> name_windows = { "Crop 1", "Crop 2", "Crop 3", "Crop 4", "Crop 5" };

    Mat image_hsv, image_hsv_crop, temp;
    cvtColor(image_copy, image_hsv, COLOR_BGR2HSV);
    Scalar brightness = 0;
    vector<double> alpha;
    vector<cv::Mat> chn_hsv_crop;

    //--------------------- Image cropping -------------------------------------------------------------
    for (int i = 0; i < window_coord.size(); i++)
    {
        x_angle = window_coord[i][0];
        y_angle = window_coord[i][1];
        width = window_coord[i][2];
        height = window_coord[i][3];

        Rect crop_window(x_angle, y_angle, width, height);
        image_crop.push_back(image_copy(crop_window));

        cvtColor(image_crop[i], image_hsv_crop, COLOR_BGR2HSV);  

        split(image_hsv_crop, chn_hsv_crop);
        brightness = 0;     // Reinitialize at each iteration
        brightness = cv::mean(chn_hsv_crop[2]);
        alpha.push_back(brightness[0]);
    }

    double alpha_tot = 0, sum = 0;
    for (int n = 0; n < alpha.size(); n++)
    {
        sum += alpha[n];
    }
    alpha_tot = (sum / alpha.size());           

    Mat image_bgr_norm;
    vector<cv::Mat> chn_hsv;
    split(image_hsv, chn_hsv);
    chn_hsv[2].convertTo(chn_hsv[2], -1, 120 / alpha_tot);      // Normalization of brightness
    merge(chn_hsv, image_hsv);
    cvtColor(image_hsv, image_bgr_norm, COLOR_HSV2BGR);    

    //--------------------------------------------------------------------------------------------------
    //              HSV Space pre-filtering
    //--------------------------------------------------------------------------------------------------

    medianBlur(image_hsv, image_hsv, 9);       // Smoothing the image

    Vec3b channels_HSV;
    Vec3b channels_BGR;

    // fill the image with black pixel except for the ones into the ranges
    for (int i = 0; i < image_bgr_norm.rows; i++) {
        for (int j = 0; j < image_bgr_norm.cols; j++) {
            channels_HSV = image_hsv.at<Vec3b>(i, j);
            channels_BGR = image_bgr_norm.at<Vec3b>(i, j);
            image_copy.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);     // Paint the mask in black

            if ((0 < channels_HSV[0] && channels_HSV[0] < 50) &&
                (40 < channels_HSV[1] && channels_HSV[1] < 180) &&
                (channels_HSV[2] < 170) &&
                (20 < channels_BGR[0]) && (40 < channels_BGR[1]) && (95 < channels_BGR[2]) &&
                (channels_BGR[1] < channels_BGR[2]) && (channels_BGR[0] < channels_BGR[2]) &&
                (channels_BGR[0] < 220 || channels_BGR[1] < 220 || channels_BGR[2] < 220) &&   // Remove white component
                (15 < abs(channels_BGR[2] - channels_BGR[1])))
            {
                image_copy.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255);
            }
        }
    }

    //--------------------------------------------------------------------------------------------------
    //              Create the mask
    //--------------------------------------------------------------------------------------------------
    Mat image_mask = image_copy.clone();

    for (int i = 0; i < image_copy.rows; i++) 
    {
        for (int j = 0; j < image_copy.cols; j++) 
        {
            image_mask.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
           
            for (int m = 0; m < window_coord.size(); m++)
            {
                x_angle = window_coord[m][0];
                y_angle = window_coord[m][1];
                width = window_coord[m][2];
                height = window_coord[m][3];

                if ((x_angle < j && j < x_angle + width) && (y_angle < i && i < y_angle + height))
                {
                    //cout << "coordinates: " << x_angle << " " << y_angle << " " << width << " " << height << " " << m << endl;
                    image_mask.at<cv::Vec3b>(i, j) = image_copy.at<cv::Vec3b>(i, j);
                }
            }
        }
    }

    // Erosion and dilation for smoothing the border of the mask and remove small segmentation
    Mat kernel = Mat::ones(9, 9, CV_8U);
    for (int p = 0; p < 15; p++)
    {
        erode(image_mask, image_mask, kernel);
        dilate(image_mask, image_mask, kernel);
    }

    // Inverting the mask and giving a color to highlinght the hands
    for (int i = 0; i < image_mask.rows; i++)
    {
        for (int j = 0; j < image_mask.cols; j++)
        {
            if (image_mask.at<Vec3b>(i, j) == Vec3b(0, 0, 0)) {                
                image_mask.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
            }
            else {
                image_mask.at<Vec3b>(i, j) = Vec3b(0, 100, 180);
            }
        }
    }

    Mat output_mask;  // Result output
    src.copyTo(output_mask, image_mask);

    // Adding the rectangle of the hand detection
    for (int i = 0; i < window_coord.size(); i++)
    {
        x_angle = window_coord[i][0];
        y_angle = window_coord[i][1];
        width = window_coord[i][2];
        height = window_coord[i][3];
        Rect crop_window(x_angle, y_angle, width, height);
        rectangle(output_mask, crop_window, Scalar(255, 0, 255), 2);      // Draw it on image
    }

    //--------------------------------------------------------------------------------------------------
    //              Print images
    //--------------------------------------------------------------------------------------------------

    //namedWindow("Normalized", WINDOW_NORMAL);
    //resizeWindow("Normalized", 740, 540);
    //imshow("Normalized", image_bgr_norm);
    //setMouseCallback("Normalized", CallBackFunc, (void*)&image_bgr_norm);

    //namedWindow("Hand detection", WINDOW_NORMAL);
    //resizeWindow("Hand detection", 740, 540);
    //imshow("Hand detection", image_windows);

    namedWindow("Hand detection and segmantation", WINDOW_NORMAL);
    resizeWindow("Hand detection and segmantation", 740, 540);
    imshow("Hand detection and segmantation", output_mask);

    waitKey(0);
}