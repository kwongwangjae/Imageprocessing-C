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

    if (argc != 4)  // 입력 이미지, x_size, y_size만 받음
    {
        printf("Usage: Exe imgData x_size y_size\n");  // <-- 여기를 수정
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

    // 감마값을 조절하면서 평균 값을 확인하는 루프
    float gamma = 0.1;  // 시작 감마 값
    float step = 0.01;  // 감마 값의 변화량
    Mat gammaCorrectedImg;
    Scalar mean, stddev;

    while(true)
    {
        gammaCorrection(cvImg, gammaCorrectedImg, gamma);
        meanStdDev(gammaCorrectedImg, mean, stddev);
        
        if (mean[0] >= 127 && mean[0] <= 129)  // 평균 값이 원하는 범위에 들어오면 루프 종료
        {
            break;
        }

        gamma += step;  // 아니면 감마값 조절
        if (gamma > 10) // 감마 값이 너무 크면 종료 (선택적)
        {
            printf("Cannot find appropriate gamma within limits.\n");
            exit(0);
        }
    }

    printf("Appropriate gamma value: %f with mean: %f\n", gamma, mean[0]);

    namedWindow("Gamma Corrected Image (Gamma: " + to_string(gamma) + ")", WINDOW_AUTOSIZE);
    imshow("Gamma Corrected Image (Gamma: " + to_string(gamma) + ")", gammaCorrectedImg);

    waitKey(0);

    // 메모리 해제
    for(i = 0; i < Row; i++)
        free(img[i]);
    free(img);

    return 0;
}
