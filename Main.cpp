#include <iostream>
#include <thread>
#include <stdlib.h>
#include <mutex>
using namespace std;

mutex thr_mutex;

unsigned long SumOptim(unsigned long rgb1, unsigned long rgb2)
{
	unsigned long sum = rgb1 + rgb2;
	unsigned long mask = (rgb1 ^ rgb2) & 0x01010100;
	unsigned long over = mask ^ sum & 0x01010100;
	sum = sum - mask;
	sum = (sum & 0x00FEFEFF) + mask;
	return sum | ((over * 0xFF) >> 8);
}

unsigned long SumNaive(unsigned long rgb1, unsigned long rgb2)
{
	unsigned long sum = rgb1 + rgb2;
	if((rgb1 & 0x000000FF) + (rgb2 & 0x000000FF) > 0xFF)
		sum = (sum - 0x100) | 0xFF;
	if((rgb1 & 0x0000FF00) + (rgb2 & 0x0000FF00) > 0xFF00)
		sum = (sum - 0x10000) | 0xFF00;
	if((rgb1 & 0x00FF0000) + (rgb2 & 0x00FF0000) > 0xFF0000)
		sum = (sum - 0x1000000) | 0xFF0000;
	return sum;
}

void CycleOptim(unsigned long times)
{
	clock_t begin = clock();
	for (unsigned long i=0; i < times; i++)
	{
		SumOptim(i, i);
	}
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	thr_mutex.lock();
	cout << "Optim:\t" << elapsed_secs << endl;
	thr_mutex.unlock();
}

void CycleNaive(unsigned long times)
{
	clock_t begin = clock();
	for (unsigned long i=0; i < times; i++)
	{
		SumNaive(i, i);
	}
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	thr_mutex.lock();
	cout << "Naive:\t" << elapsed_secs << endl;
	thr_mutex.unlock();
}

void main()
{
	// Пул значений для теста корректности:
	//без переполнения; переполнение в 1-м, 2-м, 3-м байтах; 
	//в 1-м и 2-м; в 1-м и 3-м; во 2-м и 3-м; переполнение во всех байтах; 
	//переполнение в 1-м и сумма 255 во 2-м; переполнение в 1-м и сумма 255 во 2-м и 3-м; переполнение во 2-м и сумма 255 во 3-м
	long test[11][2] = {{0x00a7432f, 0x00275871}, {0x00a7432f, 0x002758e3}, {0x00a7432f, 0x0027c071}, {0x00a7432f, 0x00595871},
						{0x00a7432f, 0x0027c0e3}, {0x00a7432f, 0x005958e3}, {0x00a7432f, 0x0059c071}, {0x00a7432f, 0x0059c0e3},
						{0x00a7432f, 0x0027bce3}, {0x00a7432f, 0x0058bc71}, {0x00a7432f, 0x0058c071}};
	cout << "Correctness test on pull of elements ...";
	
	for (int i=0; i < 11; i++)
	{
		if(SumNaive(test[i][0], test[i][1]) ^ SumOptim(test[i][0], test[i][1]))
			cout << "Error!" << endl;
	}
	cout << "\tDone" << endl;
	// Тест производительности на N элементов
	const long N = 10000000;
	thread thr1(CycleNaive, N);
	thread thr2(CycleOptim, N);
	cout << "Summing " << N << " elements ..." << endl;
	thr1.join();
	thr2.join();
	char q;
	cout << "Vse!" << endl;
	cin >> q;
}
