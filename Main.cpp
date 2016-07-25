#include <iostream>
#include <thread>
#include <stdlib.h>
using namespace std;

union RGB
{
	unsigned long l;
	unsigned char bytes[4];
};

unsigned long randomRGB ()
{
	RGB rgb1;
	rgb1.l = 0;
	for (int i=0; i < 3; i++)
		rgb1.bytes[i] = rand() % 0x100;
	return rgb1.l;
}

unsigned long SumOptim(unsigned long rgb1, unsigned long rgb2)
{
	unsigned long sum = rgb1 + rgb2;
	unsigned long mask = (rgb1 ^ rgb2 ^ sum) & 0x01010100;
	mask = mask ^ (((sum - (mask & 0x00000100)) ^ sum) & 0x01010000);
	mask = mask ^ (((sum - (mask & 0x00010000)) ^ sum) & 0x01000000);
	sum = sum - mask;
	mask = (mask * 0xFF) >> 8;
	return sum | mask;
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
		SumOptim(i+5000, i+100000);
	}
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "Optim:\t" << elapsed_secs << endl;
}

void CycleNaive(unsigned long times)
{
	clock_t begin = clock();
	for (unsigned long i=0; i < times; i++)
	{
		SumNaive(i+5000, i+100000);
	}
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "Naive:\t" << elapsed_secs << endl;
}

void main()
{
	// Тест корректности
	cout << "Correctness test on 10k elements ...";
	
	for (unsigned long i=0; i < 10000; i++)
	{
		unsigned long rgb1 = randomRGB();
		unsigned long rgb2 = randomRGB();
		unsigned long rgb3 = SumNaive(rgb1, rgb2);
		unsigned long rgb4 = SumOptim(rgb1, rgb2);
		if(SumNaive(rgb1, rgb2) ^ SumOptim(rgb1, rgb2))
			cout << "Error!" << endl;
	}
	cout << "\tDone" << endl;
	// Тест производительности на 10,000,000 элементов
	thread thr1(CycleNaive,10000000);
	thread thr2(CycleOptim,10000000);
	cout << "Summing 10M elements ..." << endl;
	thr1.join();
	thr2.join();
	char q;
	cout << "Vse!" << endl;
	cin >> q;
}