#include <vector>
#include <iostream>
#include <stdlib.h>
#include <math.h>

#include <fstream>
#include <sstream>

using namespace std;


class Neuron{
	public:
		Neuron(){};
		void setBias(float b){bais = b;}
		void setWeight(vector<float> w){weight = w;}
		void setOutput(float output){this->output = output;}
		float getBias(){return bais;}
		float getWeight(){return weight;}
		float getOutput(){return output;}
	private:
		vector<float> weight;
		float bais;
		float output;
};

class ANN{
	public:
		ANN(vector<int> input);
		void feedforward(vector<float> inputData);
		float activefunction(float sum);
		void backpropagation(vector<float> resultData);
		void printResult();
	private:
		vector<vector<Neuron>> neuron;
};

void ANN::printResult(){
	cout << "Output: [ " ;
	for(int i = 0;i<neuron.back().size();i++){
		cout << neuron.back()[i].getOutput() << " | ";
	}
	cout << "]" << endl;
}

void ANN::backpropagation(vector<float> resultData){
	
}

float ANN::activefunction(float sum){
	return 1/(1+exp(-sum));
}

void ANN::feedforward(vector<float> inputData){
	
	
	//set input layer Output
	for(int i=0;i<inputData.size();i++){
		neuron[0][i].setOutput(inputData[i]);
	}
	
	for(int i = 0;i<neuron.size()-1;i++){
		float sum = 0;
		cout << "Layer "<<i<<endl;
		for(int j = 0;j<neuron[i].size();j++){
			cout<<"Neuron "<< j<< " output: "<< neuron[i][j].getOutput()<< " weight: "<< neuron[i][j].getWeight() << endl;
			sum += neuron[i][j].getOutput() * neuron[i][j].getWeight(); 
		}
		for(int k = 0;k<neuron[i+1].size();k++){
			float temp = sum;
			temp += neuron[i+1][k].getBias();
			neuron[i+1][k].setOutput(activefunction(temp));
		}
		
		
	}
}

ANN::ANN(vector<int> input){
	
	for(int i = 0; i < input.size();i++){
		neuron.push_back(vector<Neuron>());
		
		for(int j = 0;j < input[i];j++){
			float weight = rand() / float(RAND_MAX);
			neuron[i].push_back(Neuron());
			if(i != 0)neuron[i][j].setBias(1);
			neuron[i][j].setWeight(weight);
		}
	}
};



int main(){
	vector<int> test;
	test.push_back(784); 	//input layer
	test.push_back(3);		//hidden layer
	test.push_back(10);		//output layer
	ANN mynet(test);
	
	vector< vector<float> > X_train;
	vector<float> y_train;

	ifstream myfile("../train_small.txt");
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
	} 
	else 
		cout << "Unable to open file" << '\n';
	
	
	vector<float> inputData;
	inputData = X_train[0];
	mynet.feedforward(inputData);
	
	mynet.printResult();
	
	//vector<float> resultData;
	//mynet.backpropagation(resultData);
	return 0;
}