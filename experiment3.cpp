/*
Code written and edited by Julia Adamczyk and Brendan Aguiar
Version: 0.1

Version History edits
0.1 Put code together with image class. Successfully read header of ppm file. readImage function runs into exception error - Brendan Aguiar
0.2 Successfully read data. Began training function - Julia Adamczyk and Brendan Aguiar (team session)
0.3 Built training model, started classifying testing data. - Brendan Aguiar
0.4 Modified model, classify, testing and main. Created load_image -  Julia Adamczyk
0.5
*/

#include "image.h"//for ImageType and RGB datatypes
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <math.h>

void printMenu();
void readImageHeader(char fname[], int& N, int& M, int& Q, bool& type, bool& again);
void readImage(char fname[], ImageType& image, bool& again);
void training(bool&, float mean[], float cov[][2]);
void testing(bool& again, float mean[], float cov[][2], ImageType& test_image, ImageType& Reference, float threshold);
void calculate_mean(vector<float> dataset, float& m1);
void calculate_cov(vector<float> dataset, vector<float> dataset2, float m1[], float s1[][2]);
float model(float r, float g, float m[], float s[][2]);
void load_image(char filename[], ImageType& image, bool& again);
float determinant_of_diagonal(float mat[][2]);
void writeImage(char fname[], ImageType& image);
void getErrorRates(ImageType reference_image, int rates[], bool again, char fname[]);
void printErrorReport(int r[], float t, char dataset[]);
float M_PI = (float)3.14;
float c = .004;//Max

float optimum_t_3 = 140;
float optimum_t_6 = 130;

int main()
{
	//Training Paramaters
	float mean[2];
	float cov[2][2] = { { 0, 0 },{0, 0} };
	//Program Control
	bool again = true;
	int switch_on;
	//Image type paramters
	char test1[] = "Training_3.ppm";
	char test2[] = "Training_6.ppm";
	char train1[] = "new_reference.ppm";
	char train2[] = "new_reference2.ppm";
	char ref3[] = "ref3.ppm";
	char ref6[] = "ref6.ppm";
	char report1[] = "report1.txt";
	char report2[] = "report2.txt";
	ImageType test_image;
	ImageType test_image2;
	ImageType reference_image3;
	ImageType reference_image6;
	float t = .004;//Threshold
	int rates3[2];//[0] = FN, [1] = FP
	int rates6[2];//[0] = FN, [1] = FP
	while (again)
	{
		printMenu();
		cin >> switch_on;
		switch (switch_on)
		{
		case 1:
			training(again, mean, cov);
			break;
		case 2:
			cout << "Testing file: " << test1 << "..." << endl;
			load_image(test1, test_image, again);
			testing(again, mean, cov, test_image, reference_image3, t);
			writeImage(train1, reference_image3);
			getErrorRates(reference_image3, rates3, again, ref3);

			cout << "Testing file: " << test2 << "..." << endl;
			load_image(test2, test_image2, again);
			testing(again, mean, cov, test_image2, reference_image6, t);
			writeImage(train2, reference_image6);
			getErrorRates(reference_image6, rates6, again, ref6);
			printErrorReport(rates3, t, report1);
			printErrorReport(rates6, t, report2);
			break;
		case 3:
			again = false;
		default:
			break;
		}
	}
	return 0;
}

