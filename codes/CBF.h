#ifndef _CBF_H
#define _CBF_H

#include "BOBHash32.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <math.h>
#include <algorithm>

typedef unsigned long long ull;

#define HASH_MAX (0xFFFFFFFF)

class CBF
{
	int n, m, k;
	int * counterarray;
	BOBHash32 * bobhash[40];


public:
	CBF();

	void insert(ull key, int f);
	void insert_all(ull *keys, int *fs, int item_num);

	int query(ull key, int f);
	double query_cr(ull *keys, int *fs, int item_num);
	

	void init(int mm, int kk);
	void clear();


	double query_cr_upper();
	double query_cr_lower();


	~CBF();
};
CBF::CBF()
{
	counterarray = NULL;
	for(int i = 0; i < 40; i++)
	{
		bobhash[i] = new BOBHash32(256 + 31 * i);
	}
}

void CBF::init(int mm, int kk)
{
	n = 0;
	m = mm;
	k = kk;
	
	if(counterarray != NULL)
		delete []counterarray;

	counterarray = new int[m];
	
	memset(counterarray, 0, sizeof(int) * m);
}

void CBF::clear()
{
	memset(counterarray, 0, sizeof(int) * m);
}
CBF::~CBF()
{
	delete[] counterarray;
	for(int i = 0; i < k; i++)
	{
		delete bobhash[i];
	}
}


void CBF::insert(ull key, int f)
{
	n++;
	int index = 0;
	for(int i = 0; i < k; i ++)
	{
		index = bobhash[i]->run((const char *)&key, 8) % m;

		counterarray[index] += f;
	}
}
void CBF::insert_all(ull *keys, int *fs, int item_num)
{
	for(int i = 0; i < item_num; i++)
	{
		insert(keys[i], fs[i]);
	}
}
int CBF::query(ull key, int f)
{
	int index = 0;
	int min_value = (1 << 30);
	for(int i = 0; i < k; i ++)
	{
		index = bobhash[i]->run((const char *)&key, 8) % m;
		min_value = min_value < counterarray[index] ? min_value : counterarray[index];
	}
	//correct case!
	if(min_value == f)
		return 1;
	return 0;
}
double CBF::query_cr(ull *keys, int *fs, int item_num)
{
	double cr_num = 0;
	for(int i = 0; i < item_num; i++)
	{
		if(query(keys[i], fs[i]) == 1)
		{
			cr_num ++;
		}
	}
	return cr_num / item_num;
}

double CBF::query_cr_upper()
{
	double temp = pow(1.0 - 1.0 / m, (n - 1) * 1.0 * k);
	return 1.0 - pow(1.0 - temp, k * 1.0);
}
double CBF::query_cr_lower()
{
	double temp = pow(1.0 - k * 1.0 / m, (n - 1) * 1.0);
	return 1.0 - pow(1.0 - temp, k * 1.0);
}

#endif//_CBF_H