/*
Code written and edited by Julia Adamczyk and Brendan Aguiar
Version: 0.1

Version History edits
0.1 Copied stuff over experiment1.cpp and adjusted for setB - Julia Adamczyk
0.2 changed seed in srand to 1 and added errorRates1 to derive_results function - Brendan Aguiar
0.3 - modified case3 and matrix determinant function to incorporate non-diagonal matrices - Julia Adamczyk
*/


//testing setB sample
#include <iostream>
#include <math.h>
#include <list>
#include <fstream>
#include <vector>
#include <set>
#include <time.h>
using namespace std;

//Gloabl assignments
const float CLASS_1_SIZE = 60000.0;
const float CLASS_2_SIZE = 140000.0;
const float TOTAL_SIZE = 200000.0;

//Function declarations
void printDistr(vector<float> setA[]);

void classify(vector <float> set[], float m1[], float s1[][2], float m2[], float s2[][2], float r[]);
float case1(vector<float>::iterator i1, vector<float>::iterator i2, float m[], float s, float prior);
float case3(vector<float>::iterator i1, vector<float>::iterator i2, float m[], float s[][2], float prior);
float euclidean(vector<float>::iterator i1, vector<float>::iterator i2, float m[]);
float determinant_of_diagonal(float mat[][2]);
float BhatBound(float m1[], float s1[][2], float m2[], float s2[][2], float prior1, float prior2);
void printMenu();
void classifyEuclidean(vector<float> set[], float m1[], float s1[][2], float m2[], float s2[][2], float r[]);
void printErrorReport(float r[], string dataset);
void read_from_file(ifstream& input_file, vector <float> set[]);

void calculate_mean(vector<float> dataset[], float* m1, float* m2);
void calculate_cov(vector<float> dataset[], float* m1, float* m2, float s1[][2], float s2[][2]);

void create_set(vector <float> dataset[], vector <float> new_dataset[], int new_size);
void derive_results(vector <float> dataset[], vector <float> new_dataset[], float r[]);
//void classifyPercentages(vector<float> class1[], vector<float> class2[], float r[][4]);
//void shuffle(vector<float> set, size_t begin, size_t end );


int main() {

	//Assignments for setA
	vector <float> setB[2];

	//set default values
	float cov1[2][2] = { {0, 0},
						{0, 0} };
	float cov2[2][2] = { {0, 0},
						{0, 0} };
	float mean1[2] = { 0, 0 };
	float mean2[2] = { 0, 0 };


	//Error rates
	float errorRates1[4] = { 0, 0, 0, 0 };


	//reading the data in from a file
	ifstream inFile;
	string filename1 = "SetB.txt";

	vector <float> dataset1[2];
	vector <float> dataset2[2];
	vector <float> dataset3[2];
	vector <float> dataset4[2];
	//Program Control
	bool again = true;
	int switch_on;
	srand(1);
	while (again)
	{
		printMenu();
		cin >> switch_on;
		switch (switch_on)
		{
		case 1:
			//Load Set B into the program
			inFile.open(filename1, ios::in);
			read_from_file(inFile, setB);
			inFile.close();
			break;
		case 2:
			//Print Set B to the terminal
			printDistr(setB);
			break;
		case 3:
			//calculate mean and covariance for set B
			calculate_mean(setB, mean1, mean2);
			cout << "\nExpected mean for class 1\nx:" << mean1[0] << " y: " << mean1[1] << endl;
			cout << "Expected mean for class 2\nx: " << mean2[0] << " y: " << mean2[1] << endl;
			calculate_cov(setB, mean1, mean2, cov1, cov2);
			cout << "Expected covariance for class 1: [" << cov1[0][0] << ", " << cov1[0][1] << ", " << cov1[1][0] << ", " << cov1[1][1] << "]\n";
			cout << "Expected covariance for class 2: [" << cov2[0][0] << ", " << cov2[0][1] << ", " << cov2[1][0] << ", " << cov2[1][1] << "]\n";
			break;
		case 4:
			//Full size Set B classification
			cout << "Classifying setB of original size..." << endl;
			classify(setB, mean1, cov1, mean2, cov2, errorRates1);
			break;
		case 5:
			//Set B classification using euclidean distances
			classifyEuclidean(setB, mean1, cov1, mean2, cov2, errorRates1);
			break;
		case 6:
			//Print Error Report Set B
			printErrorReport(errorRates1, "Set B");
			break;
		case 7:
			cout << "Experiment on 20 samples (0.01% original size): " << endl;
			create_set(setB, dataset1, 20);
			derive_results(setB, dataset1, errorRates1);
			break;
		case 8:
			cout << "Experiment on 200 samples (0.1% original size): " << endl;
			create_set(setB, dataset2, 200);
			derive_results(setB, dataset2, errorRates1);
			break;
		case 9:
			cout << "Experiment on 2,000 samples (1% original size): " << endl;
			create_set(setB, dataset3, 2000);
			derive_results(setB, dataset3, errorRates1);
			break;
		case 10:
			cout << "Experiment on 20,000 samples (10% original size): " << endl;
			create_set(setB, dataset4, 20000);
			derive_results(setB, dataset4, errorRates1);
			break;
		case 11:
			again = false;
			break;
		default:
			break;
		}
	}
	return 0;
}

