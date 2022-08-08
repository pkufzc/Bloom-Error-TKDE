#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
 
#include "BF.h"
#include "CBF.h"
using namespace std;

#define _BF
#define _CBF

int DATA_SIZE,UNIT;

ofstream fou;
ifstream fi;
string path="../datasets/";

#ifdef _BF
ull items_bf[4 * 1500000 + 1000000];

bool prime(int x){
	for(int i=2;i*i<=x;i++){
		if(x%i==0) return 0;
	}
	return 1;
}
void test_bf(int n, int k, int m, BF* bf, ofstream* fou)
{
	int offset;
	for(int i=1;;i+=1){
		if(prime(m+i)){
			offset=i;
			break;
		}
	}
	
	bf->init(m+offset, k);

	bf->insert_all(items_bf, n);

	double emprical_fp = bf->query_fp(items_bf + DATA_SIZE, DATA_SIZE * 3);
	double bloom_fp = bf->query_fp_bloom();
	double bose_up = bf->query_up_bose();
	double ours_up = bf->query_up_ours();

	double opt_k_bloom = bf->query_opt_k_bloom();
	double opt_k_ours = bf->query_opt_k_ours();

	
	int tmp = (int)opt_k_ours;

	int emprical_opt_k=tmp;
	bf->clear();
	bf->init(m+offset, tmp);
	bf->insert_all(items_bf, n);
	double min_fp = bf->query_fp(items_bf + DATA_SIZE, DATA_SIZE * 3);

	
	for(int i=max(1,tmp-5);i<=min(60,tmp+5);i++){
		bf->clear();
		bf->init(m+offset, i);
		bf->insert_all(items_bf, n);
		double tmp_fp = bf->query_fp(items_bf + DATA_SIZE, DATA_SIZE * 3);
		
		if(tmp_fp<min_fp){
			min_fp = tmp_fp;
			emprical_opt_k = i;
		}
	}

	
	*fou<<n<<","<< k<<","<< m+offset<<","<< emprical_fp<<","<< bloom_fp<<","<< bose_up<<","<< ours_up<<","<< (bose_up - bloom_fp) / bloom_fp<<","<< (ours_up - bloom_fp) / bloom_fp<<","<< opt_k_bloom<<","<< opt_k_ours<<","<<emprical_opt_k<<"\n";
	
}

void bf_exp()
{

	
	int item_cnt = 0;

        fi.open(path+"_distinct.csv");
        while(fi>>items_bf[item_cnt]){
                item_cnt++;
        }
        fi.close();
	


	BF * bf = new BF();
	fou.open(path+"_data.csv");
	

	printf("********************************************** BF test *********************************************\n");
	
	printf("bf test-1 (varying n) starts!\n");
	fou<<"n"<<","<<"k"<<","<<"m"<<","<<"Empirically FP probability,FP probability of Bloom (lower bound),Upper bound of Bose,Upper bound of ours,Bounds error ratio of Bose,Bounds error ratio of ours,Theoretically optimal k of Bloom,Theoretically optimal k of ours,Empirically optimal k\n";
	for(int n = UNIT; n <= 10 * UNIT; n += UNIT)
	{
		test_bf(n, 6, 50 * UNIT, bf, &fou);
		bf->clear();
	}
	fou<<"\n";
	printf("bf test-1 (varying n) ends!\n");
	

	printf("bf test-2 (varying k) starts!\n");
	fou<<"n"<<","<<"k"<<","<<"m"<<","<<"Empirically FP probability,FP probability of Bloom (lower bound),Upper bound of Bose,Upper bound of ours,Bounds error ratio of Bose,Bounds error ratio of ours,Theoretically optimal k of Bloom,Theoretically optimal k of ours,Empirically optimal k\n";
	for(int k = 3; k <= 12; k++)
	{
		test_bf(5 * UNIT, k, 50 * UNIT, bf, &fou);
		bf->clear();
	}
	fou<<"\n";
	printf("bf test-2 (varying k) ends!\n");
	

	printf("bf test-3 (varying m) starts!\n");
	fou<<"n"<<","<<"k"<<","<<"m"<<","<<"Empirically FP probability,FP probability of Bloom (lower bound),Upper bound of Bose,Upper bound of ours,Bounds error ratio of Bose,Bounds error ratio of ours,Theoretically optimal k of Bloom,Theoretically optimal k of ours,Empirically optimal k\n";
	for(int m = 10 * UNIT; m <= 100 * UNIT; m += 10 * UNIT)
	{
		test_bf(5 * UNIT, 6, m, bf, &fou);
		bf->clear();
	}
	fou<<"\n";
	printf("bf test-3 (varying m) ends!\n");
	

	printf("****************************************************************************************************\n\n");


}

