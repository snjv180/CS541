#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <math.h>

#define EPSILON 0.05

using namespace std;

int main(int argc, char* argv[]){
	int plateSize = 0;
	int numThreads = 0;
	int numberOfIterations = 0;
	double vDash = 0.0;
	double epsilonValue = 0.0;

	//Open file for debugging purposes.
	ofstream myfile("pdeSolver.csv");
	if (!myfile.is_open())
	{
		cout << "Unable to open file" << endl;
	}

	/*if (argc == 3){
		plateSize = stoi(argv[1],nullptr,10);
		numThreads = stoi(argv[2], nullptr, 10);
		cout << "Plate Size " << plateSize<<endl;
		cout << "Number of Threads " << numThreads << endl;
	}
	else {
		plateSize = 64;
		numThreads = 4;
	}*/
	
	plateSize = 64;
	numThreads = 4;

	//calculate the value of episilon
	epsilonValue =-1.0 * log10(EPSILON);

	numberOfIterations = (int)ceil(((double)plateSize * epsilonValue) / (3.0));

	double **array;
	array = (double**)malloc((plateSize+2)* sizeof(double *));
	for (int i = 0; i < (plateSize + 2); i++){
		array[i] = (double *)malloc((plateSize + 2) * sizeof(double));
	}
	
	//Set up all values to 50
	for (int i = 0; i < (plateSize + 2); i++) {
		for (int j = 0; j < (plateSize + 2); j++){
			array[i][j] = 50.0; 
		}
	}

	// Set the Top Most Portion 0
	for (int j = 0; j < (plateSize + 2); j++){
			array[0][j] = 0.0; 
	}
		
	//Set other edges to values 100
	for (int j = 1; j < (plateSize + 2); j++){
		array[j][0] = 100.0;
		array[j][plateSize+1] = 100.0;
		array[plateSize+1][j] = 100.0;
	}
	
	//Print Initial state of the problem in console
	for (int i = 0; i < (plateSize + 2); i++) {
		for (int j = 0; j < (plateSize + 2); j++){
			if (array[i][j] == 0.0){
				cout << array[i][j] << "   ";
			}
			else if (array[i][j] == 100.0){
				cout << array[i][j] << " ";
			}
			else if (array[i][j] == 50.0){
				cout << array[i][j] << "  ";
			}
			
		}
		cout << endl;
	}

	//Write output to file Initial State
	myfile << "Initial State" << endl;
	for (int j = 0; j < (plateSize + 2); j++){
		myfile << "Plate"<<j << ",";
	}
	myfile << endl;
	
	for (int i = 0; i < (plateSize + 2); i++) {
		for (int j = 0; j < (plateSize + 2); j++){
			myfile << array[i][j] << ",";
		}
		myfile << endl;
	}

	cout << endl << "Computation for Solving Partial Differential Equation Started. " << endl<<endl;
	cout << "Number of Iterations " << numberOfIterations << endl<<endl;
	//Loop for numberOfIterations times
	for (int k = 0; k < numberOfIterations; k++)
	{
		for (int i = 1; i < (plateSize + 1); i++)
		{
			//Calculate all "Odd" Points
			for (int j = 1; j < (plateSize + 1); j++)
			{
				vDash = 0.0;
				if ((i + j) % 2 == 0){
					vDash = (array[i - 1][j] + array[i][j - 1] + array[i + 1][j] + array[i][j + 1]) / 4.0;
					array[i][j] = (vDash + array[i][j]) / 2.0;
					//array[i][j] = 25.0;
				}
			}
			//Calculate all "Even" Points
			for (int k = 1; k < (plateSize + 1); k++)
			{
				vDash = 0.0;
				if ((i + k) % 2 != 0){
					vDash = (array[i - 1][k] + array[i][k - 1] + array[i + 1][k] + array[i][k + 1]) / 4.0;
					array[i][k] = (vDash + array[i][k]) / 2.0;
					//array[i][j] = 75.0;
				}
			}
		}

		//Write output to file for each iteration State
		myfile << endl << "Iteration "<< k << endl;

		for (int j = 0; j < (plateSize + 2); j++){
			myfile << "Plate" << j << ",";
		}
		myfile << endl;
		for (int i = 0; i < (plateSize + 2); i++) {
			for (int j = 0; j < (plateSize + 2); j++){
				myfile << array[i][j] << ",";
			}
			myfile << endl;
		}
	}
	
	//Print results
	for (int i = 0; i < (plateSize + 2); i++) {
		for (int j = 0; j < (plateSize + 2); j++){
			cout << array[i][j] << "  ";
		}
		cout << endl;
	}

	//Write output to file Final State
	myfile << endl << "Final State" << endl;
	
	for (int j = 0; j < (plateSize + 2); j++){
		myfile << "Plate" << j << ",";
	}
	myfile << endl;
	for (int i = 0; i < (plateSize + 2); i++) {
		for (int j = 0; j < (plateSize + 2); j++){
			myfile << array[i][j] << ",";
		}
		myfile << endl;
	}

	myfile << endl << "Number of Iterations = " << numberOfIterations << endl << endl;

	//close the file
	myfile.flush();
	myfile.close();

	cout << endl << "Computation for Solving Partial Differential Equation Ended. " << endl;

	getchar();
	return 1;
}
