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
		s.push_back("����");
		s.push_back("����");
		s.push_back("���");
		s.push_back("���");
		s.push_back("������");
		s.push_back("����");
		s.push_back("�����");
		s.push_back("����");
		s.push_back("������");
		s.push_back("������");

		d.push_back("��������");
		d.push_back("��������");
		d.push_back("�����");
		d.push_back("���������");
		d.push_back("����������");
		d.push_back("���������");
		d.push_back("�����������");
		d.push_back("���������");

		t.push_back("������");
		t.push_back("�����������");
		t.push_back("����������");
		t.push_back("����������");
		t.push_back("������������");
		t.push_back("����������");
		t.push_back("�����������");
		t.push_back("����������");
		t.push_back("������������");
		t.push_back("������������");
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
		if(i == 1) temp = "���";
		else if(i == 2) temp = "������";
		else if(i >= 3 && i <= 4) 
		{
			temp = s[i] + "���";
		}
		else
		{
			temp = s[i] + "���";
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

