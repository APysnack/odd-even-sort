/*
   -------------------------------

   Alan Pysnack
   J00643490
   CSC-399
   Homework 5
   Odd/Even Sort PARALLEL VERSION

*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <math.h>
#include <ctype.h>
#include <mpi.h>
#include <algorithm>
#include <string>
#include <ctime>

using namespace std;

vector<int> generateNums(int inputNum);
bool checkArgs(int argc, char* argv[]);
void generateArrays(int numVecs, char* argv[]);
void resizeArrays(int numVecs, char* argv[]);
void broadcastArrays(int numVecs);
int calculateWorkload(int rank, int size);
void localSort(int rank, int comm_sz, int workload);
void parallelSort(int rank, int comm_sz, int workload, int order);
void mergeHigh();
void mergeLow();
void sanity(int rank, int phase);
vector<vector<int>> vectorList;
vector<int> myLocalVector;
vector<int> remoteVector;
vector<int> outVector;
vector<int> sortTempVector;

int main(int argCount, char* argValue[])
{
	int comm_sz;
	int rank;
        
	double duration;
        clock_t start;
	start = clock();

	MPI_Init(&argCount, &argValue);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	// seeds random time
	srand(time(0));

	bool validArgs = checkArgs(argCount, argValue);


	if (validArgs) {

		int numVecs = argCount - 1;

		resizeArrays(numVecs, argValue);

		if (rank == 0) {
			generateArrays(numVecs, argValue);
		}

		int inputNum = strtol(argValue[1], NULL, 0);


		MPI_Barrier(MPI_COMM_WORLD);
		broadcastArrays(numVecs);

		if (rank == 0) {
			cout << "Here is your list before the sort" << endl;
			for (int i = 0; i < vectorList[0].size(); i++) {
				cout << vectorList[0].at(i) << endl;
			}
			cout << endl;
		}

		int workload = calculateWorkload(rank, comm_sz);
		parallelSort(rank, comm_sz, workload, inputNum);
		duration = (clock() - start) / (double)CLOCKS_PER_SEC;

		if (rank == 0) {
			cout << "The time the parallel program took: " << duration << " seconds" << endl;
		}
		MPI_Finalize();
	}

	return 0;
}

void generateArrays(int numVecs, char* argv[]) {
	for (int i = 0; i < numVecs; i++) {
		int inputNum = strtol(argv[i + 1], NULL, 0);
		vectorList[i] = generateNums(inputNum);
	}
}

int calculateWorkload(int rank, int size) {

	int workload = 0;

	if ((vectorList.at(0).size() % size) == 0) {
		workload = vectorList.at(0).size() / size;
	}
	else {
		int floor = (vectorList.at(0).size() / size);
		if (rank != (size - 1)) {
			workload = floor;
		}
		else {
			workload = vectorList.at(0).size() - (floor * (size - 1));
		}

	}

	return workload;

}

// verifies that the input is an integer
bool checkArgs(int argc, char* argv[])
{
	bool test = false;

	if (argc > 4 || argc == 1)
	{
		printf("Please enter 1, 2, or 3 positive integer arguments in the command line");
		return test;
	}

	for (int i = 1; i < argc; i++)
	{
		test = strtol(argv[i], NULL, 10);
		if (strtol(argv[i], NULL, 10) <= 0) {
			cout << "Please enter only positive numbers" << endl;
			test = false;
			return test;
		}
	}

	if (test == false) {
		cout << "There was an error with your input, please ensure you're entering between 1 to 3 positive integers at the command line" << endl;
	}

	return test;
}

void resizeArrays(int numVecs, char* argv[]) {
	for (int i = 0; i < numVecs; i++) {
		int arrSize = strtol(argv[i+1], NULL, 10);
		vector<int> tempVector;
		tempVector.resize(arrSize);
		vectorList.push_back(tempVector);
	}
}

void broadcastArrays(int numVecs) {
	for (int i = 0; i < numVecs; i++) {
		MPI_Bcast(vectorList.at(i).data(), vectorList.at(i).size(), MPI_INT, 0, MPI_COMM_WORLD);
	}
}

// generates random numbers
vector<int> generateNums(int inputNum)
{

	vector<int> randArray;
	for (int i = 0; i < inputNum; i++)
	{
		randArray.push_back(abs((rand() - rand()) * rand() % 1000));
	}

	return randArray;
}

int computePartner(int phase, int myrank, int comm_sz) {
	int partner = 0;

	if (phase % 2 == 0) {
		if (myrank % 2 != 0) {
			partner = myrank - 1;
		}

		else {
			partner = myrank + 1;
		}
	}

	else {

		if (myrank % 2 != 0) {
			partner = myrank + 1;
		}
		else {
			partner = myrank - 1;
		}

	}

	if (partner == -1 || partner == comm_sz) {
		partner = -1;
	}

	return partner;
}


void localSort(int rank, int comm_sz, int workload) {
	int start = 0;

	if (rank != (comm_sz - 1)) {
		start = workload * (rank);
	}

	else {
		start = vectorList.at(0).size() - workload;
	}

	for (int i = 0; i < myLocalVector.size(); i++) {
		myLocalVector[i] = vectorList.at(0).at(start + i);
	}

	sort(myLocalVector.begin(), myLocalVector.end());

}

void parallelSort(int rank, int comm_sz, int workload, int order) {
	int partner = 0;
	int lastpartner = 0;
	int offload;
	int mainload = (vectorList.at(0).size() / comm_sz);

	if (vectorList.at(0).size() % comm_sz == 0) {
		offload = mainload;
	}
	else {
		offload = vectorList.at(0).size() - ((vectorList.at(0).size() / comm_sz) * (comm_sz - 1));
	}

	remoteVector.resize(mainload);
	myLocalVector.resize(workload);

	localSort(rank, comm_sz, workload);

	for (int phase = 0; phase < comm_sz + 1; phase++) {
		lastpartner = partner;
		partner = computePartner(phase, rank, comm_sz);

		if (partner > -1) {
			if (partner == (comm_sz - 1)) {
				remoteVector.resize(offload);
			}
			if (lastpartner == (comm_sz - 1)) {
				remoteVector.resize(workload);
			}

			MPI_Sendrecv(myLocalVector.data(), myLocalVector.size(), MPI_INT, partner, 0, remoteVector.data(), offload, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			if (order % 2 == 1) {
				if (rank < partner) {
					mergeLow();
				}
				else {
					mergeHigh();
				}
			}
			else {
				if (rank < partner) {
					mergeLow();
				}
				else {
					mergeHigh();
				}
			}
		}
	}

	if (rank != 0) {
		MPI_Send(myLocalVector.data(), myLocalVector.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	else {
		if (order % 2 == 1) {
			cout << "Here are your sorted numbers:" << endl;
			outVector.resize(mainload);
			for (int i = 1; i < comm_sz - 1; i++) {
				MPI_Recv(outVector.data(), mainload, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				for (int j = 0; j < outVector.size(); j++) {
					cout << to_string(outVector.at(j)) << "\n";
				}
			}
			outVector.resize(offload);
			MPI_Recv(outVector.data(), offload, MPI_INT, (comm_sz - 1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for (int j = 0; j < outVector.size(); j++) {
				cout << to_string(outVector.at(j)) << "\n";
			}
		}
		else {
			cout << "Here are your sorted numbers:" << endl;
			outVector.resize(offload);
			MPI_Recv(outVector.data(), offload, MPI_INT, (comm_sz - 1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for (int j = outVector.size() - 1; j >= 0; j--) {
				cout << to_string(outVector.at(j)) << "\n";
			}
			outVector.resize(mainload);
			for (int i = (comm_sz -2); i >= 1; i--) {
				MPI_Recv(outVector.data(), mainload, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				for (int j = outVector.size() - 1; j >= 0; j--) {
					cout << to_string(outVector.at(j)) << "\n";
				}
			}
		}
	}
}

void sanity(int rank, int phase) {
	string stringOut1;
	string stringOut2;
	string stringOut3;
	string mainout;

	for (int i = 0; i < myLocalVector.size(); i++) {
		stringOut1 += to_string(myLocalVector.at(i)) + " ";
	}

	for (int i = 0; i < remoteVector.size(); i++) {
		stringOut2 += to_string(remoteVector.at(i)) + " ";
	}

	for (int i = 0; i < sortTempVector.size(); i++) {
		stringOut3 += to_string(sortTempVector.at(i)) + " ";
	}

	string pt1 = "Rank is ";
	string pt2 = ", local is ";
	string pt3 = ", remote is ";
	string pt4 = ", temp is ";
	string pt5 = ", phase is ";

	mainout = pt1 + to_string(rank) + pt2 + stringOut1 + pt3 + stringOut2 + pt4 + stringOut3 + pt5 + to_string(phase) + "\n";

	cout << mainout;


}

void mergeLow() {
	sortTempVector.resize(myLocalVector.size());
	int mi, ri, ti;
	mi = ri = ti = 0;


	while (ti < myLocalVector.size()) {
		if (myLocalVector.at(mi) <= remoteVector.at(ri)) {
			sortTempVector[ti] = myLocalVector.at(mi);
			ti++;
			mi++;
		}

		else {
			sortTempVector[ti] = remoteVector.at(ri);
			ti++;
			ri++;
		}
		if (ti > myLocalVector.size()) {
			sortTempVector[ti] = myLocalVector.at(mi);
			ti++;
			mi++;
		}
	}

	for (mi = 0; mi < myLocalVector.size(); mi++) {
		myLocalVector[mi] = sortTempVector.at(mi);
	}
}

void mergeHigh() {
	sortTempVector.resize(myLocalVector.size());

	int mi, ri, ti;
	mi = myLocalVector.size() - 1;
	ri = remoteVector.size() - 1;
	ti = 0;

	while (ti < myLocalVector.size()) {
		if (myLocalVector.at(mi) >= remoteVector.at(ri)) {
			sortTempVector[ti] = myLocalVector.at(mi);
			ti++;
			mi--;
		}

		else {
			sortTempVector[ti] = remoteVector.at(ri);
			ti++;
			ri--;

			// if the remote vector is smaller than the local vector and we've examined all its elements
			if (ri < 0) {
				while (ti < myLocalVector.size()) {
					sortTempVector[ti] = myLocalVector.at(mi);
					ti++;
					mi--;
				}
			}
		}
	}
	for (int i = 0; i < myLocalVector.size(); i++) {
		myLocalVector[i] = sortTempVector.at((sortTempVector.size() - 1) - i);
	}

}
