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

    if ((f = fopen(filename, "rb")) == NULL)  // fixed here
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

void write_ucmatrix(int size_x, int size_y, uchar **ucmatrix, const char *filename)
{
    int i;
    FILE *f;

    if ((f = fopen(filename, "wb")) == NULL)  // fixed here
    {
        printf("%s File open Error!\n", filename);
        exit(0);
    }

    for (i=0; i<size_y; i++)
        if (fwrite(ucmatrix[i], sizeof(uchar), size_x, f) != size_x)
        {
            printf("Data Write Error!\n");
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

    img = uc_alloc(Col, Row);  // fixed here
    read_ucmatrix(Col, Row, img, argv[1]);

    Mat cvImg(Row, Col, CV_8UC1);  // You can use CV_8UC1 instead of CV_8UC(1)

    for(i=0; i<Row; i++)
        for(j=0; j<Col; j++)
            cvImg.at<uchar>(i,j) = img[i][j];

    namedWindow(argv[1], WINDOW_AUTOSIZE);
    imshow(argv[1], cvImg);
    waitKey(0);

    return 0;
}
