#include <opencv2/opencv.hpp> 
#include <opencv2/core.hpp> 
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp> 
#include <opencv2/imgcodecs.hpp> 
#include <opencv2/imgproc.hpp> 
#include <stdio.h>
#include <bitset>
#include <limits>

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

Mat LoadCustomImage(const char *filename, int width, int height)
{
    uchar **ucmatrix = uc_alloc(width, height);
    read_ucmatrix(width, height, ucmatrix, filename);
    
    Mat image(height, width, CV_8UC1);
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            image.at<uchar>(i, j) = ucmatrix[i][j];


    for (int i = 0; i < height; i++)
        free(ucmatrix[i]);
    free(ucmatrix);

    return image;
}

using namespace std;
using namespace cv;

void BitPlaneSynthesis(const Mat& src, Mat& dst)
{
    dst = Mat::zeros(src.size(), CV_8UC1); // 동일한 크기의 검은색 영상 생성
    for(int i = 0; i < src.rows; i++)
    {
        for(int j = 0; j < src.cols; j++)
        {
            uchar pixelValue = src.at<uchar>(i, j);
            uchar synthesizedValue = 0;
            
            if(pixelValue & (1 << 7)) 
                synthesizedValue |= (1 << 7);
            if(pixelValue & (1 << 6)) 
                synthesizedValue |= (1 << 6);
            if(pixelValue & (1 << 5)) 
                synthesizedValue |= (1 << 5);
            if(pixelValue & (1 << 4)) 
                synthesizedValue |= (1 << 4);

            dst.at<uchar>(i, j) = synthesizedValue;
        }
    }
}
void BitPlaneSynthesis(const Mat& src, Mat& dst, int bit)
{
    dst = Mat::zeros(src.size(), CV_8UC1); 
    for(int i = 0; i < src.rows; i++)
    {
        for(int j = 0; j < src.cols; j++)
        {
            uchar pixelValue = src.at<uchar>(i, j);
            uchar synthesizedValue = 0;

            if(pixelValue & (1 << bit))  //비트값에 대한 변환 수정 
                synthesizedValue = 255;  // 비트가 1이면 값은 255로 설정


            dst.at<uchar>(i, j) = synthesizedValue;
        }
    }
}


int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("Usage: ./YourProgramName <input_image_path> <output_image_path>\n");
        return -1;
    }

    Mat src = LoadCustomImage(argv[1], 256, 256);
    if(src.empty())
    {
        printf("Could not open or find the image.\n");
        return -1;
    }

    for(int bit = 0; bit < 8; bit++)
    {
        Mat synthesizedImage;
        BitPlaneSynthesis(src, synthesizedImage, bit);

        string windowName = "Bit plane " + to_string(bit);
        namedWindow(windowName, WINDOW_AUTOSIZE);
        imshow(windowName, synthesizedImage);
    }

    waitKey(0); 

    return 0;
}