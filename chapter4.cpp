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


int main(int argc, char *argv[])
{
    int i, j, Row, Col;
    uchar **img;

    if (argc != 4)
    {
        printf("Exe imgData x_size y_size \n");
        exit(0);
    }

    Col = atoi(argv[2]);
    Row = atoi(argv[3]);

    img = uc_alloc(Col, Row);
    read_ucmatrix(Col, Row, img, argv[1]);

    Mat cvImg(Row, Col, CV_8UC1);

    for(i=0; i<Row; i++)
        for(j=0; j<Col; j++)
            cvImg.at<uchar>(i,j) = img[i][j];

    // 모자이크 처리
    int mosaicSize = 15;
    Mat mosaicImg;
    resize(cvImg, mosaicImg, Size(cvImg.cols/mosaicSize, cvImg.rows/mosaicSize));
    resize(mosaicImg, mosaicImg, Size(cvImg.cols, cvImg.rows));

    namedWindow("Mosaic Image", WINDOW_AUTOSIZE);
    imshow("Mosaic Image", mosaicImg);

    // Negative 처리
    Mat negativeImg;
    bitwise_not(cvImg, negativeImg);

// 평균과 표준편차 계산
Scalar mean, stddev;
meanStdDev(cvImg, mean, stddev);

// 분산은 표준편차의 제곱이므로 아래와 같이 계산
double variance = stddev[0] * stddev[0];

// 평균값과 분산값을 문자열로 변환
char meanText[100], varianceText[100];
snprintf(meanText, sizeof(meanText), "everage: %f", mean[0]);
snprintf(varianceText, sizeof(varianceText), "bunsan: %f", variance);

// 문자열을 영상에 출력
putText(cvImg, meanText, Point(10,30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,255), 2);
putText(cvImg, varianceText, Point(10,60), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,255), 2);


    namedWindow("Original Image", WINDOW_AUTOSIZE);
    imshow("Original Image", cvImg);
    namedWindow("Negative Image", WINDOW_AUTOSIZE);
    imshow("Negative Image", negativeImg);

    waitKey(0);

    return 0;
}