/*
Description: Prints sample set to console. Could be modified to print to file.
*/
void printDistr(vector <float> set[])
{
	vector<float>::iterator it2 = set[1].begin(); //iterator for y values
	for (vector<float>::iterator it = set[0].begin(); it != set[0].end(); ++it)// initialize loop with iterator for x values
	{
		cout << *it << " " << *it2 << endl;
		++it2;
	}
}



/*
Description: Checks covariances of classes to decide discriminant case. Generates P(w1/x) and P(w2/x).
Uses logic from minimum error rate: Decides w1 if P(w1/x) > P(w2/x), else decides w2.
Increments misclassifcation observations for class 1.
*/
void classify(vector <float> set[], float m1[], float s1[][2], float m2[], float s2[][2], float r[])
{
	int miss1 = 0;//missclassification incrementor
	int miss2 = 0;//missclassification incrementor
	float prior1 = CLASS_1_SIZE / TOTAL_SIZE;//probability of class 1 60,000/200,000
	float prior2 = CLASS_2_SIZE / TOTAL_SIZE;//probability of class 2 140,000/200,000
	vector<float>::iterator it1 = set[0].begin();
	vector<float>::iterator it2 = set[1].begin();
	float size1 = set[0].size() * .3;
	float size2 = set[0].size() * .7;

	if (s1[0][0] == s2[0][0] && s1[1][1] == s2[1][1]) //Case I where covariances are equal
	{
		float s = s1[1][1];
		for (int i = 1; i <= size1; i++)//w1 samples should hold more weight in g1
		{
			float g1 = case1(it1, it2, m1, s, prior1);//g1(x)
			float g2 = case1(it1, it2, m2, s, prior2);//g2(x)
			if (g1 < g2) //if g1 is missclassied
				miss1++; //increments missclassification rate
			++it1;
			++it2;
		}
		for (int i = (size1 + 1); i <= size1 + size2; i++)// w2 samples should hold more weight in g2
		{
			float g1 = case1(it1, it2, m1, s, prior1);//g1(x) = P(w1/x)
			float g2 = case1(it1, it2, m2, s, prior2);
			if (g1 > g2) //if g2 is missclassified
				miss2++; //increments missclassification rate
			++it1;
			++it2;
		}
		cout << "Number of samples of class 1 that are missclassified: " << miss1 << endl;
		cout << "Number of samples of class 2 that are missclassified: " << miss2 << endl;
	}
	else //Case III where covariances are unequal
	{
		for (int i = 1; i <= size1; i++)//w1 samples should hold more weight in g1
		{
			float g1 = case3(it1, it2, m1, s1, prior1);//g1(x)
			float g2 = case3(it1, it2, m2, s2, prior2);//g2(x)
			if (g1 < g2) //if w1 is missclassied
				miss1++; //increments missclassification rate
			++it1;
			++it2;
		}
		for (int i = size1 + 1; i <= (size1 + size2); i++)// w2 samples should hold more weight in g2
		{
			float g1 = case3(it1, it2, m1, s1, prior1);//g1(x) = P(w1/x)
			float g2 = case3(it1, it2, m2, s2, prior2);
			if (g1 > g2) //if w2 is missclassied
				miss2++; //increments missclassification rate
			++it1;
			++it2;
		}
		cout << "Number of samples of class 1 that are missclassified: " << miss1 << endl;
		cout << "Number of samples of class 2 that are missclassified: " << miss2 << endl;
	}

	//Error Calculations

	cout << "Error rates for problem 1 and 2:" << endl;
	r[0] = miss1 / size1;
	r[1] = miss2 / size2;
	r[2] = (miss1 + miss2) / (size1 + size2);
	r[3] = BhatBound(m1, s1, m2, s2, prior1, prior2);// Bhattacharyya bound
	cout << r[0] << " " << r[1] << " " << r[2] << " " << r[3] << endl;
}

