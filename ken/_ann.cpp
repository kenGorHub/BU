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
		void setBias(float b){bais = b;}
		void setOutput(float output){this->output = output;}
		float getBias(){return bais;}
		float getOutput(){return output;}
		vector<float> weight;
	private:
		float bais;
		float output;
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
		void train(vector<float> resultData);
		void printResult();
		void lossfunction(vector<float> y);
		void updateWeight(vector<vector<float>> errors);
	private:
		vector<vector<Neuron>> neuron;
		float learingRate = 0.1;
		int epoch = 10;
};

void ANN::printResult(){
	cout << "Output: [ " ;
	for(int i = 0;i<neuron.back().size();i++){
		cout << neuron.back()[i].getOutput() << " | ";
	}
	cout << "]" << endl;
}

void ANN::lossfunction(vector<float> y){
	//cal Output layer
	vector<float> dAverageLoss;
 	for(int i=0;i<neuron.back().size();i++){
		dAverageLoss.push_back(neuron.back()[i].getOutput() - y[i]);
	}
	vector<float> dOutput;
	for(int j = 0;j<neuron[neuron.size()-2].size();j++){
		float sum = 0;
		for(int i = 0; i<neuron.back().size();i++){
			sum +=neuron[neuron.size()-2][j].weight[i] * neuron.back()[i].getOutput();
			
		}
		dOutput.push_back(derivativeactivationfunction(sum));
		
	}
	vector<vector<float>> errors;
	vector<float> error;
	
	for(int i =0;i<dAverageLoss.size();i++){
		error.push_back(dAverageLoss[i] * dOutput[i]);
	}
	errors.push_back(error);
	updateWeight(errors);
	
	vector<vector<float>> m_w;//updated weight
	for(int j= 0;j < neuron[neuron.size()-2].size();j++){
		m_w.push_back(vector<float>(neuron.back().size()));
		for(int i = 0;i< neuron.back().size();i++){
			m_w[j][i]=neuron[neuron.size()-2][j].weight[i];
		}
	}
	vector<float> temp;
	for(int i=0;i<m_w.size();i++){
		float sum =0;
		for(int j=0;j<m_w[i].size();j++){
			sum += m_w[i][j] * error[i];
		}
		temp.push_back(sum);
	}
	
	dOutput.clear();
	for(int k =neuron.size()-2;k>=1  ;k--){
		for(int j = 0;j<neuron[k].size();j++){
			float sum = 0;
			for(int i = 0; i<neuron[k-1].size();i++){
				sum +=neuron[k][j].weight[i] * neuron[k][j].getOutput();
			}
			dOutput.push_back(derivativeactivationfunction(sum));
		}
	}
	error.clear();
	for(int i =0;i<dOutput.size();i++){
		error.push_back(dOutput[i]*temp[i]);
	}
	
	errors.push_back(error);
	
	updateWeight(errors);
	
	
}

void ANN::updateWeight(vector<vector<float>> errors){
	int nowlayer = neuron.size()-1;
	for(int k = 0; k< errors.size();k++){
		//update last hidden's weight

		for(int i = 0;i<errors[k].size();i++){
			int sum = 0;
			
			for(int j = 0;j<neuron[nowlayer-1].size();j++){
				sum += errors[k][i] * neuron[nowlayer-1][j].getOutput();
			}
			
			sum = sum * learingRate / neuron[nowlayer-1].size();
			
			for(int j = 0;j<neuron[nowlayer-1].size();j++){
				for(int neuronNum = 0; neuronNum < neuron[nowlayer].size();neuronNum++){
					neuron[nowlayer-1][j].weight[i] -= sum;

				}
			}
			
			
		}
		nowlayer--;
	}
}

void ANN::train(vector<float> resultData){
	
	vector<float> y;
	
	for(int i = 0;i<10;i++){
		if(resultData[0] == i){
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
	return 1/(1+exp(-sum));
}

void ANN::feedforward(vector<float> inputData){
	
	
	//set input layer Output
	for(int i=0;i<inputData.size();i++){
		neuron[0][i].setOutput(inputData[i]);
	}

	for(int layerNum = 0;layerNum < neuron.size()-1;layerNum++){
		for(int nextNeuronNum = 0;nextNeuronNum<neuron[layerNum+1].size();nextNeuronNum++){
			float sum = 0;				
			for(int outputNum = 0;outputNum<neuron[layerNum].size();outputNum++){
				sum += neuron[layerNum][outputNum].getOutput() * neuron[layerNum][outputNum].weight[nextNeuronNum];
			}
			sum += neuron[layerNum+1][nextNeuronNum].getBias();
			neuron[layerNum+1][nextNeuronNum].setOutput(activationfunction(sum));
			//cout << "Layer:" << layerNum << " nextNeuronNum:" << nextNeuronNum  << " Output :" << neuron[layerNum+1][nextNeuronNum].getOutput() << endl;
			
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
	}

}



int main(){
	vector<int> test;
	test.push_back(784); 	//input layer
	test.push_back(2);		//hidden layer
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
	
	mynet.printResult();//output
	
	vector<float> resultData;
	resultData = y_train;
	mynet.train(resultData);
	
	cout <<"Answer me:" << y_train[0] << endl;
	mynet.feedforward(inputData);
	
	mynet.printResult();
	return 0;
}