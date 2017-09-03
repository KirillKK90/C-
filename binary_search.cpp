//______________________________________________________________________________________________________________________
// Binary search implementation for algorithm studying purpose.
// It is implemented here in both recursive and non-recursive forms.
//______________________________________________________________________________________________________________________
// It is proven that the problem of finding given number out of sorted array of numbers has
// the best solution. Binary search is fastest one and is O(log2 N).
//______________________________________________________________________________________________________________________
// And the problem of finding given element out of array of elements is often reduced to the search problem.


#include "stdafx.h"
#include <stdio.h>
#include <math.h>

//#define RECURSIVE 1


#ifdef RECURSIVE
#define SEARCH binSearchRecursive
#else
#define SEARCH binSearch
#endif

#define ASSERT_EQ(x, y) if (x == y) printf("OK. \n"); else printf("Fail: x == %d, y == %d. \n", x, y ); 

// NOTE: input array should be sorted in the ascending order
int binSearchRecursive(int, int, int*);
void lookUp(int, int, int, int*, int&);
int mod(int);

int binSearch(int, int, int*);
double log2(double);


const int size = 10;
int data[size];

//	TESTS
void test1()
{
	data[0] = 4;
	ASSERT_EQ(0, SEARCH(4, 1, data)); 
}

void test2_1()
{
	data[0] = 3;
	data[1] = 4;
	ASSERT_EQ(0, SEARCH(3, 2, data)); 
}

void test2_2()
{
	data[0] = 3;
	data[1] = 4;
	ASSERT_EQ(1, SEARCH(4, 2, data)); 
}

void test3_1()
{
	data[0] = 3;
	data[1] = 4;
	data[2] = 7;
	ASSERT_EQ(0, SEARCH(3, 3, data)); 
}

void test3_2()
{
	data[0] = 3;
	data[1] = 4;
	data[2] = 7;
	ASSERT_EQ(1, SEARCH(4, 3, data)); 
}

void test3_3()
{
	data[0] = 3;
	data[1] = 4;
	data[2] = 7;
	ASSERT_EQ(2, SEARCH(7, 3, data)); 
}


void test10()
{
	for(int i = 0; i < size; ++i)
		data[i] = i + 1;
	ASSERT_EQ(4, SEARCH(5, size, data)); 
}


void test1000()
{
	const int sz = 1001;
	int largeArr[sz];
	for(int i = 0; i < sz; ++i)
		largeArr[i] = i + 1;
	ASSERT_EQ(450, SEARCH(451, sz, largeArr)); 
}

void testVar()
{
	const int sz = 10;
	int largeArr[sz];
	largeArr[0] = 2;
	largeArr[1] = 6;
	largeArr[2] = 22;
	largeArr[3] = 47;
	largeArr[4] = 79;
	largeArr[5] = 101;
	largeArr[6] = 250;
	largeArr[7] = 703;
	largeArr[8] = 900;
	largeArr[9] = 999;
	ASSERT_EQ(5, SEARCH(101, sz, largeArr)); 
}

int _tmain(int argc, _TCHAR* argv[])
{
	test1();
	test2_1();
	test2_2();
	test3_1();
	test3_2();
	test3_3();
	test10();
	test1000();
	testVar();

	return 0;
}

int mod(int x)
{
	if(x < 0) return -x;
	else return x;
}

int binSearch(int pat, int n, int *m)
{
	int res = -1;
	if(n == 2) 
	{
		for(int i = 0; i < n; ++i)
		{
			if(m[i] == pat) res = i;
		}
	}
	else
	{
		int i = 0;
		int l = 0;
		int h = n - 1;

		//code improvement
		for(int j = 0; j < (int)log2(n) + 1; ++j)
		{
			i = (h + l) / 2; 
			if(h == l + 1 && l != 0) i = h; 
			if(m[i] < pat)  l = i; 
			else if(m[i] > pat) h = i;
			else res = i;
		}
	}
	return res;
}


void lookUp(int pat, int l, int h, int *m, int &res)
{
	int i = (h - l) / 2; 
	if(i == 0 && l != 0 ) i = h - l;
	if(m[l + i] < pat) lookUp(pat, l + i, h, m, res);
	else if(m[l + i] > pat) lookUp(pat, l, l + i, m, res);
	else res = i + l;
	return;
}

int binSearchRecursive(int pat, int n, int *m)
{
	int res = -1;
	if(n == 2) 
	{
		for(int i = 0; i < n; ++i)
		{
			if(m[i] == pat) res = i;
		}
	}
	else lookUp(pat, 0, n - 1, m, res);
	return res;
}

double log2( double n )
{
	// log(n)/log(2) is log2(n).
	return log( n ) / log( (double)2 );
}
