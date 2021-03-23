/*
   -------------------------------

   Alan Pysnack
   J00643490
   CSC-399
   Homework 5
   Odd/Even Sort SERIAL VERSION

   -------------------------------
   Program compiles with "g++ Homework3.cpp -o Homework3"
   Program executes with "./Homework3.exe n" where n is an integer value > 0
   Program accepts inputs for up to 3 different values of n 
   -------------------------------
examples:

"./Homework3.exe 10" generates a list of 10 randomly sorted numbers
"./Homework3.exe 10 11 13" generates 3 different lists of randomly sorted numbers: of sizes 10, 11, and 13
"./Homework3.exe 20, -2, 13" generates a list of 20 sorted numbers, a prompt displaying that a negative input is not accepted, then list of 13 sorted numbers

-------------------------------
Eact shell scripts used (after compiling as Homework3.out):
-------------------------------
#!bin/sh/
./Homework3.out 20
-------------------------------
#!bin/sh/
./Homework3.out 21
-------------------------------
#!bin/sh/
./Homework3.out -5
-------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <ctype.h>
#include <algorithm>

using namespace std;

std::vector<int> sortNums(std::vector<int> randArray);
std::vector<int> generateNums(int inputNum);
bool checkInt(int argc, char *argv[]);
void program_driver(int argc, char *argv[]);

main(int argc, char *argv[])
{

	double duration;
	clock_t start;
	start = clock();

	// seeds random time
	srand(time(0));

	// main function to run the program, receives command line args
	program_driver(argc, argv);

	duration = (clock() - start) / (double)CLOCKS_PER_SEC;
	cout << "The time the serial program took: " << duration << " seconds" << endl;

	return 0;
}

void program_driver(int argc, char *argv[])
{
	// ensures user enters between 1 to 3 arguments in the command line
	if (argc > 4 || argc == 1)
	{
		printf("Please enter 1, 2, or 3 positive integer arguments in the command line");
	}
	else
	{
		// ensures that the user entered an integer
		bool validArgs = checkInt(argc, argv);

		// if user input was integer:
		if (validArgs)
		{
			int sum = 0;

			// cycles through all the integers the user entered at the command line
			for (int i = 1; i < argc; i++)
			{
				// converts the users input to a long
				int inputNum = strtol(argv[i], NULL, 0);

				// ensures the users input was a positive number
				if (inputNum <= 0)
				{
					printf("We're sorry, this program does not process integers less than or equal to 0\n");
				}

				// if all conditions are met, generates a vector with argv random numbers
				else
				{

					// stores all the random numbers in randArray
					vector<int> randArray = generateNums(inputNum);
					printf("Here is your unsorted list of %d random numbers:\n", inputNum);

					// prints out all the unsorted numbers
					for (int i = 0; i < randArray.size(); i++)
					{
						cout << randArray.at(i) << endl;
					}

					// calls the sorting algorithm on the array
					vector<int> test = sortNums(randArray);

					for (int i = 0; i < test.size(); i++)
					{
						cout << test.at(i) << endl;
					}

					cout << endl;
				}
			}
		}
		else
		{
			cout << "Please ensure your arguments are all integers" << endl;
		}
	}
}

// verifies that the input is an integer
bool checkInt(int argc, char *argv[])
{
	bool test;

	for (int i = 1; i < argc; i++)
	{
		test = strtol(argv[i], NULL, 10);
	}

	return test;
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

// algorithm to sort an incoming array
vector<int> sortNums(vector<int> randArray)
{
	int n = randArray.size();
	int temp;

	if (n % 2 == 1)
	{
		// for loop that iterates until phases exceed the array size
		for (int phase = 0; phase < n; phase++)
		{
			// if the phase is even ("middle-to-left" comparison)
			if (phase % 2 == 0)
			{

				// for even phases does a "middle-to-left" comparison. Swaps if left side > right side
				for (int i = 1; i < n; i += 2)
				{

					if (randArray.at(i - 1) > randArray.at(i))
					{
						temp = randArray.at(i);
						randArray[i] = randArray.at(i - 1);
						randArray[i - 1] = temp;
					}
				}
			}

			// if the phase is odd ("middle-to-right" comparison)
			else
			{

				for (int i = 1; i < (n - 1); i += 2)
				{
					// compares "middle" to element on the right and swaps if middle > right
					if (randArray.at(i) > randArray.at(i + 1))
					{
						temp = randArray.at(i);
						randArray[i] = randArray.at(i + 1);
						randArray[i + 1] = temp;
					}
				}
			}
		}
	}
	else
	{
		// for loop that iterates until phases equal the array size
		for (int phase = 0; phase < n; phase++)
		{
			// if the phase is even ("middle-to-left" comparison)
			if (phase % 2 == 0)
			{

				// for even phases does a "middle-to-left" comparison. Swaps if left side > right side
				for (int i = 1; i < n; i += 2)
				{

					if (randArray.at(i - 1) < randArray.at(i))
					{
						temp = randArray.at(i);
						randArray[i] = randArray.at(i - 1);
						randArray[i - 1] = temp;
					}
				}
			}

			// if the phase is odd ("middle-to-right" comparison)
			else
			{

				for (int i = 1; i < (n - 1); i += 2)
				{
					// compares "middle" to element on the right and swaps if middle > right
					if (randArray.at(i) < randArray.at(i + 1))
					{
						temp = randArray.at(i);
						randArray[i] = randArray.at(i + 1);
						randArray[i + 1] = temp;
					}
				}
			}
		}
	}

	return randArray;
}
