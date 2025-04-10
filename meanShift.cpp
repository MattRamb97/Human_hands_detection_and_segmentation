#include "TMT_Final_Project.h"

using namespace cv;
using namespace std;


void meanShift() {

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

    //--------------------------------------------------------------------------------------------------
    //              Sharpening
    //--------------------------------------------------------------------------------------------------

    Mat image_copy = src.clone();
    Mat sharp_image;
    
    GaussianBlur(image_copy, sharp_image, cv::Size(0, 0), 3);
    addWeighted(image_copy, 1.5, sharp_image, -0.5, 0, sharp_image);

    // Blurring
    //Mat blur_image;
    //int Gaussian_kernel = 5;
    //blur(image_out, blur_image, Size(Gaussian_kernel, Gaussian_kernel));

    //--------------------------------------------------------------------------------------------------
    //              Mean Shift
    //--------------------------------------------------------------------------------------------------

    Mat seg_image;
    pyrMeanShiftFiltering(sharp_image, seg_image, 20, 30, 5);

    //--------------------------------------------------------------------------------------------------
    //              Image cropping
    //--------------------------------------------------------------------------------------------------
    unsigned int x_angle;
    unsigned int y_angle;
    unsigned int width;
    unsigned int height;
    vector<Mat> image_crop;
    vector<Mat> image_crop_bnw;
    vector<Mat> otsu_crop;
    vector<string> name_windows = {"Crop 1", "Crop 2", "Crop 3", "Crop 4", "Crop 5"};
    vector<string> name_windows_bnw = { "Crop 1 bnw", "Crop 2 bnw", "Crop 3 bnw", "Crop 4 bnw", "Crop 5 bnw" };

    Mat temp,temp1;


    for (int i = 0; i < window_coord.size(); i++)
    {
        x_angle = window_coord[i][0];
        y_angle = window_coord[i][1];
        width = window_coord[i][2];
        height = window_coord[i][3];

        Rect crop_window(x_angle, y_angle, width, height);

        // Draw it on image
        image_crop.push_back(seg_image(crop_window));
        cvtColor(seg_image(crop_window), temp, COLOR_BGR2GRAY);
        image_crop_bnw.push_back(temp);                  
 
        rectangle(image_copy, crop_window, Scalar(255, 0, 255), 2);

        //--------------------------------------------------------------------------------------------------
        //              Multi Otsu for every cropped image
        //--------------------------------------------------------------------------------------------------
        //equalizeHist(image_crop_bnw[i], image_crop_bnw[i]);

        int histogram[256] = { 0 };
        int pixelsCount = image_crop_bnw[i].cols * image_crop_bnw[i].rows;

        for (int y = 0; y < image_crop_bnw[i].rows; y++)
        {
            for (int x = 0; x < image_crop_bnw[i].cols; x++)
            {
                uchar value = image_crop_bnw[i].at<uchar>(y, x);
                histogram[value]++;
            }
        }

        double c = 0;
        double Mt = 0;

        double p[256] = { 0 };
        for (int k = 0; k < 256; k++)
        {
            p[k] = (double)histogram[k] / (double)pixelsCount;
            Mt += k * p[k];
        }

        int optimalTreshold1 = 0;
        int optimalTreshold2 = 0;
        //int optimalTreshold3 = 0;

        double maxBetweenVar = 0;

        double w0 = 0;
        double m0 = 0;
        double c0 = 0;      // interclass variance
        double p0 = 0;

        double w1 = 0;
        double m1 = 0;
        double c1 = 0;
        double p1 = 0;

        double w2 = 0;
        double m2 = 0;
        double c2 = 0;
        double p2 = 0;
        for (int tr1 = 0; tr1 < 256; tr1++)
        {
            p0 += p[tr1];
            w0 += (tr1 * p[tr1]);
            if (p0 != 0)
            {
                m0 = w0 / p0;
            }

            c0 = p0 * (m0 - Mt) * (m0 - Mt);

            c1 = 0;
            w1 = 0;
            m1 = 0;
            p1 = 0;
            for (int tr2 = tr1 + 1; tr2 < 256; tr2++)
            {

                p1 += p[tr2];
                w1 += (tr2 * p[tr2]);
                if (p1 != 0)
                {
                    m1 = w1 / p1;
                }

                c1 = p1 * (m1 - Mt) * (m1 - Mt);


                c2 = 0;
                w2 = 0;
                m2 = 0;
                p2 = 0;

                for (int tr3 = tr2 + 1; tr3 < 256; tr3++)
                {

                    p2 += p[tr3];
                    w2 += (tr3 * p[tr3]);
                    if (p2 != 0)
                    {
                        m2 = w2 / p2;
                    }

                    c2 = p2 * (m2 - Mt) * (m2 - Mt);

                    // double p3 = 1 - (p0 + p1 + p2);
                    // double w3 = Mt - (w0 + w1 + w2);
                    // double m3 = w3 / p3;
                    // double c3 = p3 * (m3 - Mt) * (m3 - Mt);

                    //double c = c0 + c1 + c2 + c3;
                    double c = c0 + c1 + c2;

                    if (maxBetweenVar < c)
                    {
                        maxBetweenVar = c;
                        optimalTreshold1 = tr1;
                        optimalTreshold2 = tr2;
                        //optimalTreshold3 = tr3;
                    }
                }
            }
        }

        cout << optimalTreshold1 << "  " << optimalTreshold2 << endl;
        
        inRange(image_crop_bnw[i], Scalar(optimalTreshold1), Scalar(optimalTreshold2), temp1);
        otsu_crop.push_back(temp1);

        namedWindow(name_windows_bnw[i], WINDOW_NORMAL);
        imshow(name_windows_bnw[i], otsu_crop[i]);

        namedWindow(name_windows[i], WINDOW_NORMAL);
        imshow(name_windows[i], image_crop[i]);

    }
    //--------------------------------------------------------------------------------------------------
    //              Print images
    //--------------------------------------------------------------------------------------------------

    namedWindow("Source", WINDOW_NORMAL);
    resizeWindow("Source", 740, 540);
    imshow("Source", image_copy);

    namedWindow("Mean Shift", WINDOW_NORMAL);
    resizeWindow("Mean Shift", 740, 540);
    imshow("Mean Shift", seg_image);



    waitKey(0);
}