#endif //_BF

#ifdef _CBF
ull items_cbf[4*1500000 + 1000000];
int items_fs[4*1500000 + 1000000];

void test_cbf(int n, int k, int m, CBF* cbf, ofstream* fou)
{
	int offset;
	for(int i=1;;i+=1){
		if(prime(m+i)){
			offset=i;
			break;
		}
	}
	cbf->init(m+offset, k);

	cbf->insert_all(items_cbf, items_fs, n);

	double emprical_cr = cbf->query_cr(items_cbf, items_fs, n);
	double upper_cr = cbf->query_cr_upper();
	double lower_cr = cbf->query_cr_lower();


	
	*fou<<n<<","<< k<<","<< m+offset<<","<< emprical_cr<<","<< upper_cr<<","<< lower_cr<<"\n";

}

void cbf_exp()
{	
	
	int item_cnt = 0;


        fi.open(path+"_cnt.csv");
        while(fi>>items_cbf[item_cnt]){
                fi>>items_fs[item_cnt];
                item_cnt++;
        }
        fi.close();
	

	CBF * cbf = new CBF();

	

	printf("********************************************** CBF test *********************************************\n");
	


	fou<<"n,k,m,Empirically correct rate,Theoretically upper bound,Theoretically lower bound\n";
	printf("cbf test-1 (varying n) starts!\n");
	
	for(int n = UNIT; n <= 10 * UNIT; n += UNIT)
	{
		test_cbf(n, 6, 50 * UNIT, cbf, &fou);
		cbf->clear();
	}
	
	printf("cbf test-1 (varying n) ends!\n\n");
	fou<<"\n";
 
	
	fou<<"n,k,m,Empirically correct rate,Theoretically upper bound,Theoretically lower bound\n";
	printf("cbf test-2 (varying k) starts!\n");
	
	for(int k = 3; k <= 12; k++)
	{
		test_cbf(5 * UNIT, k, 50 * UNIT, cbf, &fou);
		cbf->clear();
	}
	
	printf("cbf test-2 (varying k) ends!\n\n");
	

	fou<<"\n"<<"n,k,m,Empirically correct rate,Theoretically upper bound,Theoretically lower bound\n";
	printf("cbf test-3 (varying m) starts!\n");
	
	for(int m = 10 * UNIT; m <= 100 * UNIT; m += 10 * UNIT)
	{
		test_cbf(5 * UNIT, 6, m, cbf, &fou);
		cbf->clear();
	}
	
	printf("cbf test-3 (varying m) ends!\n\n");
	fou<<"\n";
	printf("*****************************************************************************************************\n");
	fou.close();
}
#endif //_CBF





int main(int argc, char ** argv)
{

	string x=argv[1];
	if(x=="dc"){
		DATA_SIZE=1500000;
		UNIT=150000;
	}else if(x=="net"){
		DATA_SIZE=170000;
		UNIT=17000;
	}else if(x=="syn"){
		DATA_SIZE=230000;
		UNIT=23000;
	}else{
		DATA_SIZE=0;
		UNIT=0;
	}
	path=path+x+"/"+x;

#ifdef _BF
	bf_exp();
#endif //_BF

#ifdef _CBF
	cbf_exp();
#endif //_CBF

	return 0;
}
