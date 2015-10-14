#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <math.h>
#include <mpi.h>

#define EPSILON 0.05

using namespace std;

void copyArrayValues(int, int, double *, double**);
void copyValuestoStrip(int, int, double *, double**);
void calcOddPoints(int, int, double**);
void calcEvenPoints(int, int, double**);

int main(){
	int plateSize = 3;
	double **array;
	double *strip;
	int stripNumber = 0;

	array = (double**)malloc((plateSize + 2)* sizeof(double *));
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
		array[j][plateSize + 1] = 100.0;
		array[plateSize + 1][j] = 100.0;
	}

	//Print Initial state of the problem in console
	cout << "Initial Array" << endl;
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

	//Allocate Value for Strip
	strip = (double *)malloc((plateSize + 2) * sizeof(double));
	for (int i = 0; i < (plateSize + 2); i++)
	{
		strip[i] = 11;
	}

	cout << endl <<"Initial Strip" << endl;
	for (int i = 0; i < (plateSize + 2); i++)
	{
		cout << strip[i] << " ";
	}

	copyArrayValues(1,3,strip,array);

	//Print Initial state of the problem in console
	cout << endl <<"Final Array" << endl;
	for (int i = 0; i < (plateSize + 2); i++) {
		for (int j = 0; j < (plateSize + 2); j++){
			cout << array[i][j] << "   ";
		}
		cout << endl;
	}

	copyValuestoStrip(4, 3, strip, array);

	cout << endl << "Final Strip" << endl;
	for (int i = 0; i < (plateSize + 2); i++)
	{
		cout << strip[i] << " ";
	}
	getchar();

	return 1;
}

void copyArrayValues(int stripNumber, int plateSize, double *strip, double** array){
	for (int i = 0; i < (plateSize + 2); i++)
	{
		array[stripNumber][i] = strip[i];
	}
}

void copyValuestoStrip(int stripNumber, int plateSize, double *strip, double** array){
	cout << "Called";
	for (int i = 0; i < (plateSize + 2); i++)
	{
		strip[i] = array[stripNumber][i];
		cout << array[stripNumber][i] << ",";
	}
}

void calcOddPoints(int stripNumber, int plateSize, double** array){
	double vDash = 0.0;
	for (int j = 1; j < (plateSize + 1); j++)
	{
		vDash = 0.0;
		if ((stripNumber + j) % 2 != 0){
			vDash = (array[stripNumber - 1][j] + array[stripNumber][j - 1] + array[stripNumber + 1][j] + array[stripNumber][j + 1]) / 4.0;
			array[stripNumber][j] = (vDash + array[stripNumber][j]) / 2.0;
		}
	}
}

void calcEvenPoints(int stripNumber, int plateSize, double** array){
	double vDash = 0.0;
	for (int j = 1; j < (plateSize + 1); j++)
	{
		vDash = 0.0;
		if ((stripNumber + j) % 2 == 0){
			vDash = (array[stripNumber - 1][j] + array[stripNumber][j - 1] + array[stripNumber + 1][j] + array[stripNumber][j + 1]) / 4.0;
			array[stripNumber][j] = (vDash + array[stripNumber][j]) / 2.0;
		}
	}
}