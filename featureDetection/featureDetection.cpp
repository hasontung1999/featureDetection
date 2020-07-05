#include "Method.h"

int main(int argc, char** argv)
{
	///// Load source image and convert it to gray
	//srcGray = imread("leaf.jpg", IMREAD_GRAYSCALE);
	//srcRGB = imread("leaf.jpg", IMREAD_COLOR);

	///// Create a window and a trackbar
	//namedWindow(source_window, WINDOW_AUTOSIZE);
	//imshow(source_window, srcRGB);

	///*vector<float> kernel = createNormLoG(3, 3, 1 / sqrt(2));
	//for (int i = 0; i < kernel.size(); i++)
	//{
	//    cout << kernel[i]<<" ";
	//}*/
	//Mat dst;
	///*detectHarris(srcGray, dst,2000);
	//for (int i = 0; i < dst.rows * dst.cols; i++)
	//{
	//    int x = i / dst.cols;
	//    int y = i % dst.cols;
	//    if (dst.at<uchar>(x,y)!=0)
	//    {
	//        circle(srcRGB, Point(y, x), 2, Scalar(0,0,255), 2, 8, 0);
	//    }
	//}*/
	//detectBlob(srcGray, dst, 8);
	//
	///// Showing the result
	//namedWindow(corners_window, WINDOW_AUTOSIZE);
	//imshow(corners_window, dst);
	if (argc != 3)
	{
		cout << "Chuong trinh phat hien dac trung cua anh" << endl;
		cout << "<tenchuongtrinh><duongdantaptinanh><malenh>" << endl;
		cout << "Ma lenh: " << endl << "1. Harris"
			<< endl << "2. Blob" << endl << "3. DoG" << endl <<
			"4. SIFT" << endl;
		return -1;
	}
	Mat image;
	image = imread(argv[1], IMREAD_GRAYSCALE);
	if (!image.data)
	{
		cout << "Khong the mo anh" << std::endl;
		return -1;
	}
	namedWindow("Display window", WINDOW_AUTOSIZE);
	imshow("Display window", image);

	Mat dstImg = image.clone();

	if (strcmp(argv[2], "1") == 0)
	{
		detectHarris(image, dstImg, 2000);
		imshow("Harris output", dstImg);
	}
	if (strcmp(argv[2], "2") == 0)
	{
		detectBlob(image, dstImg, 8);
		imshow("Blob output", dstImg);
	}
	if (strcmp(argv[2], "3") == 0)
	{
		detectDOG(image);
		imshow("DOG output", dstImg);
	}
	if (strcmp(argv[2], "4") == 0)
	{
		matchBySIFT(image, dstImg, 0);
		imshow("SIFT output", dstImg);
	}

	waitKey(0);
	return(0);
}
