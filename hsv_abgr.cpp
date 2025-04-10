//#include "TMT_Final_Project.h"
//
//using namespace cv;
//using namespace std;
//
//void CallBackFunc(int event, int x, int y, int flags, void* userdata)
//{
//    if (event == EVENT_LBUTTONDOWN) {
//        Mat image = *(Mat*)userdata;
//        Mat image_out = image.clone();
//        Mat img_hsv;
//        Rect rect(x, y, 1, 1);
//        Scalar mean = cv::mean(image_out(rect));
//        cout << "Blue: " << mean[0] << "  Green: " << mean[1] << "  Red: " << mean[2] << " A: " << mean[3] << endl;
//        cvtColor(image_out, img_hsv, COLOR_BGR2HSV);
//        Scalar mean_hsv = cv::mean(img_hsv(rect));
//        cout << "H mean: " << mean_hsv[0] << "  S mean: " << mean_hsv[1] << "  V mean: " << mean_hsv[2] << endl;
//    }
//}
//
//void hsv_abgr() {
//
//    // Reading the source image
//    string path("C:\\Users\\tomma\\Desktop\\rgb\\02.jpg");
//    //string path("...\\test\\input_images\\01.jpg");
//    Mat src = imread(path, IMREAD_COLOR);
//
//    if (src.empty()) {
//        std::cout << "Could not open or find the image!\n" << std::endl;
//        return exit(0);
//    }
//
//    //--------------------------------------------------------------------------------------------------
//    //              Reading from file txt
//    //--------------------------------------------------------------------------------------------------
//
//    string fileName = "C:\\Users\\tomma\\Desktop\\Univeristy\\1.2 - Computer Vision\\CV_workspace\\Computer-Vision\\TMT_Final_Project\\test\\bounding_boxes\\02_output.txt";
//    ifstream myfile(fileName.c_str(), ios_base::in);
//
//    if (!myfile.is_open())
//    {
//        cout << " Failed to read file " << endl;
//    }
//
//    vector<vector<int>> window_coord;       // (x, y, width, height)
//    vector<int> tmp;
//
//    int num;
//    int count = 0;
//
//    while (myfile >> num) {
//        tmp.push_back(num);
//        count++;
//        if (count == 4)
//        {
//            window_coord.push_back(tmp);
//            tmp.clear();
//            count = 0;
//        }
//    }
//
//    for (int i = 0; i < window_coord.size(); i++)
//    {
//        for (int j = 0; j < window_coord[i].size(); j++)
//            cout << window_coord[i][j] << " ";
//        cout << '\n';
//    }
//    myfile.close();
//
//
//    Mat image_copy = src.clone();
//
//    //--------------------------------------------------------------------------------------------------
//    //              HSV Space pre-filtering
//    //--------------------------------------------------------------------------------------------------
//
//    Mat img_hsv, img_bgra, img_ycc;
//
//    //medianBlur(img_hsv, img_hsv, 9);                    // Smoothing the image
//
//    cvtColor(image_copy, img_hsv, COLOR_BGR2HSV);
//    cvtColor(image_copy, img_bgra, COLOR_BGR2BGRA);
//    cvtColor(image_copy, img_ycc, COLOR_BGR2YCrCb);
//    
//    Vec3b channels_HSV;
//    Vec3b channels_BGRA;
//    Vec3b channels_YCC;
//
//    // Evaluate every pixel in the three different spaces, paint evertything in black
//    // if a pixel satisfy the constraints paint it in white.
//    for (int i = 0; i < img_hsv.rows; i++) {
//        for (int j = 0; j < img_hsv.cols; j++) {
//            channels_HSV = img_hsv.at<Vec3b>(i, j);
//            channels_BGRA = img_bgra.at<Vec3b>(i, j);
//            channels_YCC = img_ycc.at<Vec3b>(i, j);
//            img_hsv.at<Vec3b>(i, j) = Vec3b(0, 0, 0);     // Paint in black
//
//            if ((0 < channels_HSV[0] && channels_HSV[0] < 50) &&
//                (40 < channels_HSV[1] && channels_HSV[1] < 170) &&      // 0.23<S<0.68
//                (20 < channels_BGRA[0]) && (40 < channels_BGRA[1]) && (95 < channels_BGRA[2]) &&
//                (channels_BGRA[1] < channels_BGRA[2]) && (channels_BGRA[0] < channels_BGRA[2]) &&
//                (channels_BGRA[0] < 220 || channels_BGRA[1] < 220 || channels_BGRA[2] < 220) &&   // Remove white component
//                (15 < abs(channels_BGRA[2] - channels_BGRA[1]))     )// && (15 < channels_BGRA[3]) )
//            {
//                img_hsv.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
//            }
//            else if ((20 < channels_BGRA[0]) && (40 < channels_BGRA[1]) && (95 < channels_BGRA[2]) &&
//                (channels_BGRA[1] < channels_BGRA[2]) && (channels_BGRA[0] < channels_BGRA[2]) &&
//                (15 < abs(channels_BGRA[2] - channels_BGRA[1])) &&          // (15 < channels_BGRA[3]) &&
//                (135 < channels_YCC[1]) && (85 < channels_YCC[2]) && (80 < channels_YCC[0]) &&
//                (channels_YCC[1] < (1.5862 * channels_YCC[2] + 20)) &&
//                (channels_YCC[1] > (0.3448 * channels_YCC[2] + 76.2069)) &&
//                (channels_YCC[1] > (-4.5652 * channels_YCC[2] + 234.5652)) &&
//                (channels_YCC[1] < (-1.15 * channels_YCC[2] + 301.75)) &&
//                (channels_YCC[1] < (-2.2857 * channels_YCC[2] + 432.85)) )
//            {
//                img_hsv.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
//            }
//        }
//    }
//
//    //--------------------------------------------------------------------------------------------------
//    //              Image cropping
//    //--------------------------------------------------------------------------------------------------
//    //unsigned int x_angle;
//    //unsigned int y_angle;
//    //unsigned int width;
//    //unsigned int height;
//    //vector<Mat> image_crop;
//    //vector<string> name_windows = { "Crop 1", "Crop 2", "Crop 3", "Crop 4", "Crop 5" };
//
//    //Mat temp;
//
//    //for (int i = 0; i < window_coord.size(); i++)
//    //{
//    //    x_angle = window_coord[i][0];
//    //    y_angle = window_coord[i][1];
//    //    width = window_coord[i][2];
//    //    height = window_coord[i][3];
//
//    //    Rect crop_window(x_angle, y_angle, width, height);
//
//    //    // Draw it on image
//    //    image_crop.push_back(image_copy(crop_window));
//    //    namedWindow(name_windows[i], WINDOW_NORMAL);
//    //    imshow(name_windows[i], image_crop[i]);
//
//    //    rectangle(image_copy, crop_window, Scalar(255, 0, 255), 2);
//    //}
//
//    //--------------------------------------------------------------------------------------------------
//    //              Print images
//    //--------------------------------------------------------------------------------------------------
//
//    namedWindow("Source", WINDOW_NORMAL);
//    resizeWindow("Source", 740, 540);
//    imshow("Source", image_copy);
//    setMouseCallback("Source", CallBackFunc, (void*)&image_copy);
//
//
//    namedWindow("Mask", WINDOW_NORMAL);
//    resizeWindow("Mask", 740, 540);
//    imshow("Mask", img_hsv);
//
//    waitKey(0);
//}