/*
Description: Returns the discriminant of case I where covariances of class 1 and class 2 are equal.
*/
float case1(vector<float>::iterator i1, vector<float>::iterator i2, float m[], float s, float prior)
{
	float e = euclidean(i1, i2, m); // ||x-m||^2
	return log(prior) + (e / (2 * s * s) * (-1));
}

float case3(vector<float>::iterator i1, vector<float>::iterator i2, float m[], float s[][2], float prior)
{
	float inverse[2][2];
	float determinant = determinant_of_diagonal(s);
	inverse[0][0] = s[1][1] / determinant;
	inverse[1][1] = s[0][0] / determinant;
	inverse[0][1] = -s[0][1] / determinant;
	inverse[1][0] = -s[1][0] / determinant;

	float addend1 = ( (*i1) * (-0.5f * inverse[0][0]) + (*i2) * (-0.5f * inverse[1][0])) * (*i1) + ( (*i1) * (-0.5f * inverse[0][1]) + (*i2) * (-0.5f * inverse[1][1])) * (*i2); //(x^t)*Wi*x
	float addend2 = (inverse[0][0] * m[0] + inverse[0][1] * m[1]) * (*i1) + (inverse[1][0] * m[0] + inverse[1][1] * m[1]) * (*i2); //(wi)^t*x
	float addend3 = ( m[0] * (-0.5f * inverse[0][0]) + m[1] * (-0.5f * inverse[1][0])) * m[0] + ( m[0] * (-0.5f * inverse[0][1]) + m[1] * (-0.5f * inverse[1][1])) * m[1] - 0.5f * log(determinant) + log(prior); //wi0
	return (addend1 + addend2 + addend3);
}

/*
Description: Returns the euclidean distance.
*/
float euclidean(vector<float>::iterator i1, vector<float>::iterator i2, float m[])
{
	return (((*i1) - m[0]) * ((*i1) - m[0]) + ((*i2) - m[1]) * ((*i2) - m[1])); //euclideanDistance() or  ||x-m||^2
}

/*
Description: Returns the determinant of the passed in 2x2 matrix (not necessarily diagonal).
*/
float determinant_of_diagonal(float mat[][2])
{
	float det = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
	return det;
}

/*
Description: Calculates and returns the Bhattacharyya bound.
*/
float BhatBound(float m1[], float s1[][2], float m2[], float s2[][2], float prior1, float prior2)
{
	float addend1 = pow(m2[0] - m1[0], 2) * (1 / (.5f * (s1[0][0] + s2[0][0]))) + (pow(m2[1] - m1[1], 2) * (1 / (.5f * (s1[1][1] + s2[1][1]))));
	addend1 = addend1 * .125f;
	float mat1[2][2] = { {.5f * (s1[0][0] + s2[0][0]), 0},
		{0, .5f * (s1[1][1] + s2[1][1])} };
	float det1 = determinant_of_diagonal(mat1);
	float det2 = determinant_of_diagonal(s1);
	float det3 = determinant_of_diagonal(s2);
	float addend2 = .5f * log(det1 / (pow(det2, .5f) * pow(det3, .5f)));
	float kBound = addend1 + addend2;
	return sqrt(prior1 * prior2) * exp(kBound * (-1));
}

