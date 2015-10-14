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
void broadCastToAll(int, int, int, int, double *);
void printStep(int, int);
void printStripInfo(string, int, int, int, int, double**);

int main(int argc, char* argv[]){
	int plateSize = 0;
	int numThreads = 0;
	int numberOfIterations = 0;
	double epsilonValue = 0.0;
	int rank, numProcesses;
	MPI_Status status;
	double **array;
	double *strip;
	int stripNumber = 0;
	int receivedStripNumber = 0;

	/*if (argc == 3){
		plateSize = stoi(argv[1], nullptr, 10);
		numThreads = stoi(argv[2], nullptr, 10);
		cout << "Plate Size " << plateSize << endl;
		cout << "Number of Threads " << numThreads << endl;
		}
		else {
		plateSize = 64;
		numThreads = 4;
		}*/

	//Initialize the PlateSize to 64 and Number of Threads to 4
	plateSize = 64;
	numThreads = 4;

	//calculate the value of episilon
	epsilonValue = -1.0 * log10(EPSILON);

	//Calculate Number of Iterations necessary
	numberOfIterations = (int)ceil(((double)plateSize * epsilonValue) / (3.0));

	//Initialize MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

	//Allocate Memory for array having steady state temperature values
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

	if (rank == 0){
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
		cout << endl << "Computation for Solving Partial Differential Equation Started. " << endl << endl;
		cout << "Number of Iterations " << numberOfIterations << endl << endl;
	}

	//Allocate Value for Strip
	strip = (double *)malloc((plateSize + 2) * sizeof(double));

	//Loop for numberOfIterations times
	//Starts from k=0 to k=numberOfIterations-1
	for (int k = 0; k < numberOfIterations; k++)
	{

		//Initialize Strip Number to 0 again
		//Break array into Strip
		//Communicate the Strip to each Process
		//Each Process Calculates the value of Strip
		//Broadcast the Strip Number first
		//BroadCast the value of Each Strip to other processes
		//Update the value of the strip as per the stripNumber in other corresponding processes

		//Initialize stripNumber to 0 for each iteration
		stripNumber = rank + 1;

		while (true){

			//Calculation Logic Started
			/*
			If this if rank 0 process and the first Strip then
			no need to receive values from any other process just wait for the
			rank 0 to calculate the first strip value and
			the broadcast the value of the strip to all other
			processes.
			*/
			if (rank == 0 && stripNumber == 1){
				//Breaking up array into strip for setting 
				//up communication
				for (int j = 0; j < (plateSize + 2); j++)
				{
					//As we want to copy the strip 1 for process 0 and consecutively.
					strip[j] = array[stripNumber][j];
				}

				//copy value from strip to final array
				copyArrayValues(stripNumber, plateSize, strip, array);

				//Calculate odd points
				calcOddPoints(stripNumber, plateSize, array);

				//Calculate even points
				calcEvenPoints(stripNumber, plateSize, array);

				//Set information onto the buffer
				copyValuestoStrip(stripNumber, plateSize, strip, array);

				for (int i = 1; i < numProcesses; i++)
				{
					//cout << "Broadcasting Strip " << stripNumber << " From Process " << rank << " to " << i << endl;

					//Broadcast StripNumber to all the other processors
					MPI_Send(&stripNumber, 1, MPI_INTEGER, i, 50, MPI_COMM_WORLD);

					//Broadcast the Strip information to all the processors so they can work accordingly.
					MPI_Send(strip, (plateSize + 2), MPI_DOUBLE, i, 60, MPI_COMM_WORLD);
				}

				//Distribute the the strip number on the basis of process
				stripNumber += numProcesses;
			}
			else {
				//reInitialize receivedStripNumber to ZERO
				receivedStripNumber = 0;

				//Receive the stripNumber
				MPI_Recv(&receivedStripNumber, 1, MPI_INTEGER, MPI_ANY_SOURCE, 50, MPI_COMM_WORLD, &status);

				//Receive the Strip Values
				MPI_Recv(strip, (plateSize + 2), MPI_DOUBLE, MPI_ANY_SOURCE, 60, MPI_COMM_WORLD, &status);

				//copy value from strip to final array
				copyArrayValues(receivedStripNumber, plateSize, strip, array);

				if (receivedStripNumber == plateSize) {
					break;
				}

				if (((receivedStripNumber + 1) == stripNumber) && (stripNumber <= (plateSize))){

					//Calculate odd points
					calcOddPoints(stripNumber, plateSize, array);

					//Calculate even points
					calcEvenPoints(stripNumber, plateSize, array);

					//Set information onto the buffer
					copyValuestoStrip(stripNumber, plateSize, strip, array);

					//Broadcast Strip Info
					broadCastToAll(rank, numProcesses, plateSize, stripNumber, strip);

					if (stripNumber == plateSize) {
						break;
					}

					//Distribute the the strip number on the basis of process
					stripNumber += numProcesses;

				}
			}
		}
		cout << "Iteration Step : " << k << endl;

		if (rank == 0){

			//Open file for debugging purposes.
			ofstream myfile("pdeSolverParallel.csv", ios::out | ios::app);

			//Write output to file for each iteration State
			myfile << endl << "Iteration " << k << endl;

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
			//close the file
			myfile.flush();
			myfile.close();
		}
	}

	if (rank == 0){
		//Print results
		for (int i = 0; i < (plateSize + 2); i++) {
			for (int j = 0; j < (plateSize + 2); j++){
				cout << array[i][j] << "  ";
			}
			cout << endl;
		}

		cout << endl << "Computation for Solving Partial Differential Equation Ended. " << endl;
	}


	MPI_Finalize();
	return 1;
}

//Method to values from strip to array
void copyArrayValues(int stripNumber, int plateSize, double *strip, double** array){
	for (int i = 0; i < (plateSize + 2); i++)
	{
		array[stripNumber][i] = strip[i];
	}
}

//Method to copy values from array to strip
void copyValuestoStrip(int stripNumber, int plateSize, double *strip, double** array){
	for (int i = 0; i < (plateSize + 2); i++)
	{
		strip[i] = array[stripNumber][i];
	}
}

//Method to calculate odd points for array according to strip number
void calcOddPoints(int stripNumber, int plateSize, double** array){
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

//Method to calculate even points for array according to strip number
void calcEvenPoints(int stripNumber, int plateSize, double** array){
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

//Broadcast strip values to other processor nodes from source node
void broadCastToAll(int rank, int numProcesses, int plateSize, int stripNumber, double *strip){
	for (int i = 0; i < numProcesses; i++)
	{
		//No need to send same value for strip if same processor
		if (i == rank) continue;

		//cout << "Broadcasting Strip " << stripNumber << " From Process " << rank << " to " << i << endl;

		//Broadcast StripNumber to all the other processors
		MPI_Send(&stripNumber, 1, MPI_INTEGER, i, 50, MPI_COMM_WORLD);

		//Broadcast the Strip information to all the processors so they can work accordingly.
		MPI_Send(strip, (plateSize + 2), MPI_DOUBLE, i, 60, MPI_COMM_WORLD);
	}
}

//Print the strip information for a particular processor
//used for debugging
void printStep(int rank, int stripNumber){
	cout << "Strip Number " << stripNumber << " Processed by Process " << rank << endl;
}

//Print information at a given iteration step for a given strip
//this method is more specific
void printStripInfo(string str, int iteration, int iterationStep, int stripNumber, int plateSize, double** array){
	if (iteration == iterationStep){
		cout << str << endl;
		for (int i = 0; i < (plateSize + 2); i++)
		{
			cout << array[stripNumber][i] << " ";
		}
		cout << endl;
	}
}