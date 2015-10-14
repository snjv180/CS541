#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <math.h>
#include <sys/types.h>
#include <pthread.h>

#define EPSILON 0.05

using namespace std;

typedef struct {
	int threadId;
	double **array;
	int *stripNumber;
	int *numberOfIterations;
	int *plateSize;
	int *numThreads;
	int *turn;
} parameter;

pthread_mutex_t mutex;

void calcOddPoints(int, int, double**);
void calcEvenPoints(int, int, double**);
void printStep(int, int);
void printStripInfo(string, int, int, int, int, double**);

void *hello(void *arg)
{
	parameter *p = (parameter *)arg;
	printf("Thread %d ; Plate Size %d; Number of Theads %d; Number of Iterations %d; Address of Array %d; Address of Strip %d; Adress of StripNumber %d \n", p->threadId, *(p->plateSize), *(p->numThreads), *(p->numberOfIterations), p->array, p->stripNumber);
	return (NULL);
}

void *solve(void* arg){
	parameter *p = (parameter *)arg;
	int numberOfIterations = 0;
	while (numberOfIterations < *(p->numberOfIterations))
	{
		*(p->stripNumber) = 1;
		*(p->turn) = 0;
		while (true)
		{
			//Must be bound by Mutex
			//Critical Section
			pthread_mutex_lock(&mutex);
			if (*(p->turn) == p->threadId){
				
				if (*(p->turn) == p->threadId){
                    //calculate odd points
					calcOddPoints(*(p->stripNumber), *(p->plateSize), p->array);
                    //calculate even points
					calcEvenPoints(*(p->stripNumber), *(p->plateSize), p->array);
                    //Debug Info
					if (*(p->stripNumber) == 1 && numberOfIterations == 0){
						cout<<"Computation by"<<p->threadId << "Value is " << p->array[1][1]<<endl;
					}
					//cout << "Thread ID " << p->threadId << " has StripNumber " << *(p->stripNumber) << endl;
					*(p->stripNumber) += 1;
				}

				if (!(*(p->stripNumber) < (*(p->plateSize) + 1))){
					
					numberOfIterations++;
					pthread_mutex_unlock(&mutex);
					break;
				}
				
				*(p->turn) +=1;
				
				if (*(p->turn) == *(p->numThreads)){
					*(p->turn) = 0;
				}
			}
			pthread_mutex_unlock(&mutex);
		}
	}
	if (p->threadId==3){
		//Open file for debugging purposes.
		ofstream myfile("pdeSolverPThreadTest.csv", ios::out | ios::app);

		//Write output to file for each iteration State
		myfile << endl << "Iteration " << numberOfIterations << endl;

		for (int j = 0; j < (*(p->plateSize) + 2); j++){
			myfile << "Plate" << j << ",";
		}
		myfile << endl;
		for (int i = 0; i < (*(p->plateSize) + 2); i++) {
			for (int j = 0; j < (*(p->plateSize) + 2); j++){
				myfile << p->array[i][j] << ",";
			}
			myfile << endl;
		}
		//close the file
		myfile.flush();
		myfile.close();
	}
	return (NULL);
}

int main(int argc, char* argv[]){
	pthread_t *threads;
	pthread_attr_t pthread_custom_attr;
	int plateSize, numThreads, numberOfIterations, stripNumber = 0, turn = 0;
	double **array;
	double epsilonValue = 0.0;
	parameter *param;

	//Initialize the PlateSize to argv[1] and Number of Threads to argv[2]
	if (argc == 3){
		plateSize = stoi(argv[1], nullptr, 10);
		numThreads = stoi(argv[2], nullptr, 10);
	}
	else {
		plateSize = 64;
		numThreads = 4;
	}

	//Set things Up
	{
		//Allocate Memory for Thread
		threads = (pthread_t *)malloc(numThreads*sizeof(*threads));

		//Initialize Thread attribute
		pthread_attr_init(&pthread_custom_attr);

		//Allocate Memory for param
		param = (parameter *)malloc(sizeof(parameter)*numThreads);

		//Allocate Memory for parameter members
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

		//Calculate the value of episilon
		epsilonValue = -1.0 * log10(EPSILON);

		numberOfIterations = (int)ceil(((double)plateSize * epsilonValue) / (3.0));

		//Print Initial state of the problem in console
		/*for (int i = 0; i < (plateSize + 2); i++) {
			for (int j = 0; j < (plateSize + 2); j++){
			if (param->array[i][j] == 0.0){
			cout << param->array[i][j] << "   ";
			}
			else if (param->array[i][j] == 100.0){
			cout << param->array[i][j] << " ";
			}
			else if (param->array[i][j] == 50.0){
			cout << param->array[i][j] << "  ";
			}

			}
			cout << endl;
			}*/

		//Assign Number of PlateSize, Number of Iterations and Number of Threads
		for (int i = 0; i < numThreads; i++)
		{
			param[i].numberOfIterations = &numberOfIterations;
			param[i].numThreads = &numThreads;
			param[i].plateSize = &plateSize;
			param[i].stripNumber = &stripNumber;
			param[i].turn = &turn;
			param[i].array = array;
		}

	}
	//Setup Mutex
	pthread_mutex_init(&mutex, NULL);
	
	cout << "Computation Started" << endl;
	
	//Start Threads Up
	for (int i = 0; i < numThreads; i++)
	{
		param[i].threadId = i;
		pthread_create(&threads[i], &pthread_custom_attr, solve, (void *)(param + i));
	}

	cout << "Computation Finished" << endl;

	getchar();

	return 1;
}


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

void printStep(int rank, int stripNumber){
	cout << "Strip Number " << stripNumber << " Processed by Process " << rank << endl;
}

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