void printMenu() {
	cout << "Choose from the following options:\n1)Load training/testing data.\n2)Test data\n3)Quit Program" << endl;
}
void readImageHeader(char fname[], int& N, int& M, int& Q, bool& type, bool& again)
{
	char header[100], * ptr;
	ifstream ifp;

	ifp.open(fname, ios::in | ios::binary);

	if (!ifp) {
		cout << "Can't read image: " << fname << endl;
		again = false;
	}

	// read header

	type = false; // PGM

	ifp.getline(header, 100, '\n');
	if ((header[0] == 80) &&  /* 'P' */
		(header[1] == 53)) {  /* '5' */
		type = false;
	}
	else if ((header[0] == 80) &&  /* 'P' */
		(header[1] == 54)) {        /* '6' */
		type = true;
	}
	else {
		cout << "Image " << fname << " is not PGM or PPM" << endl;
		again = false;
	}

	ifp.getline(header, 100, '\n');
	while (header[0] == '#')
		ifp.getline(header, 100, '\n');

	M = strtol(header, &ptr, 0);
	N = atoi(ptr);

	ifp.getline(header, 100, '\n');

	Q = strtol(header, &ptr, 0);

	ifp.close();
}
void readImage(char fname[], ImageType& image, bool& again)
{
	int i, j;
	int N, M, Q;
	unsigned char* charImage;
	char header[100], * ptr;
	ifstream ifp;

	ifp.open(fname, ios::in | ios::binary);

	if (!ifp) {
		cout << "Can't read image: " << fname << endl;
		again = false;
	}

	// read header

	ifp.getline(header, 100, '\n');

	if ((header[0] != 80) ||    /* 'P' */
		(header[1] != 54)) {   /* '6' */
		cout << "Image " << fname << " is not PPM" << endl;
		again = false;
	}

	ifp.getline(header, 100, '\n');
	while (header[0] == '#')
		ifp.getline(header, 100, '\n');

	M = strtol(header, &ptr, 0);
	N = atoi(ptr);
	cout << "M equals: " << M << "N equals: " << N << endl;

	ifp.getline(header, 100, '\n');
	Q = strtol(header, &ptr, 0);
	cout << "Q equals: " << Q << endl;

	charImage = (unsigned char*) new unsigned char[3 * M * N];

	ifp.read(reinterpret_cast<char*>(charImage), (3 * M * N) * sizeof(unsigned char));

	if (ifp.fail()) {
		cout << "Image " << fname << " has wrong size" << endl;
		again = false;
	}


	ifp.close();
	cout << "successfuly read the file" << endl;
	/* Convert the unsigned characters to integers */

	RGB val;

	for (i = 0; i < N; i++)
		for (j = 0; j < 3 * M; j += 3) {
			val.r = (int)charImage[i * 3 * M + j];
			val.g = (int)charImage[i * 3 * M + j + 1];
			val.b = (int)charImage[i * 3 * M + j + 2];
			image.setPixelVal(i, j / 3, val);
		}

	delete[] charImage;
}

//functions (training and testing) to test the YCbCr color space. Comment these two out or delete the comment if you want to test different variation.
/*
void training(bool& again, float mean[], float cov[][2]) {
	char ref1[] = "ref1.ppm";
	char Training_1[] = "Training_1.ppm";
	int N1, M1, Q1, N1_color, M1_color, Q1_color;
	bool typeR1, typeR1_color;
	ImageType T1;
	ImageType T1_color;

	vector<float> skin_color[2];//[1] = r, [2] = g
	readImageHeader(ref1, N1, M1, Q1, typeR1, again); //read image header

	T1.setImageInfo(N1, M1, Q1);
	readImage(ref1, T1, again);

	readImageHeader(Training_1, N1_color, M1_color, Q1_color, typeR1_color, again); //read image header
	T1_color.setImageInfo(N1_color, M1_color, Q1_color);
	readImage(Training_1, T1_color, again);

	RGB pixel;
	RGB pixel_color;
	float R, G, B;
	float r, g;

	for (int i = 0; i < N1; i++) {
		for (int j = 0; j < M1; j++) {
			T1.getPixelVal(i, j, pixel);
			T1_color.getPixelVal(i, j, pixel_color);
			if ((pixel.r != 0 && pixel.g != 0 && pixel.b != 0)) {
				//Normalize variables
				R = (float)pixel_color.r;
				G = (float)pixel_color.g;
				B = (float)pixel_color.b;
				r = R / (R + G + B);
				g = G / (R + G + B);
				skin_color[0].push_back(r);
				skin_color[1].push_back(g);
			}
		}
	}
	float r_mean = 0;
	float g_mean = 0;
	calculate_mean(skin_color[0], r_mean);
	calculate_mean(skin_color[1], g_mean);
	mean[0] = r_mean;
	mean[1] = g_mean;
	calculate_cov(skin_color[0], skin_color[1], mean, cov);
	cout << "\nmean =  [" << mean[0] << ", " << mean[1] << "]\n";
	cout << "\ncovariance =  [" << cov[0][0] << ", " << cov[0][1] << ", " << cov[1][0] << ", " << cov[1][1] << "]\n";
}

//this function creates a black and white reference image from the color testing image
//then this image should be used to compute FP: non-face pixel which was classified as skin-color the real was black but in our image it was white)
//and FN: face pixel which was classified as non-skin color (it was white in provided reference but our ref_image showed it as black)
void testing(bool& again, float mean[], float cov[][2], ImageType& test_image, ImageType& Reference, float threshold) {
	int N, M, Q;
	test_image.getImageInfo(N, M, Q);
	cout << N << " " << M << endl;
	//allocate reference image
	Reference.setImageInfo(N, M, Q);

	RGB val;
	RGB pixel;
	int R, G, B;
	float r, g;

	//vector<float> test[2];


	float g1;

	//loop through the testing image
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			//normalize the testing image pixel values
			test_image.getPixelVal(i, j, pixel);
			R = pixel.r;
			G = pixel.g;
			B = pixel.b;
			r = (float)R / (R + G + B);
			g = (float)G / (R + G + B);
			//calculate discriminant
			g1 = model(r, g, mean, cov);
			//compare to threshold
			if (g1 > threshold) //in skin color distribution
			{
				//miss++;
				//set pixel to white
				val.b = 255;
				val.g = 255;
				val.r = 255;
				Reference.setPixelVal(i, j, val);
			}
			else {
				//set pixel to black
				val.b = 0;
				val.g = 0;
				val.r = 0;
				Reference.setPixelVal(i, j, val);
			}
		}
	}
}

*/

