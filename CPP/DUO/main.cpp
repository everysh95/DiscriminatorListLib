#include<string>
#include<cstdio>
#include<iomanip>
#include<iostream>
#include<fstream>
#include<sstream>
#include"Discriminator.h"

int main(int argc,char** argv)
{
	using namespace base;
	using namespace std;
	d_creater<int> dc(1,2);
	//テスト用
	vector<vector<double> > case_list;
	vector<int> class_list;


	int n = 0;
	double rho = 0.0001;
	int d = 3;

	vector<double> base_v;
	base_v.push_back(1);
	bool ff = true;

	{
		ifstream fin(argv[1]);
		while(fin)
		{
			string bufs;
			getline(fin,bufs);
			n++;
		}
		fin.close();
	}
	n--;

	FILE* fp = fopen(argv[1],"r");
	for(int i = 0;i < n;i++)
	{
		vector<double> bv;
		bv.push_back(1);
		for(int j = 1;j < d;j++)
		{
			double buf;
			fscanf(fp,"%lf,",&buf);
			if(ff)
				base_v.push_back((buf != 0)? buf:1);
			bv.push_back((buf - base_v[j])/base_v[j]);
		}
		int bb;
		fscanf(fp,"%d",&bb);
		dc.add_case(bv,bb);
		case_list.push_back(bv);
		class_list.push_back(bb);
		ff = false;
	}
	fclose(fp);

	cout << "case size:" << dc.case_size() << endl;
	cout << endl;
	d_list<int> dl;
	dl = dc.study(2,rho,false,[](bool t,bool f,int i,int s){
		cout << "\e[1A\e[2K" << "Creating[" << setprecision(3)  << (s - i)/static_cast<double>(s)*100 << "%]" << endl;});
	cout << "Discriminator Size:" << dl.size() << endl;

	int umn = 0;
	vector<int> unis;
	for(int i = 0;i < n;i++)
	{
		if(i != 0)
		{
			cout << "\e[1A\e[2K" << flush;
			cout <<	"\e[1A\e[2K" << flush;
		}
		if(dl.run(case_list[i]) != class_list[i])
		{
			unis.push_back(i);
			umn++;
		}
		cout << "Matching[" << i + 1 << "/" << n << "]" << endl;
		cout << "UnMatch[" << umn << "/" << n << "]" << endl;
	}

	for(int i = 0;i < unis.size();i++)
	{
		cout << "Unmatch Parametor:" << flush;
		for(int j = 0;j < case_list[unis[i]].size();j++)
		{
			cout << case_list[unis[i]][j] << " " << flush;
		}
		cout << class_list[unis[i]] << endl;
		cout << "Unmatch Cost:" << dl.run_cost(case_list[unis[i]]) << endl;
	}

	return 0;

}
