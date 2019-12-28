#include <vector>
#include <iostream>
#include <stdlib.h>
#include <math.h>

#include <fstream>
#include <sstream>

using namespace std;


class Neuron{
	public:
		Neuron(int numOutputs);
		void setOutput(float output){this->output = output;}
		float getOutput(){return output;}
		void setLoss(float loss){this->loss = loss;}
		float getLoss(){return loss;}
		vector<float> weight;
		float gradient = 0;
	private:
		float output = 0;
		float loss = 0;
		
};

Neuron::Neuron(int numOutputs){
	for(int c = 0; c < numOutputs; c++){	
		weight.push_back(rand() / float(RAND_MAX));
	}
	//bais = rand() / float(RAND_MAX);
}

class ANN{
	public:
		ANN(vector<int> input);
		void feedforward(vector<float> inputData);
		float activationfunction(float sum);
		float derivativeactivationfunction(float sum);
		void train(int expectOutput);
		void printResult();
		void lossfunction(vector<float> y);
		void updateWeight();
		void importfile(string filename);
		void exportfile(string filename);
	private:
		vector<vector<Neuron>> neuron;
		vector<float> bias;
		float learingRate = 0.5;
		int epoch = 10;
};

void ANN::printResult(){
	cout << "Output: [ " ;
	for(int i = 0; i < neuron.back().size();i++){
		cout << i << " : " << neuron.back()[i].getOutput() << " | ";
	}
	cout << "]" << endl;
	
}

void ANN::lossfunction(vector<float> y){
	float totalLoss = 0;
	
	//cal Output layer
	for(int n = 0 ; n < neuron.back().size(); n++){
		neuron.back()[n].setLoss((neuron.back()[n].getOutput()-y[n])*neuron.back()[n].getOutput()*(1-neuron.back()[n].getOutput()));
		//cout << n << " : "<< neuron.back()[n].getLoss() << endl;
		totalLoss += neuron.back()[n].getLoss();
	}

	//calculate hidden layer
	for(int hiddenlayer = neuron.size()-2; hiddenlayer > 0 ; hiddenlayer--){
		int nextsize = neuron[hiddenlayer+1].size();
		if(hiddenlayer != neuron.size()-2)nextsize--;
		
		for(int n = 0; n < neuron[hiddenlayer].size()-1; n++){
			float sum = 0;
			for(int prevN = 0; prevN < nextsize; prevN++){
				sum += neuron[hiddenlayer+1][prevN].getLoss()*neuron[hiddenlayer][n].weight[prevN];
			}

			neuron[hiddenlayer][n].setLoss(sum * neuron[hiddenlayer][n].getOutput() * ( 1 - neuron[hiddenlayer][n].getOutput()));
			//cout << n << " : "<< neuron[hiddenlayer][n].getLoss() << endl;
			totalLoss += neuron[hiddenlayer][n].getLoss();
		}
	}
	
	
	
	
	
	
	
	updateWeight();
	cout << "Total Loss : " << totalLoss << endl;
	
}

void ANN::updateWeight(){

	for(int layer = neuron.size()-2; layer >= 0; layer--){
		int nextsize = neuron[layer+1].size();
		if(layer != neuron.size()-2)nextsize--;
		
		for(int n = 0; n < neuron[layer].size()-1; n++){
			for(int nextN = 0; nextN < nextsize; nextN++){
				neuron[layer][n].weight[nextN] = neuron[layer][n].weight[nextN] - learingRate * neuron[layer+1][nextN].getLoss() * neuron[layer][n].getOutput();
			}
		}
	}
	
	
	

}

void ANN::train(int expectOutput){
	
	vector<float> y;
	
	for(int i = 0;i<10;i++){
		if(expectOutput == i){
			y.push_back(1);
		}else{
			y.push_back(0);
		}
	}
	
	lossfunction(y);
	
	
}

float ANN::derivativeactivationfunction(float sum){
	return 1/(1+exp(-sum))*(1-1/(1+exp(-sum)));
}

float ANN::activationfunction(float sum){
	//cout << sum << endl;
	return 1/(1+exp(-sum));
}

void ANN::feedforward(vector<float> inputData){
	

	//set input layer Output
	for(int i=0;i<inputData.size();i++){
		neuron[0][i].setOutput(inputData[i]);
	}

	for(int layer = 0 ; layer < neuron.size()-1; layer++){
		int nextsize = neuron[layer+1].size();
		if(layer != neuron.size()-2)nextsize--;

		for(int nextNeuron = 0; nextNeuron < nextsize; nextNeuron++){
			float sum = 0;
			
			for(int n = 0 ; n < neuron[layer].size(); n++){

				sum += neuron[layer][n].getOutput() * neuron[layer][n].weight[nextNeuron];
				
			}
			neuron[layer+1][nextNeuron].setOutput(activationfunction(sum));
		}
		
	}


};

ANN::ANN(vector<int> input){
	
	for(int i = 0; i < input.size();i++){
		neuron.push_back(vector<Neuron>());
		int numOutputs = i == input.size() - 1 ? 0 : input[i + 1];
		for(int j = 0;j < input[i];j++){
			neuron[i].push_back(Neuron(numOutputs));
		}
		if(i != input.size()-1){ // set Bias
			neuron[i].push_back(Neuron(0));
			float w = rand() / float(RAND_MAX);
			for(int c = 0; c < numOutputs; c++){	
				neuron[i].back().weight.push_back(w);
			}
			neuron[i].back().setOutput(1);
		}
		
	}

}

void ANN::exportfile(string filename){
	ofstream outFile;
    outFile.open(filename);

    for (int i = 0; i < neuron.size(); i++){
		for(int j = 0; j < neuron[i].size(); j++){
			for(int k = 0; k < neuron[i][j].weight.size(); k++){		
				outFile << neuron[i][j].weight[k] << endl;
			}
		}
    } 

    outFile.close();
}

void ANN::importfile(string filename){
	ifstream inFile;
    inFile.open(filename);

	for (int i = 0; i < neuron.size(); i++){
		for(int j = 0; j < neuron[i].size(); j++){
			for(int k = 0; k < neuron[i][j].weight.size(); k++){		
				inFile >> neuron[i][j].weight[k];
			}
		}
    }
}

int main(){
	vector<int> test;
	test.push_back(784); 	//input layer
	test.push_back(16);		//hidden layer
	test.push_back(16);		//hidden layer
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
	
	mynet.exportfile("./inputbefore.txt");
	vector<float> inputData;
	
	for(int i = 0;i < X_train.size();i++){
		inputData = X_train[i];
		mynet.feedforward(inputData);
		int expectOutput = y_train[i];

		mynet.train(expectOutput);
		mynet.exportfile("./inputafter.txt");
		
		cout <<"Answer me:" << y_train[i] << endl;
		mynet.printResult();//output
		//mynet.feedforward(inputData);
		
		//mynet.printResult();//output
	}
	
	
	return 0;
}