/*
Description: Prints menu for switch cases.
*/
void printMenu() {
	cout << "Select from the following choices...\n1. Load SetB from file\n2. Print SetB to terminal\n";
	cout << "3. Calculate mean and covariance for setB\n4. Classify SetB (using calculated variables)\n5. Classify(Euclidean) SetB \n6. Print Error Report SetB \n";
	cout << "\n7. Experiment on 0.01% original set size \n8. Experiment on 0.1% original set size \n9. Experiment on 1% original set size \n";
	cout << "10. Experiment on 10% original set size \n\n11. Quit Program";
	cout << "\n\nNote: Load data before you compute mean and variance, classify, or print it. Classify data before you generate an error report.\n\n";
	cout << "Choice: ";
}
/*
Description: Classifies a dataset using only euclidean distance.
*/
void classifyEuclidean(vector <float> set[], float m1[], float s1[][2], float m2[], float s2[][2], float r[])
{
	int miss1 = 0;//missclassification incrementor
	int miss2 = 0;//missclassification incrementor
	float prior1 = CLASS_1_SIZE / TOTAL_SIZE;//probability of class 1 60,000/200,000
	float prior2 = CLASS_2_SIZE / TOTAL_SIZE;
	vector<float>::iterator it1 = set[0].begin();
	vector<float>::iterator it2 = set[1].begin();
	float errorRates[4] = { 0, 0, 0, 0 };
	float size1 = set[0].size() * .3;
	float size2 = set[0].size() * .7;

	for (int i = 1; i <= size1; i++)//w1 samples should hold more weight in g1
	{
		float g1 = euclidean(it1, it2, m1) * (-1);//g1(x) = P(w1/x)
		float g2 = euclidean(it1, it2, m2) * (-1);//g2(x) = P(w2/x)
		if (g1 < g2) //if w1 is missclassied
			miss1++; //increments missclassification rate
		++it1;
		++it2;
	}
	for (int i = (size1 + 1); i <= (size1 + size2); i++)// w2 samples should hold more weight in g2
	{
		float g1 = euclidean(it1, it2, m1) * (-1);//g1(x) = P(w1/x)
		float g2 = euclidean(it1, it2, m2) * (-1);//g2(x) = P(w2/x)
		if (g1 > g2) //if w2 is missclassied
			miss2++; //increments missclassification rate
		++it1;
		++it2;
	}
	cout << "Number of samples of class 1 that are missclassified: " << miss1 << endl;
	cout << "Number of samples of class 2 that are missclassified: " << miss2 << endl;

	//Error Calculations
	cout << "Error rates for problem 3 and 4:" << endl;
	r[0] = miss1 / size1;
	r[1] = miss2 / size2;
	r[2] = (miss1 + miss2) / (size1 + size2);
	r[3] = BhatBound(m1, s1, m2, s2, prior1, prior2);// Bhattacharyya bound
	cout << r[0] << " " << r[1] << " " << r[2] << " " << r[3] << endl;
}

/*
Description: Print out the error report for the passed in data set to a file.
*/
void printErrorReport(float r[], string dataset)
{
	//r[0] = class 1 missclassification, r[1] = class 2 missclassification,
					// r[2] = Total missclassification, r[3] = Bhattacharyya bound
	cout << "Enter file name: " << endl;
	string filename;
	cin >> filename;
	ofstream fout;
	fout.open(filename);
	cout << "Writing to file now..." << endl;
	fout << "Error Report for " << dataset << ":" << endl;
	fout << "Class 1 Missclassification Rate: " << r[0] << endl;
	fout << "Class 2 Missclassification Rate: " << r[1] << endl;
	fout << "Total Missclassification Rate: " << r[2] << endl;
	fout << "Bhattacharyya bound: " << r[3] << endl;
	cout << "File successfully written." << endl;
	fout.close();
}

/*
Description: Loads sets from the file into data structure.
*/
void read_from_file(ifstream& input_file, vector <float> dataset[]) {

	float x;
	float y;
	for (int index = 0; index < 200000; index++)
	{
		input_file >> x >> y;
		dataset[0].push_back(x);
		dataset[1].push_back(y);
	}
}

void calculate_mean(vector <float> dataset[], float* m1, float* m2) {

	float sum_x1 = 0.0;
	float sum_y1 = 0.0;
	float sum_x2 = 0.0;
	float sum_y2 = 0.0;

	int size1 = dataset[0].size() * .3;
	int size2 = dataset[0].size() * .7;

	vector<float>::iterator it1 = dataset[0].begin();
	vector<float>::iterator it2 = dataset[1].begin();
	for (int i = 1; i <= size1; i++)
	{
		sum_x1 += *it1;
		sum_y1 += *it2;
		++it1;
		++it2;
	}
	for (int i = size1 + 1; i <= size1 + size2; i++)
	{
		sum_x2 += *it1;
		sum_y2 += *it2;
		++it1;
		++it2;
	}
	m1[0] = sum_x1 / size1;
	m1[1] = sum_y1 / size1;

	m2[0] = sum_x2 / size2;
	m2[1] = sum_y2 / size2;
}

