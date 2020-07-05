#include "Method.h"

void castUchar(Mat& img)
{
    for (int i = 0; i < img.rows * img.cols; i++)
    {
        int x = i / img.cols;
        int y = i % img.cols;
        int value = img.at<uchar>(x, y);
        img.at<uchar>(x, y) = value > 255 ? 255 : value;
    }
}

vector<float> createGaussKernel(int height, int width, float sigma)
{
    // Áp dụng công thức Gaussian để tạo ma trận Gaussian
    vector<float>kernel;
    float inverse_2Pi_SigmaSquare;
    float inverse_2_SigmaSquare;
    int	indexRow, indexCol;

    inverse_2Pi_SigmaSquare = 1 / (2 * PI * sigma * sigma);
    inverse_2_SigmaSquare = 1 / (2 * sigma * sigma);
    int midX = height / 2;
    int midY = width / 2;
    float value;
    for (indexRow = 0; indexRow < height; indexRow++)
    {
        for (indexCol = 0; indexCol < width; indexCol++)
        {
            value = inverse_2Pi_SigmaSquare
                * exp(-((indexRow - midX) * (indexRow - midX) + (indexCol - midY) * (indexCol - midY))
                    * inverse_2_SigmaSquare);
            kernel.push_back(value);
        }
    }
    return kernel;
}

void convolutionKernel3(Mat& src, Mat& dst, vector<float> kernel)
{
    float magnitude;
    dst = src.clone();
    for (int x = 1; x < src.rows - 1; x++)
    {
        for (int y = 1; y < src.cols - 1; y++)
        {
            magnitude = abs(src.at<uchar>(x - 1, y - 1) * kernel[0] +
                src.at<uchar>(x, y - 1) * kernel[1] +
                src.at<uchar>(x + 1, y - 1) * kernel[2] +
                src.at<uchar>(x - 1, y) * kernel[3] +
                src.at<uchar>(x, y) * kernel[4] +
                src.at<uchar>(x + 1, y) * kernel[5] +
                src.at<uchar>(x - 1, y + 1) * kernel[6] +
                src.at<uchar>(x, y + 1) * kernel[7] +
                src.at<uchar>(x + 1, y + 1) * kernel[8]);
            dst.at<uchar>(x, y) = magnitude > 255 ? 255 : magnitude;
        }
    }
}

void sobelGradient(Mat& src, Mat& dst, int xAxis, int yAxis)
{
    dst = src.clone();
    Mat dstX = src.clone();
    Mat dstY = src.clone();
    float magnitude;
    if (xAxis == 1 && yAxis == 0)
    {
        convolutionKernel3(src, dst, sobelX);
    }
    else if (xAxis == 0 && yAxis == 1)
    {
        convolutionKernel3(src, dst, sobelY);
    }
    else if (xAxis == 1 && yAxis == 1)
    {
        convolutionKernel3(src, dstX, sobelX);
        convolutionKernel3(src, dstY, sobelY);
        for (int x = 1; x < src.rows - 1; x++)
        {
            for (int y = 1; y < src.cols - 1; y++)
            {
                magnitude = sqrt(dstX.at<uchar>(x, y) * dstX.at<uchar>(x, y) + dstY.at<uchar>(x, y) * dstY.at<uchar>(x, y));
                magnitude = magnitude > 255 ? 255 : magnitude;
                dst.at<uchar>(x, y) = magnitude;
            }
        }
    }

}

vector<float> solveEquation2(int a, int b, int c)
{
    vector<float> result;
    if (a == 0) return result;
    float delta = b * b - 4 * a * c;
    if (delta < 0) return result;
    result.push_back((-b + sqrt(delta)) / (2 * a));
    result.push_back((-b - sqrt(delta)) / (2 * a));
    return result;
}

vector<float> calcEigenValue(vector<int> arr)
{
    vector<float> eigenValue;
    int a, b, c;
    a = 1;
    b = -(arr[0] + arr[3]);
    c = arr[0] * arr[3] - arr[1] * arr[2];
    eigenValue = solveEquation2(a, b, c);
    return eigenValue;
}

