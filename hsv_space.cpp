#include "TMT_Final_Project.h"

using namespace cv;
using namespace std;

//void CallBackFunc(int event, int x, int y, int flags, void* userdata)
//{
//    if (event == EVENT_LBUTTONDOWN) {
//        Mat image = *(Mat*)userdata;
//        Mat image_out = image.clone();
//        Mat img_hsv;
//        Rect rect(x, y, 1, 1);
//        Scalar mean = cv::mean(image_out(rect));
//        cout << "Blue: " << mean[0] << "  Green: " << mean[1] << "  Red: " << mean[2] << endl;
//        cvtColor(image_out, img_hsv, COLOR_BGR2HSV);
//        Scalar mean_hsv = cv::mean(img_hsv(rect));
//        cout << "H: " << mean_hsv[0] << "  S: " << mean_hsv[1] << "  V: " << mean_hsv[2] << endl;
//    }
//}

void hsv_space() {

    // Reading the source image
    string path("C:\\Users\\tomma\\Desktop\\rgb\\01.jpg");
    //string path("...\\test\\input_images\\01.jpg");
    Mat src = imread(path, IMREAD_COLOR);

    if (src.empty()) {
        std::cout << "Could not open or find the image!\n" << std::endl;
        return exit(0);
    }

    //--------------------------------------------------------------------------------------------------
    //              Reading from file txt
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

    //--------------------------------------------------------------------------------------------------
    //              Equalize light brightness
    //--------------------------------------------------------------------------------------------------

    Mat image_copy = src.clone();
    Mat image_windows = image_copy.clone();

    //Mat image_gray;
    //float intensity_tot = 0, alpha = 0;

    //cvtColor(image_copy, image_gray, COLOR_BGR2GRAY);
    //for (int i = 0; i < image_gray.rows; i++)
    //{
    //    for (int j = 0; j < image_gray.cols; j++) {
    //        intensity_tot += image_gray.at<uchar>(i, j);
    //    }
    //}
    //alpha = intensity_tot / (255 * image_gray.rows * image_gray.cols);
    //cout << "alpha: " << alpha << endl;
    //image_copy.convertTo(image_copy, -1, alpha);


    //vector<cv::Mat> chnn;
    //cvtColor(image_copy, image_copy, COLOR_BGR2YUV);
    //split(image_copy, chnn);
    //equalizeHist(chnn[0], chnn[0]);
    //merge(chnn, image_copy);
    //cvtColor(image_copy, image_copy, COLOR_YUV2BGR);

    //--------------------------------------------------------------------------------------------------
    //              Equalize light brightness respect to the hands
    //--------------------------------------------------------------------------------------------------
    unsigned int x_angle;
    unsigned int y_angle;
    unsigned int width;
    unsigned int height;
    vector<Mat> image_crop;
    vector<string> name_windows = { "Crop 1", "Crop 2", "Crop 3", "Crop 4", "Crop 5" };

    Mat image_gray;
    float intensity_tot = 0; 
    vector<float> alpha;
    

    Mat temp;

    for (int i = 0; i < window_coord.size(); i++)
    {

        x_angle = window_coord[i][0];
        y_angle = window_coord[i][1];
        width = window_coord[i][2];
        height = window_coord[i][3];

        Rect crop_window(x_angle, y_angle, width, height);

        
        image_crop.push_back(image_copy(crop_window));
        namedWindow(name_windows[i], WINDOW_NORMAL);
        imshow(name_windows[i], image_crop[i]);

        rectangle(image_windows, crop_window, Scalar(255, 0, 255), 2); // Draw it on image


        cvtColor(image_crop[i], image_gray, COLOR_BGR2GRAY);
        
        intensity_tot = 0;
        for (int w = 0; w < image_gray.rows; w++)
        {            
            for (int j = 0; j < image_gray.cols; j++) 
            {
                intensity_tot += image_gray.at<uchar>(w, j);
            }
        }
        alpha.push_back( intensity_tot / (255 * image_gray.rows * image_gray.cols));
        cout << "alpha: " << alpha[i] << endl;
    }

    float alpha_tot=0, sum=0;
    for (int n=0; n<alpha.size(); n++)
    {
        sum += alpha[n];           
    }
    alpha_tot = (sum / alpha.size());
    cout << "alpha tot: " << alpha_tot << endl;

    vector<cv::Mat> chn_hsv;
    cvtColor(image_copy, image_copy, COLOR_BGR2HSV);
    split(image_copy, chn_hsv);
    chn_hsv[2].convertTo(chn_hsv[2], -1, alpha_tot);
    merge(chn_hsv, image_copy);
    cvtColor(image_copy, image_copy, COLOR_HSV2BGR);

    //vector<cv::Mat> chn_bgr;
    //split(image_copy, chn_bgr);
    //chn_bgr[0].convertTo(chn_bgr[0], -1, alpha_tot);
    //chn_bgr[1].convertTo(chn_bgr[1], -1, alpha_tot);
    //chn_bgr[2].convertTo(chn_bgr[2], -1, alpha_tot);
    //merge(chn_bgr, image_copy);

    //image_copy.convertTo(image_copy, -1, alpha_tot);

    /*vector<cv::Mat> chnn;
    cvtColor(image_copy, image_copy, COLOR_BGR2YUV);
    split(image_copy, chnn);
    equalizeHist(chnn[0], chnn[0]);
    merge(chnn, image_copy);
    cvtColor(image_copy, image_copy, COLOR_YUV2BGR);*/

    //--------------------------------------------------------------------------------------------------
    //              HSV Space pre-filtering
    //--------------------------------------------------------------------------------------------------
    
    Mat img_hsv;

    cvtColor(image_copy, img_hsv, COLOR_BGR2HSV);
    medianBlur(img_hsv, img_hsv, 9);                    // Smoothing the image
 
    Vec3b channels_HSV;
    Vec3b channels_BGR;

    // fill the mask with black pixel except for the ones into the T_RANGE
    for (int i = 0; i < img_hsv.rows; i++) {
        for (int j = 0; j < img_hsv.cols; j++) {
            channels_HSV = img_hsv.at<Vec3b>(i, j);
            channels_BGR = image_copy.at<Vec3b>(i, j);
            img_hsv.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);     // Paint in black

            if ( (0 < channels_HSV[0] && channels_HSV[0] < 50) &&
                (40 < channels_HSV[1] && channels_HSV[1] < 170) &&
                (channels_HSV[2] < 160) &&
                (20 < channels_BGR[0]) && (40 < channels_BGR[1]) && (95 < channels_BGR[2]) &&
                (channels_BGR[1] < channels_BGR[2]) && (channels_BGR[0] < channels_BGR[2]) &&
                (channels_BGR[0] < 220 || channels_BGR[1] < 220 || channels_BGR[2] < 220) &&   // Remove white component
                (15 < abs(channels_BGR[2] - channels_BGR[1]))   )     
            {

                img_hsv.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255);
            }
        }
    }

    //--------------------------------------------------------------------------------------------------
    //              Image cropping
    //--------------------------------------------------------------------------------------------------
    //unsigned int x_angle;
    //unsigned int y_angle;
    //unsigned int width;
    //unsigned int height;
    //vector<Mat> image_crop;
    //vector<string> name_windows = { "Crop 1", "Crop 2", "Crop 3", "Crop 4", "Crop 5" };

    //Mat temp;

    //for (int i = 0; i < window_coord.size(); i++)
    //{
    //    x_angle = window_coord[i][0];
    //    y_angle = window_coord[i][1];
    //    width = window_coord[i][2];
    //    height = window_coord[i][3];

    //    Rect crop_window(x_angle, y_angle, width, height);

    //    // Draw it on image
    //    image_crop.push_back(image_copy(crop_window));
    //    namedWindow(name_windows[i], WINDOW_NORMAL);
    //    imshow(name_windows[i], image_crop[i]);

    //    rectangle(image_copy, crop_window, Scalar(255, 0, 255), 2);
    //}

    //--------------------------------------------------------------------------------------------------
    //              Print images
    //--------------------------------------------------------------------------------------------------

    namedWindow("Source", WINDOW_NORMAL);
    resizeWindow("Source", 740, 540);
    imshow("Source", image_copy);
    //setMouseCallback("Source", CallBackFunc, (void*)&image_copy);

    namedWindow("Hand detection", WINDOW_NORMAL);
    resizeWindow("Hand detection", 740, 540);
    imshow("Hand detection", image_windows);

    namedWindow("Mask", WINDOW_NORMAL);
    resizeWindow("Mask", 740, 540);
    imshow("Mask", img_hsv);

    waitKey(0);
}