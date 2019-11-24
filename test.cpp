#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include "Vector.cpp"
using namespace std;

vector<Vector<int>> X;
Vector<int> y;
int main () {
	ifstream  infile;
	infile.open("test.txt");
	string s ;
	while(getline(infile,s)){
		size_t pos = 0;
		string delimiter = "	"; 
		string num=s.substr(0, 1);
		y.add(stof(num));
		s.erase(0, 1+delimiter.length());
		string token;
		Vector<int> temp;
		while ((pos = s.find(delimiter)) != string::npos) {
			token = s.substr(0, pos);
			s.erase(0, pos + delimiter.length());
			temp.add(stof(token));
		}
		temp.add(stof(s));
		X.push_back(temp);
	}
	infile.close();
	
	cout<<y.lengths()<<endl;cout<<X.size()<<endl;
	return 0;
}
void print(Vector<int> temp){
	for(int i=1;i<28*28+1;i++){
		int n = temp.get(i-1)-'0';
		if(n>0)
			cout<<"*";
		else
			cout<<"0";
		if(i%28==0)
			cout<<endl;
	}
}