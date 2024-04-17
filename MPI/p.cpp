#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctime>
#include <random>
#include <cstring>
#include <algorithm>
#include <omp.h>


using namespace std;

double targetFunction(double x) {
	return x * x;
}

void processInitialization(int& n, double& l, double& r, double& h) {
	do {
		printf("Input integration segment\n");
		printf("l:");
		scanf_s("%lf", &l);
		printf("r:");
		scanf_s("%lf", &r);
		printf("Chosen segment: [%f, %f]\n", l, r);
		if (l > r) {
			printf("r must be greater than l!\n");
		}
	} while (l > r);

	do {
		printf("Input number of split segments:");
		scanf_s("%d", &n);
		printf("Chosen number of segments: %d\n", n);
		if (n <= 0) {
			printf("n must be greater than 0!\n");
		}
	} while (n <= 0);

	h = (r - l) / (double)n;

	return;
}

/*
void serialResultCalculation(double x0, int k, double h,
	double& result) {

	result = 0;
	double prevX = x0;
	for (int i = 1; i <= k; i++) {
		double nextX = prevX + h;
		result += (targetFunction(prevX) +
			4 * targetFunction(0.5 * (prevX + nextX)) +
			targetFunction(nextX)) * (nextX - prevX) / 6.0;
		prevX = nextX;
	}

	return;
}
*/

void serialResultCalculation(double x0, int k, double h, double& result)
{
	double prevX, nextX;
	double accumsum = 0;
#pragma omp parallel num_threads(8) reduction(+:accumsum) private(prevX, nextX)
	{
#pragma omp for
		for (int i = 1; i <= k; i++) {
			prevX = x0 + h * (i - 1);
			nextX = x0 + h * i;
			accumsum += (targetFunction(prevX) +
				4 * targetFunction(0.5 * (prevX + nextX)) +
				targetFunction(nextX)) * h / 6.0;
		}
	}
	result = accumsum;
	return;
}


int main(int argc, char** argv) {
	setvbuf(stdout, NULL, _IONBF, 0);

	int n;
	double l, r, h;

	double result;
	time_t start, finish;
	double duration;

	printf("Serial program\n");
	processInitialization(n, l, r, h);

	start = clock();
	serialResultCalculation(l, n, h, result);
	finish = clock();
	duration = (finish - start) / double(CLOCKS_PER_SEC);

	printf("Time of execution = %f\n", duration);
	printf("Result            = %f\n", result);



	return 0;
}
