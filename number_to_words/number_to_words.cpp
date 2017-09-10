#include "stdafx.h"
#include "locale"

#include <string>
#include <vector>
#include <iostream>


using namespace std;


class InterfaceResourceWord
{
public:
	virtual string getOnes(int i) = 0;

	virtual string getDecades(int i) = 0;

	virtual string getTeens(int i) = 0;
	
	virtual string getHundreds(int i) = 0;
};


class ResourceWordRus : public InterfaceResourceWord
{
public:
	ResourceWordRus()
	{
		//s.reserve(10);
		initTables();
	}

	void initTables()
	{
		s.push_back("ноль");
		s.push_back("один");
		s.push_back("два");
		s.push_back("три");
		s.push_back("четыре");
		s.push_back("пять");
		s.push_back("шесть");
		s.push_back("семь");
		s.push_back("восемь");
		s.push_back("девять");

		d.push_back("двадцать");
		d.push_back("тридцать");
		d.push_back("сорок");
		d.push_back("пятьдесят");
		d.push_back("шестьдесят");
		d.push_back("семьдесят");
		d.push_back("восемьдесят");
		d.push_back("девяносто");

		t.push_back("десять");
		t.push_back("одиннадцать");
		t.push_back("двенадцать");
		t.push_back("тринадцать");
		t.push_back("цетырнадцать");
		t.push_back("пятнадцать");
		t.push_back("шестнадцать");
		t.push_back("семнадцать");
		t.push_back("восемнадцать");
		t.push_back("девятнадцать");
	}

	string getOnes(int i) 
	{
		i = i % 10;
		return s[i];
	}

	string getDecades(int i)
	{
		string temp;
		i = i / 10;
		if(i > 1) temp = d[i - 2];

		return temp;
	}

	string getTeens(int i)
	{
		return t[i - 10];
	}

	string getHundreds(int i)
	{
		string temp;
		i = i / 100;
		if(i == 1) temp = "сто";
		else if(i == 2) temp = "двести";
		else if(i >= 3 && i <= 4) 
		{
			temp = s[i] + "ста";
		}
		else
		{
			temp = s[i] + "сот";
		}

		return temp;
	}

	~ResourceWordRus()
	{
		//s.clear();
	}

private:
	vector<string> s;
	vector<string> d;
	vector<string> t;
};


class MyNumber
{
public:
	MyNumber(int input, InterfaceResourceWord* resource)
	{
		i = input;
		r = resource;
	}

	void displayNumber()
	{
		if(i >= 0 && i < 10) 
		{
			cout << r->getOnes(i) << "\n";
		}
		else if(i > 9 && i < 20)
		{
			cout << r->getTeens(i) << "\n";
		}
		else
		{
			string temp;
			temp = r->getDecades(i);
			if(i % 10)
			{
				temp = temp + " " + r->getOnes(i);
			}
			cout << temp << "\n";
		}

	}

private:
	int i;
	InterfaceResourceWord* r;
};


int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL,"Russian");

	int myNum = 19;
	ResourceWordRus myResource;
	MyNumber num(myNum, &myResource);
	num.displayNumber();


	return 0;
}

