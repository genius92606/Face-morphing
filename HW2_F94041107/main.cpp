#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cstdlib>
#include <cmath>
#include <iostream>


using namespace cv;
using namespace std;



// Matrices to store images
Mat img1;
Mat img2;
Mat button_img;


Mat DispImage = Mat::zeros(Size(795, 390), CV_8UC3);    //The final display image




Point des1, des2, source1, source2;                     //Destination points and source points
Point des_initial(95, 70), source_initial(445, 70), blend_initial(280,330);     //Document the top-left point
Point morph[255][189];



bool des1_true, des2_true, source1_true, source2_true = true;  //check if the user draw the correct arrow
bool button_true = false;									   //check if button clicked

int des_arrow = 0, source_arrow = 0;
//allocate some memories for point of warping
Point des_start[30], des_end[30],source_start[30],source_end[30];


Point perpendicular(Point haha) {
	
	Point transform(haha.y, -haha.x);
	return transform;
}

void StartToMorph() {

	namedWindow("T=0", 1); namedWindow("T=1", 1); namedWindow("T=0.25", 1); namedWindow("T=0.5", 1); namedWindow("T=0.75", 1);
	Mat Img2ToImg1_quarter, Img1ToImg2_quarter, Img2ToImg1_quarter_back, Img1ToImg2_quarter_back, Img2ToImg1_half, Img1ToImg2_half;
	Mat blend_quarter, blend_half, blend_quarter_back;
	Point destination_temp, source_temp, psum;   
	double u, v, weight,wsum;					   
	int line_ammount;
	Mat mapx_quarter, mapy_quarter, mapx_half, mapy_half, mapx_quarter_back, mapy_quarter_back;  //after warping of 0.25/0.5/0.75
	Mat mapx_quarter_2, mapy_quarter_2, mapx_half_2, mapy_half_2, mapx_quarter_back_2, mapy_quarter_back_2;  //after warping of 0.25/0.5/0.75
	mapx_quarter.create(img1.size(), CV_32FC1);mapy_quarter.create(img1.size(), CV_32FC1);
	mapx_half.create(img1.size(), CV_32FC1);mapy_half.create(img1.size(), CV_32FC1);
	mapx_quarter_back.create(img1.size(), CV_32FC1);mapy_quarter_back.create(img1.size(), CV_32FC1);
	mapx_quarter_2.create(img1.size(), CV_32FC1); mapy_quarter_2.create(img1.size(), CV_32FC1);
	mapx_half_2.create(img1.size(), CV_32FC1); mapy_half_2.create(img1.size(), CV_32FC1);
	mapx_quarter_back_2.create(img1.size(), CV_32FC1); mapy_quarter_back_2.create(img1.size(), CV_32FC1);
	//img2->img1
	for (int x = 0; x < 255; x++) {
		for (int y = 0; y < 189; y++) {

			destination_temp.x = x; destination_temp.y = y; psum.x = 0; psum.y = 0; u = 0; v = 0; weight = 0; wsum = 0.0;
			line_ammount = (des_arrow > source_arrow) ? source_arrow : des_arrow;

			for (int z = 0; z < line_ammount; z++) {
				u = ((double)((des_end[z] - des_start[z]).x*(destination_temp - des_start[z]).x+ 
						  (des_end[z] - des_start[z]).y*(destination_temp - des_start[z]).y))/
							(pow((double)(((des_end[z]-des_start[z]).x)),2)+ pow((double)(((des_end[z] - des_start[z]).y)), 2));
				v=((double)((destination_temp-des_start[z]).x*(des_end[z]-des_start[z]).y-
						 (destination_temp - des_start[z]).y*(des_end[z] - des_start[z]).x))/
							(pow((pow((double)(((des_end[z] - des_start[z]).x)), 2) + pow((double)(((des_end[z] - des_start[z]).y)), 2)),0.5));
				source_temp = source_start[z] + u*(source_end[z]-source_start[z])+v*perpendicular(source_end[z]-source_start[z])/
					(pow((pow((double)(((source_end[z] - source_start[z]).x)), 2) + pow((double)(((source_end[z] - source_start[z]).y)), 2)), 0.5));
				weight = (pow((pow((double)(((source_end[z] - source_start[z]).x)), 2) + pow((double)(((source_end[z] - source_start[z]).y)), 2)), 0.25))/
								(1+abs(v));
				psum = psum + source_temp * weight;
				wsum = wsum + weight;
				//cout << "source temp: " << source_temp << ", psum: " << psum << ", wsum: " << wsum << ", u: " << u << ", v: " << v << ", weight: " << weight << endl;

			}
			/*if ((psum / wsum).x < 0)
				morph[x][y].x = 0;
			else if ((psum / wsum).x > 255)
				morph[x][y].x = 255;
			else
				morph[x][y].x = (psum / wsum).x;
			if ((psum / wsum).y < 0)
				morph[x][y].y = 0;
			else if ((psum / wsum).y > 255)
				morph[x][y].y = 255;
			else
				morph[x][y].y = (psum / wsum).y;*/
			//morph[x][y] = ((psum / wsum) - destination_temp)*0.5 + destination_temp;
			mapx_quarter.at<float>(y, x) = (((psum / wsum) - destination_temp)*0.75 + destination_temp).x;
			mapy_quarter.at<float>(y, x) = (((psum / wsum) - destination_temp)*0.75 + destination_temp).y;
			mapx_half.at<float>(y, x) = (((psum / wsum) - destination_temp)*0.5 + destination_temp).x;
			mapy_half.at<float>(y, x) = (((psum / wsum) - destination_temp)*0.5 + destination_temp).y;
			mapx_quarter_back.at<float>(y, x) = (((psum / wsum) - destination_temp)*0.25 + destination_temp).x;
			mapy_quarter_back.at<float>(y, x) = (((psum / wsum) - destination_temp)*0.25 + destination_temp).y;

		}
	}

	
	
	remap(img2, Img2ToImg1_quarter, mapx_quarter, mapy_quarter, INTER_LINEAR);
	remap(img2, Img2ToImg1_half, mapx_half, mapy_half, INTER_LINEAR);
	remap(img2, Img2ToImg1_quarter_back, mapx_quarter_back, mapy_quarter_back, INTER_LINEAR);
	
	

	//img1->img2
	for (int x = 0; x < 255; x++) {
		for (int y = 0; y < 189; y++) {

			destination_temp.x = x; destination_temp.y = y; psum.x = 0; psum.y = 0; u = 0; v = 0; weight = 0; wsum = 0.0;

			for (int z = 0; z < line_ammount; z++) {
				u = ((double)((source_end[z] - source_start[z]).x*(destination_temp - source_start[z]).x +
					(source_end[z] - source_start[z]).y*(destination_temp - source_start[z]).y)) /
					(pow((double)(((source_end[z] - source_start[z]).x)), 2) + pow((double)(((source_end[z] - source_start[z]).y)), 2));
				v = ((double)((destination_temp - source_start[z]).x*(source_end[z] - source_start[z]).y -
					(destination_temp - source_start[z]).y*(source_end[z] - source_start[z]).x)) /
					(pow((pow((double)(((source_end[z] - source_start[z]).x)), 2) + pow((double)(((source_end[z] - source_start[z]).y)), 2)), 0.5));
				source_temp = des_start[z] + u * (des_end[z] - des_start[z]) + v * perpendicular(des_end[z] - des_start[z]) /
					(pow((pow((double)(((des_end[z] - des_start[z]).x)), 2) + pow((double)(((des_end[z] - des_start[z]).y)), 2)), 0.5));
				weight = (pow((pow((double)(((des_end[z] - des_start[z]).x)), 2) + pow((double)(((des_end[z] - des_start[z]).y)), 2)), 0.25)) /
					(1 + abs(v));
				psum = psum + source_temp * weight;
				wsum = wsum + weight;
				//cout << "source temp: " << source_temp << ", psum: " << psum << ", wsum: " << wsum << ", u: " << u << ", v: " << v << ", weight: " << weight << endl;

			}
			
			//morph[x][y] = psum / wsum;
			mapx_quarter_2.at<float>(y, x) = (((psum / wsum) - destination_temp)*0.25 + destination_temp).x;
			mapy_quarter_2.at<float>(y, x) = (((psum / wsum) - destination_temp)*0.25 + destination_temp).y;
			mapx_half_2.at<float>(y, x) = (((psum / wsum) - destination_temp)*0.5 + destination_temp).x;
			mapy_half_2.at<float>(y, x) = (((psum / wsum) - destination_temp)*0.5 + destination_temp).y;
			//mapx_quarter_back_2.at<float>(y, x) = (((psum / wsum) - destination_temp)*0.75 + destination_temp).x;
			//mapy_quarter_back_2.at<float>(y, x) = (((psum / wsum) - destination_temp)*0.75 + destination_temp).y;
			mapx_quarter_back_2.at<float>(y, x) = (psum / wsum).x;
			mapy_quarter_back_2.at<float>(y, x) = (psum / wsum).y;
		}
	}

	
	remap(img1, Img1ToImg2_quarter, mapx_quarter_2, mapy_quarter_2, INTER_LINEAR);
	remap(img1, Img1ToImg2_half, mapx_half_2, mapy_half_2, INTER_LINEAR);
	remap(img1, Img1ToImg2_quarter_back, mapx_quarter_back_2, mapy_quarter_back_2, INTER_LINEAR);
	
	addWeighted(Img1ToImg2_quarter, 0.75, Img2ToImg1_quarter, 0.25, 0.0, blend_quarter);
	addWeighted(Img1ToImg2_half, 0.5, Img2ToImg1_half, 0.5, 0.0, blend_half);
	addWeighted(Img1ToImg2_quarter_back, 0.25, Img2ToImg1_quarter_back, 0.75, 0.0, blend_quarter_back);

	Rect ROI(280, 330, 255, 189);
	//blend.copyTo(DispImage(ROI));
	imshow("T=0", img1);
	imshow("T=0.25", blend_quarter);
	imshow("T=0.5", blend_half);
	imshow("T=0.75", blend_quarter_back);
	imshow("T=1", img2);
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	
	if (x >= 95 && x <= 350 && y >= 70 && y <= 259)
	{
		if (event == EVENT_LBUTTONDOWN)
		{
			cout << "Left button of the mouse is pressed - position (" << x-des_initial.x << ", " << y-des_initial.y << ")" << endl;
			des1 = Point(x, y);
			des1_true = true;
		}
		else if (event == EVENT_LBUTTONUP)
		{
			cout << "Left button of the mouse is released - position (" << x- des_initial.x << ", " << y- des_initial.y << ")" << endl;
			des2 = Point(x, y);
			des2_true = true;
			if (des1_true == true && des2_true == true) {
				arrowedLine(DispImage, des1, des2, Scalar(255, 255, 255), 2);
				des_start[des_arrow] = des1-des_initial; des_end[des_arrow] = des2-des_initial;
				des_arrow++;
				des1_true = false; des2_true = false;
			}
				
		}
	}
	else if (x >= 445 && x <= 700 && y >= 70 && y <= 259)
	{
		if (event == EVENT_LBUTTONDOWN)
		{
			cout << "Left button of the mouse is pressed - position (" << x- source_initial.x << ", " << y- source_initial.y << ")" << endl;
			source1 = Point(x, y);
			source1_true = true;
		}
		else if (event == EVENT_LBUTTONUP)
		{
			cout << "Left button of the mouse is released - position (" << x- source_initial.x << ", " << y- source_initial.y << ")" << endl;
			source2 = Point(x, y);
			source2_true = true;
			if (source1_true == true && source2_true == true) {
				arrowedLine(DispImage, source1, source2, Scalar(255, 255, 255), 2);
				source_start[source_arrow] = source1-source_initial; source_end[source_arrow] = source2-source_initial;
				source_arrow++;
				source1_true = false; source2_true = false;
			}
			
		}
	}
	else if (x >= 345 && x < 445 && y >= 320 && y <= 370)
	{
		if (event == EVENT_LBUTTONDOWN) {
			button_true = true;
		}
		
	}
	else if (event == EVENT_RBUTTONDOWN)
	{
		cout << "destination:" << endl;
		for (int a = 0; a < des_arrow; a++) {
			cout << "(" << des_start[a].x << "," << des_start[a].y << ") to (" << des_end[a].x << "," << des_end[a].y << ")" << endl;
		}
		cout << "source:" << endl;
		for (int a = 0; a < source_arrow; a++) {
			cout << "(" << source_start[a].x << "," << source_start[a].y << ") to (" << source_end[a].x << "," << source_end[a].y << ")" << endl;
		}
	}
	if (button_true == true)
	{
		button_true = false;
		StartToMorph();
		
	}
	
	imshow("Morphing", DispImage);
}


