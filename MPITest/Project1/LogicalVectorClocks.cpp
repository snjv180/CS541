#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <time.h>
#include <mpi.h>

int max(int a, int b){
	if (a > b){
		return a;
	}
	else {
		return b;
	}
}

int random_number(int min_num, int max_num)
{
	int result = 0, low_num = 0, hi_num = 0;
	if (min_num<max_num)
	{
		low_num = min_num;
		hi_num = max_num + 1; // this is done to include max_num in output.
	}
	else{
		low_num = max_num + 1;// this is done to include max_num in output.
		hi_num = min_num;
	}
	srand(time(NULL));
	result = (rand() % (hi_num - low_num)) + low_num;
	return result;
}

int main(int argc, char** argv){
	int my_rank;
	int p;
	int source;
	int dest;
	int tag = 50;
	MPI_Status status;
	int eventTime = 0;

	//Initialize MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	//Create a Vector for storing time values
	int my_VT[10];
	int message[10];
	int counter = 0;
	int innerLoopCounter1 = 0;
	int innerLoopCounter2 = 0;

	for (counter = 0; counter <p; counter++) {
		my_VT[counter] = 0;
		message[counter] = 0;
	}

	for (counter = 0; counter < 100 / p; counter++){
		for (source = 0; source<p; source++){
			MPI_Recv(message, p, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
			for (innerLoopCounter1 = 0; innerLoopCounter1<p; innerLoopCounter1++){
				my_VT[innerLoopCounter1] = max(my_VT[innerLoopCounter1], message[innerLoopCounter1]);
			}
		}

		my_VT[my_rank]++;
		eventTime = my_VT[my_rank];
		dest = random_number(0, p - 1);
		MPI_Send(my_VT, p, MPI_INT, dest, tag, MPI_COMM_WORLD);
		//Sleep(1000 * (my_rank + 1));
	}
	MPI_Finalize();
}