void calculate_cov(vector <float> dataset[], float* m1, float* m2, float s1[][2], float s2[][2])
{

	float size1 = dataset[0].size() * .3;
	float size2 = dataset[0].size() * .7;
	float invCLASS_1 = 1 / (size1 - 1);
	float invCLASS_2 = 1 / (size2 - 1);
	vector<float>::iterator it1 = dataset[0].begin();
	vector<float>::iterator it2 = dataset[1].begin();

	for (int i = 1; i <= size1; i++)
	{
		s1[0][0] += pow((*it1 - m1[0]), 2);
		s1[0][1] += (*it1 - m1[0]) * (*it2 - m1[1]);
		s1[1][0] += (*it1 - m1[0]) * (*it2 - m1[1]);
		s1[1][1] += pow((*it2 - m1[1]), 2);
		++it1;
		++it2;
	}
	s1[0][0] *= invCLASS_1;
	s1[1][1] *= invCLASS_1;
	s1[0][0] = sqrt(s1[0][0]);
	s1[1][1] = sqrt(s1[1][1]);
	//non diagonal values assigned to 0 comment out if you want to test the real values of covariance matrix
	//s1[0][1] = 0;
	//s1[1][0] = 0;

	//Comment out to test non diagonal of 0
	s1[0][1] *= invCLASS_1;
	s1[1][0] *= invCLASS_1;
	for (int i = size1 + 1; i <= (size1 + size2); i++)
	{
		s2[0][0] += pow((*it1 - m2[0]), 2);
		s2[0][1] += (*it1 - m2[0]) * (*it2 - m2[1]);
		s2[1][0] += (*it1 - m2[0]) * (*it2 - m2[1]);
		s2[1][1] += pow((*it2 - m2[1]), 2);
		++it1;
		++it2;
	}
	s2[0][0] *= invCLASS_2;
	s2[1][1] *= invCLASS_2;
	s2[0][0] = sqrt(s2[0][0]);
	s2[1][1] = sqrt(s2[1][1]);

	//non diagonal values assigned to 0 comment out if you want to test the real values of covariance matrix
	//s2[0][1] = 0;
	//s2[1][0] = 0;

	//Comment out to test non diagonal of 0
	s2[0][1] *= invCLASS_2;
	s2[1][0] *= invCLASS_2;
}

void create_set(vector <float> dataset[], vector <float> new_dataset[], int new_size) {
	//size of class 1
	int size1 = new_size * .3;
	//size of class 2
	int size2 = new_size * .7;
	//have a set of unique indices that you will push into a new_dataset that will be used for parameter estimation
	set<int> indices;
	//choose indices from 1-60,000
	while (indices.size() < size1)
		indices.insert((rand() % 60000 + 1));
	cout << "Testing size... Size of first class is: " << indices.size() << endl;
	//choose indices from 60,001-200,000
	while (indices.size() < new_size) {
		indices.insert(rand() % 140000 + 60001);
		//cout << "got to size: " << indices.size() << endl;
	}
	cout << "Testing size... Size of the whole set is: " << indices.size() << endl;

	cout << "Indices used in deriving set of size: " << new_size << endl;
	//push the values of the original data set of chosen indices to the new dataset
	for (set<int>::iterator itr = indices.begin(); itr != indices.end(); ++itr) {
		//printing used index just for debugging purposes
		cout << *itr << endl;
		new_dataset[0].push_back(dataset[0][*itr]);
		new_dataset[1].push_back(dataset[1][*itr]);
	}
}
/*
*** Produces a report on classifying dataset using new_dataset derived mean and covariance
*/
void derive_results(vector <float> dataset[], vector <float> new_dataset[], float errorRates1[]) {
	//only for printing results, they are not saved anywhere
	float cov1[2][2] = { {0, 0},
						{0, 0} };
	float cov2[2][2] = { {0, 0},
						{0, 0} };
	float mean1[2] = { 0, 0 };
	float mean2[2] = { 0, 0 };

	//calculate mean for the set using new_dataset
	calculate_mean(new_dataset, mean1, mean2);
	cout << "\nExpected mean for class 1\nx:" << mean1[0] << " y: " << mean1[1] << endl;
	cout << "Expected mean for class 2\nx: " << mean2[0] << " y: " << mean2[1] << endl;
	//calculate covariance for the set using new_dataset
	calculate_cov(new_dataset, mean1, mean2, cov1, cov2);
	cout << "Expected covariance for class 1: [" << cov1[0][0] << ", " << cov1[0][1] << ", " << cov1[1][0] << ", " << cov1[1][1] << "]\n";
	cout << "Expected covariance for class 2: [" << cov2[0][0] << ", " << cov2[0][1] << ", " << cov2[1][0] << ", " << cov2[1][1] << "]\n";
	//classify the whole setA using the results from parameter estimation
	classify(dataset, mean1, cov1, mean2, cov2, errorRates1);

}