bool isMax(Mat img, int x, int y, int sizeWindow)
{
    int max = 0;
    int value;
    for (int i = -sizeWindow / 2; i < sizeWindow / 2; i++)
    {
        for (int j = -sizeWindow / 2; j < sizeWindow / 2; j++)
        {
            int xIndex, yIndex;
            xIndex = x + i;
            yIndex = y + j;
            if (x + i < 0)
                xIndex = 0;
            else if (x + i >= img.rows)
                xIndex = img.rows - 1;
            if (y + j < 0)
                yIndex = 0;
            else if (y + j >= img.cols)
                yIndex = img.cols - 1;
            value = img.at<uchar>(xIndex, yIndex);
            if (value > max)
                max = value;
        }
    }
    if (max != 0 && max == img.at<uchar>(x, y))
        return true;
    return false;
}

void detectHarris(Mat& src, Mat& dst, int threshHold)
{
    Mat xGrad, yGrad;
    Mat xGradSquare, yGradSquare, xyGrad;
    Mat xGradSquareGauss, yGradSquareGauss, xyGradGauss;
    Mat R = src.clone();
    dst = src.clone();

    //Calc x and y derivative of srcImg via Sobel
    sobelGradient(src, xGrad, 1, 0);
    sobelGradient(src, yGrad, 0, 1);

    // Calc Ix_square, Iy_square and Ixy
    pow(xGrad, 2, xGradSquare);
    //castUchar(xGradSquare);
    pow(yGrad, 2, yGradSquare);
    //castUchar(yGradSquare);
    multiply(xGrad, yGrad, xyGrad);
    //castUchar(xyGrad);

    //Gaussian window
    vector<float>gaussKernel = createGaussKernel(3, 3, 1);
    convolutionKernel3(xGradSquare, xGradSquareGauss, gaussKernel);
    convolutionKernel3(yGradSquare, yGradSquareGauss, gaussKernel);
    convolutionKernel3(xyGrad, xyGradGauss, gaussKernel);

    //Calc coeff R
    int x, y, det, tr, value;
    for (int i = 0; i < src.rows * src.cols; i++)
    {
        x = i / src.cols;
        y = i % src.cols;
        det = xGradSquareGauss.at<uchar>(x, y) * yGradSquareGauss.at<uchar>(x, y)
            - xyGradGauss.at<uchar>(x, y) * xyGradGauss.at<uchar>(x, y);
        tr = xGradSquareGauss.at<uchar>(x, y) + yGradSquareGauss.at<uchar>(x, y);
        value = det - k * tr * tr;
        R.at<uchar>(x, y) = value > threshHold ? value : 0;
    }

    //Find max in local window
    for (int i = 0; i < R.rows * R.cols; i++)
    {
        x = i / R.cols;
        y = i % R.cols;
        if (isMax(R, x, y, 3))
        {
            circle(dst, Point(y, x), 2, Scalar(0));
        }
    }
}

vector<float> createNormLoG(int height, int width, float sigma)
{
    // Áp dụng công thức chuẩn hóa Laplace of Gaussian để tạo ma trận norm LoG
    vector<float>kernel;
    float inverse_2Pi_Sigma4;
    float inverse_2_SigmaSquare;

    inverse_2Pi_Sigma4 = 1 / (2 * PI * sigma * sigma * sigma * sigma);
    inverse_2_SigmaSquare = 1 / (2 * sigma * sigma);
    int midX = height / 2;
    int midY = width / 2;
    float value;
    int	indexRow, indexCol;
    for (indexRow = 0; indexRow < height; indexRow++)
    {
        for (indexCol = 0; indexCol < width; indexCol++)
        {
            int x2_add_y2 = (indexRow - midX) * (indexRow - midX) + (indexCol - midY) * (indexCol - midY);
            value = inverse_2Pi_Sigma4 * (x2_add_y2 - 2 * sigma * sigma)
                * exp(-x2_add_y2 * inverse_2_SigmaSquare);
            kernel.push_back(value);
        }
    }
    return kernel;
}