int main(int argc, char** argv)
{
	// Read image ( same size, same type )
	img1 = imread("women.jpg");
	img2 = imread("cheetah.jpg");
	button_img = imread("button.png");
	resize(button_img, button_img, Size(100,50));

	//check whether the image correctly load
	if (!img1.data) { printf("Error loading img1 \n"); return -1; }
	if (!img2.data) { printf("Error loading img2 \n"); return -1; }

	
	//put text above the image
	putText(DispImage, "img1", Point(175, 50), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 143, 143), 2);
	putText(DispImage, "img2", Point(525, 50), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 143, 143), 2);


	//copy img1 and img2 to display image
	Rect ROI1(95, 70, img1.cols, img1.rows);
	Rect ROI2(445, 70, img2.cols, img2.rows);
	img1.copyTo(DispImage(ROI1));
	img2.copyTo(DispImage(ROI2));

	
	cout << "x: " << img1.cols << ", y: " << img1.rows << endl;

	//create button
	Rect button_ROI(345, 320, 100, 50);
	button_img.copyTo(DispImage(button_ROI));



	// Create Windows
	namedWindow("Morphing", 1);
	
	setMouseCallback("Morphing", CallBackFunc, NULL);


	imshow("Morphing", DispImage);

	// Wait until user press esc key
	waitKey(waitKey() != 27);
	destroyWindow("Morphing");
	return 0;
}