void load_image(char filename[], ImageType& image, bool& again) {
	int N, M, Q;
	bool typeR;
	readImageHeader(filename, N, M, Q, typeR, again); //read image header
	image.setImageInfo(N, M, Q);
	readImage(filename, image, again);
}


float model(float r, float g, float m[], float s[][2])
{
	float det = determinant_of_diagonal(s);
	float val = 1 / (sqrt(det) * 2 * M_PI);
	float inverse[2][2];
	inverse[0][0] = s[1][1] / det;
	inverse[1][1] = s[0][0] / det;
	inverse[0][1] = -s[0][1] / det;
	inverse[1][0] = -s[1][0] / det;
	float exponent = (float)-.5 * (((r - m[0]) * (inverse[0][0]) + (g - m[1]) * (inverse[1][0])) * (r - m[0]) + ((r - m[0]) * (inverse[0][1]) + (g - m[1]) * (inverse[1][1])) * (g - m[1]));
	val *= exp(exponent);
	return val;
}
float determinant_of_diagonal(float mat[][2])
{
	float det = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
	return det;
}
void calculate_mean(vector<float> dataset, float& m1) {

	float value = 0;

	vector<float>::iterator it1 = dataset.begin();

	for (int i = 0; i < (int)dataset.size(); i++)
	{
		value += *it1;
		it1++;
	}

	m1 = (float)(value / dataset.size());

}
void calculate_cov(vector<float> dataset, vector<float> dataset2, float m1[], float s1[][2]) {

	vector<float>::iterator it1 = dataset.begin();
	vector<float>::iterator it2 = dataset2.begin();
	for (int i = 0; i < (int)dataset.size(); i++)
	{
		s1[0][0] += pow((*it1 - m1[0]), 2);
		s1[0][1] += (*it1 - m1[0]) * (*it2 - m1[1]);
		s1[1][0] += (*it1 - m1[0]) * (*it2 - m1[1]);
		s1[1][1] += pow((*it2 - m1[1]), 2);
		++it1;
		++it2;
	}
	s1[0][0] /= dataset.size();
	s1[1][1] /= dataset.size();
	s1[0][1] /= dataset.size();
	s1[1][0] /= dataset.size();
}
void writeImage(char fname[], ImageType& image)
{
	int i, j;
	int N, M, Q;
	unsigned char* charImage;
	ofstream ofp;

	image.getImageInfo(N, M, Q);

	charImage = (unsigned char*) new unsigned char[3 * M * N];

	RGB val;

	for (i = 0; i < N; i++)
		for (j = 0; j < 3 * M; j += 3) {
			image.getPixelVal(i, j / 3, val);
			charImage[i * 3 * M + j] = (unsigned char)val.r;
			charImage[i * 3 * M + j + 1] = (unsigned char)val.g;
			charImage[i * 3 * M + j + 2] = (unsigned char)val.b;
		}

	ofp.open(fname, ios::out | ios::binary);

	if (!ofp) {
		cout << "Can't open file: " << fname << endl;
		exit(1);
	}

	ofp << "P6" << endl;
	ofp << M << " " << N << endl;
	ofp << Q << endl;

	ofp.write(reinterpret_cast<char*>(charImage), (3 * M * N) * sizeof(unsigned char));

	if (ofp.fail()) {
		cout << "Can't write image " << fname << endl;
		exit(0);
	}

	ofp.close();

	delete[] charImage;
}
void getErrorRates(ImageType reference_image, int rates[], bool again, char ref[]) {
	int N1, M1, Q1;
	ImageType T1;
	bool typeR1;
	readImageHeader(ref, N1, M1, Q1, typeR1, again);
	T1.setImageInfo(N1, M1, Q1);
	readImage(ref, T1, again);
	RGB refPixel;//ref3.ppm pixel values
	RGB pixel;//our image
	int miss1 = 0;//False positives
	int miss2 = 0;// false negatives

	for (int i = 0; i < N1; i++) {
		for (int j = 0; j < M1; j++) {
			T1.getPixelVal(i, j, refPixel);
			reference_image.getPixelVal(i, j, pixel);
			if (pixel.r == 255 && refPixel.r == 0)
			{
				miss1++;
			}
			else if (pixel.r == 0 && refPixel.r == 255)
			{
				miss2++;
			}
		}
	}
	rates[0] = miss1;//False positive
	rates[1] = miss2;//False negative
}
void printErrorReport(int r[], float t, char dataset[])
{
	//r[0] = class 1 missclassification, r[1] = class 2 missclassification,
					// r[2] = Total missclassification, r[3] = Bhattacharyya bound
	cout << "Enter file name: " << endl;
	ofstream fout;
	fout.open(dataset);
	cout << "Writing to file now..." << endl;
	fout << r[0] << " " << t << endl;
	fout << r[1] << " " << t << endl;
	cout << "File successfully written." << endl;
	fout.close();
}


