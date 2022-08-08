#ifndef _BF_H
#define _BF_H

#include "BOBHash32.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <math.h>
#include <algorithm>


typedef unsigned long long ull;

#define HASH_MAX (0xFFFFFFFF)

class BF
{
	int n, m, k;
	int cell_num;
	ull * bitarray;
	BOBHash32 * bobhash[60];


public:
	BF();

	void insert(ull key);
	void insert_all(ull *keys, int item_num);

	int query(ull key);
	
	void init(int mm, int kk);
	void clear();


	double query_fp(ull *keys, int item_num);

	double query_fp_bloom();

	double query_up_bose();

	double query_up_ours();

	double query_opt_k_bloom();
	
	double query_opt_k_ours();


	~BF();
};
BF::BF()
{
	bitarray = NULL;
	for(int i = 0; i < 60; i++)
	{
		bobhash[i] = new BOBHash32(469 + 1* i);
	}
}

BF::~BF()
{
	delete[] bitarray;
	for(int i = 0; i < k; i++)
	{
		delete bobhash[i];
	}
}
void BF::insert(ull key)
{
	n++;
	int index = 0;
	for(int i = 0; i < k; i ++)
	{
		index = bobhash[i]->run((const char *)&key, 8) % m;

		bitarray[index >> 6] |= (((ull)1) << (index & 0x3F));
	}
}
void BF::insert_all(ull *keys, int item_num)
{
	for(int i = 0; i < item_num; i++)
	{
		insert(keys[i]);
	}
}
int BF::query(ull key)
{
	int index = 0;
	for(int i = 0; i < k; i ++)
	{
		index = bobhash[i]->run((const char *)&key, 8) % m;
		if(((bitarray[index >> 6] >> (index & 0x3F)) & 1) == 0)
		{
			return 0;
		}
	}	
	return 1;
}
void BF::init(int mm, int kk)
{
	n = 0;
	m = mm;
	k = kk;
	
	cell_num = (m >> 6) + 1;
	if(bitarray != NULL)
		delete []bitarray;

	bitarray = new ull[cell_num];
	
	memset(bitarray, 0, sizeof(ull) * cell_num);
}

void BF::clear()
{
	memset(bitarray, 0, sizeof(ull) * cell_num);
}
double BF::query_fp(ull *keys, int item_num)
{
	double fp_num = 0;
	for(int i = 0; i < item_num; i++)
	{
		if(query(keys[i]) == 1)
		{
			fp_num ++;
		}
	}
	return fp_num / item_num;
}
double BF::query_fp_bloom()
{
	double temp = pow(1.0 - 1.0 / m, k * n * 1.0);
	return pow(1.0 - temp, k * 1.0);
}
double ln(double x)
{
	return log(x) / log(2.71828);
}
double BF::query_up_bose()
{
	double temp = pow(1.0 - 1.0 / m, k * n * 1.0);
	double p = 1.0 - temp;

	double temp2 = sqrt((ln(m * 1.0) - 2.0 * k * ln(p * 1.0)) / m);
	return (1 + k * 1.0 * temp2 / p) * query_fp_bloom();
}
double BF::query_up_ours()
{
	double temp = pow(1.0 - k * 1.0 / m, n * 1.0);
	return pow(1.0 - temp, k * 1.0);
}

double BF::query_opt_k_bloom()
{
	return m * 1.0 * ln(2.0) / n;
}
	
double BF::query_opt_k_ours()
{
	double temp = 0.0 - ln(1.0 - 1.0 / m);
	return (ln(2.0) / n) / temp;
}
#endif//_BF_H