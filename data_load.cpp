#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <time.h>
#include <windows.h>
#include <algorithm>

using namespace std;

int main(int argc, const char * argv[]) {

	vector< vector<float> > X_train;
	vector<float> y_train;

	ifstream myfile("test.txt");

	if (myfile.is_open())
	{
		cout << "Loading data ...\n";
		string line;
		while (getline(myfile, line))
		{
			int x, y;
			vector<float> X;
			stringstream ss(line);
			ss >> y;
			y_train.push_back(y);
			for (int i = 0; i < 28 * 28; i++) {
				ss >> x;
				X.push_back(x/255.0);
			}
			X_train.push_back(X);
		}

		myfile.close();
		cout << "Loading data finished.\n";
		
			for (int i = 1; i < 28 * 28; i++) {

				if(X_train[1][i] == 0){
					cout << "0";
				}else{
					cout << "*";
				}
				if(i%28 == 0){
					cout << endl;
				}
			}
			cout << endl;
			cout << "The first number is " <<y_train[0] << endl;
		
		cout << "X_train have " << X_train.size() << "sample" << endl;
		cout << "y_train have " << y_train.size() << "sample" << endl;
	} 
	else 
		cout << "Unable to open file" << '\n';

	return 0;
}