//functions (training and testing) to test the YCbCr color space. Comment out or delete the comment if you want to test different variation.

void training(bool& again, float mean[], float cov[][2]) {
	char ref1[] = "ref1.ppm";
	char Training_1[] = "Training_1.ppm";
	int N1, M1, Q1, N1_color, M1_color, Q1_color;
	bool typeR1, typeR1_color;
	ImageType T1;
	ImageType T1_color;

	vector<float> skin_color[2];//[1] = r, [2] = g
	readImageHeader(ref1, N1, M1, Q1, typeR1, again); //read image header

	T1.setImageInfo(N1, M1, Q1);
	readImage(ref1, T1, again);

	readImageHeader(Training_1, N1_color, M1_color, Q1_color, typeR1_color, again); //read image header
	T1_color.setImageInfo(N1_color, M1_color, Q1_color);
	readImage(Training_1, T1_color, again);

	RGB pixel;
	RGB pixel_color;
	float R, G, B;
	float r, b;

	for (int i = 0; i < N1; i++) {
		for (int j = 0; j < M1; j++) {
			T1.getPixelVal(i, j, pixel);
			T1_color.getPixelVal(i, j, pixel_color);
			if ((pixel.r != 0 && pixel.g != 0 && pixel.b != 0)) {
				//calculate values for this color space
				R = (float)pixel_color.r;
				G = (float)pixel_color.g;
				B = (float)pixel_color.b;
				r = 0.5 * R - 0.419 * G - 0.081 * B;
				b = -0.169 * R - 0.332 * G + 0.5 * B;
				skin_color[0].push_back(r);
				skin_color[1].push_back(b);
			}
		}
	}
	float r_mean = 0;
	float b_mean = 0;
	calculate_mean(skin_color[0], r_mean);
	calculate_mean(skin_color[1], b_mean);
	mean[0] = r_mean;
	mean[1] = b_mean;
	calculate_cov(skin_color[0], skin_color[1], mean, cov);
	cout << "\nmean =  [" << mean[0] << ", " << mean[1] << "]\n";
	cout << "\ncovariance =  [" << cov[0][0] << ", " << cov[0][1] << ", " << cov[1][0] << ", " << cov[1][1] << "]\n";
}

//this function creates a black and white reference image from the color testing image
//then this image should be used to compute FP: non-face pixel which was classified as skin-color the real was black but in our image it was white)
//and FN: face pixel which was classified as non-skin color (it was white in provided reference but our ref_image showed it as black)
void testing(bool& again, float mean[], float cov[][2], ImageType& test_image, ImageType& Reference, float threshold) {
	int N, M, Q;
	test_image.getImageInfo(N, M, Q);
	cout << N << " " << M << endl;
	//allocate reference image
	Reference.setImageInfo(N, M, Q);

	RGB val;
	RGB pixel;
	int R, G, B;
	float r, b;

	//vector<float> test[2];


	float g1;

	//loop through the testing image
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			//normalize the testing image pixel values
			test_image.getPixelVal(i, j, pixel);
			R = pixel.r;
			G = pixel.g;
			B = pixel.b;
			r = 0.5 * R - 0.419 * G - 0.081 * B;
			b = -0.169 * R - 0.332 * G + 0.5 * B;
			//calculate discriminant
			g1 = model(r, b, mean, cov);
			//compare to threshold
			if (g1 > threshold) //in skin color distribution
			{
				//miss++;
				//set pixel to white
				val.b = 255;
				val.g = 255;
				val.r = 255;
				Reference.setPixelVal(i, j, val);
			}
			else {
				//set pixel to black
				val.b = 0;
				val.g = 0;
				val.r = 0;
				Reference.setPixelVal(i, j, val);
			}
		}
	}
}