int findMax(Mat img, int x, int y, int sizeWindow)
{
    int max = 0;
    int value;
    for (int i = -sizeWindow / 2; i < sizeWindow / 2; i++)
        for (int j = -sizeWindow / 2; j < sizeWindow / 2; j++)
        {
            int xIndex, yIndex;
            xIndex = x + i;
            yIndex = y + j;
            if (x + i < 0)
                xIndex = 0;
            else if (x + i >= img.rows)
                xIndex = img.rows - 1;
            if (y + j < 0)
                yIndex = 0;
            else if (y + j >= img.cols)
                yIndex = img.cols - 1;

            value = img.at<uchar>(xIndex, yIndex);
            if (value > max)
                max = value;
        }
    return max;
}

bool isMaxIn3Img(vector<Mat> img, int i, int x, int y, int sizeWindow)
{
    int temp1, temp2;
    if (!isMax(img[i], x, y, sizeWindow))
        return false;
    int currentValue = img[i].at<uchar>(x, y);
    if (i == 0)
    {
        temp1 = findMax(img[i + 1], x, y, sizeWindow);
        if (currentValue >= temp1)
            return true;
        return false;
    }
    if (i == img.size() - 1)
    {
        temp1 = findMax(img[i - 1], x, y, sizeWindow);
        if (currentValue >= temp1)
            return true;
        return false;
    }

    temp1 = findMax(img[i - 1], x, y, sizeWindow);
    temp2 = findMax(img[i + 1], x, y, sizeWindow);
    if ((currentValue < temp1 || currentValue < temp2))
        return false;
    return true;
}

void convolution(Mat& src, Mat& dst, vector<float>kernel, int sizeWindow)
{
    dst = Mat::zeros(src.size(), src.type());
    int size = kernel.size();
    for (int i = 0; i < src.cols * src.rows; i++)
    {
        int sum = 0;
        int xIndex = i / src.cols;
        int yIndex = i % src.cols;
        for (int j = 0; j < size; j++)
        {
            int x = j / sizeWindow;
            int y = j % sizeWindow;

            int x_ = xIndex + x - sizeWindow + 1;
            int y_ = yIndex + y - sizeWindow + 1;

            if (x_ < 0)
                x_ = 0;
            else if (x_ >= src.cols)
                x_ = src.cols - 1;
            if (y_ < 0)
                y_ = 0;
            else if (y_ >= src.rows)
                y_ = src.rows - 1;
            sum += src.at<uchar>(xIndex, yIndex) * kernel[j];
        }
        dst.at<uchar>(xIndex, yIndex) = sum;
    }
}
//Hàm detect blob
void detectBlob(Mat& src, Mat& dst, int numOfConvolution)
{
    //dst = src.clone();
    dst = Mat::zeros(src.size(), src.type());
    //cvtColor(src, dst, COLOR_GRAY2BGR);
    //Mat radius=Mat::zeros(Size(src.rows,src.cols),src.type());
    //Mat mask= Mat::zeros(Size(src.rows, src.cols), src.type());
    //vector<vector<float>> kernelLoG;
    vector<Mat> imgLoG;
    Mat temp;
    for (int i = 0; i < numOfConvolution; i++)
    {
        vector<float> kernel = createNormLoG(2 * (i + 1) + 1, 2 * (i + 1) + 1, (i + 1) / sqrt(2));
        convolution(src, temp, kernel, 2 * (i + 1) + 1);
        imgLoG.push_back(temp);
    }
    for (int i = 0; i < src.rows * src.cols; i++)
    {
        int x = i / src.cols;
        int y = i % src.cols;
        //cout << x << "," << y << endl;
        for (int j = 0; j < numOfConvolution; j++)
        {
            //cout << j;
            if (isMaxIn3Img(imgLoG, j, x, y, 3))
            {
                //mask.at<uchar>(x,y)=1;
                //radius.at<uchar>(x, y) = j + 1;
                //circle(dst, Point(y, x), j+1, Scalar(0,0,255));// , 1, 8, 0);;
                dst.at<uchar>(x, y) = 255;
            }
        }
    }
}

void detectDOG(Mat& src)
{}
void matchBySIFT(Mat& img1, Mat& img2, int detector)
{}