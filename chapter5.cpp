#include <opencv2/opencv.hpp> 
#include <opencv2/core.hpp> 
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp> 
#include <opencv2/imgcodecs.hpp> 
#include <opencv2/imgproc.hpp> 
#include <stdio.h>

typedef unsigned char uchar;

using namespace std;
using namespace cv;

uchar **uc_alloc(int size_x, int size_y)
{
    uchar **m;
    int i;

    if ((m =(uchar **) calloc(size_y, sizeof(uchar *))) ==NULL)
    {   
        printf("d_alloc error 1\7\n");
        exit(0);
    }

    for (i=0; i<size_y; i++)
        if ((m[i] =(uchar *) calloc(size_x, sizeof(uchar))) ==NULL)
        {
            printf("d_alloc error 2\7\n");
            exit(0);
        }
    return m;
}

void read_ucmatrix(int size_x, int size_y, uchar **ucmatrix, const char *filename)
{
    int i;
    FILE *f;

    if ((f = fopen(filename, "rb")) == NULL)
    {
        printf("%s File open Error!\n", filename);
        exit(0);
    }

    for (i=0; i<size_y; i++)
        if (fread(ucmatrix[i], sizeof(uchar), size_x, f) != size_x)
        {
            printf("Data Read Error!\n"); 
            exit(0);
        }
    fclose(f);
}

void gammaCorrection(const Mat& src, Mat& dst, float gamma)
{
    dst = src.clone();
    for(int i = 0; i < src.rows; i++)
    {
        for(int j = 0; j < src.cols; j++)
        {
            dst.at<uchar>(i, j) = saturate_cast<uchar>(pow((src.at<uchar>(i, j) / 255.0), gamma) * 255.0);
        }
    }
}

int main(int argc, char *argv[])
{
    int i, j, Row, Col;
    uchar **img;
    double totalValue = 0; 

    if (argc != 7)  // 3개의 감마 값을 받기 위해 argc를 7로 설정
    {
        printf("Usage: Exe imgData x_size y_size gamma1 gamma2 gamma3\n");
        exit(0);
    }

    Col = atoi(argv[2]);
    Row = atoi(argv[3]);

    img = uc_alloc(Col, Row);
    read_ucmatrix(Col, Row, img, argv[1]);

    Mat cvImg(Row, Col, CV_8UC1);

    for(i=0; i<Row; i++)
        for(j=0; j<Col; j++)
        {
            cvImg.at<uchar>(i,j) = img[i][j];
            totalValue += img[i][j];
        }


    float gamma1 = atof(argv[4]);
    float gamma2 = atof(argv[5]);
    float gamma3 = atof(argv[6]);

    Mat gammaCorrectedImg1, gammaCorrectedImg2, gammaCorrectedImg3;

    gammaCorrection(cvImg, gammaCorrectedImg1, gamma1);
    gammaCorrection(cvImg, gammaCorrectedImg2, gamma2);
    gammaCorrection(cvImg, gammaCorrectedImg3, gamma3);

    namedWindow("Gamma Corrected Image (Gamma: " + to_string(gamma1) + ")", WINDOW_AUTOSIZE);
    imshow("Gamma Corrected Image (Gamma: " + to_string(gamma1) + ")", gammaCorrectedImg1);

    namedWindow("Gamma Corrected Image (Gamma: " + to_string(gamma2) + ")", WINDOW_AUTOSIZE);
    imshow("Gamma Corrected Image (Gamma: " + to_string(gamma2) + ")", gammaCorrectedImg2);

    namedWindow("Gamma Corrected Image (Gamma: " + to_string(gamma3) + ")", WINDOW_AUTOSIZE);
    imshow("Gamma Corrected Image (Gamma: " + to_string(gamma3) + ")", gammaCorrectedImg3);

    waitKey(0);

    // 프로그램 종료 전에 동적 메모리 해제
    for(i = 0; i < Row; i++)
        free(img[i]);
    free(img);

    return 0;
}