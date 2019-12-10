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
		vector<float> weight;
		float gradient;
	private:
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
		void train(vector<float> resultData,int counter);
		void printResult();
		void lossfunction(vector<float> y);
		void updateWeight(int nowlayer);
		void importfile(string filename);
		void exportfile(string filename);
	private:
		vector<vector<Neuron>> neuron;
		vector<float> bias;
		float learingRate = 0.01;
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
			sum =neuron[neuron.size()-2][j].weight[i] * neuron.back()[i].getOutput();
			
		}
		sum += bias.back();
		dOutput.push_back(derivativeactivationfunction(sum));
		
	}

	//vector<float> error;//==gradient
	
	for(int i =0;i<dAverageLoss.size();i++){

		neuron.back()[i].gradient = dAverageLoss[i] * dOutput[i];
	}

	//calculate hidden layer
	vector<vector<float>> m_w;
	for(int j= 0;j < neuron[neuron.size()-2].size();j++){
		m_w.push_back(vector<float>(neuron.back().size()));
		for(int i = 0;i< neuron.back().size();i++){
			m_w[j][i]=neuron[neuron.size()-2][j].weight[i];
		}
	}
	vector<float> temp;
	for(int i=m_w.size()-1;i>0;i--){
		float sum =0;
		for(int j=0;j<m_w[i].size();j++){
			sum += m_w[i][j] * neuron[i][j].gradient;
		}
		temp.push_back(sum);
	}

	dOutput.clear();
	for(int k =neuron.size()-2; k > 0  ;k--){
		for(int j = 0;j<neuron[k].size();j++){
			float sum = 0;
			for(int i = 0; i<neuron[k-1].size();i++){
				sum = neuron[k-1][i].weight[j];// * neuron[k][j].getOutput();
			}
			sum += bias[k];
			//cout << bias[k] << endl;
			dOutput.push_back(derivativeactivationfunction(sum));
			neuron[k][j].gradient = dOutput[j]*temp[j];
		}
	}
	
	
	updateWeight(neuron.size()-1);
	
	
}

void ANN::updateWeight(int nowlayer){

	float sum2 = 0;
		//update weight

		for(int k= nowlayer-1;k>0 ;k--){
			for(int i = 0;i<neuron[k].size();i++){
				float sum = 0;
				
				sum2 += neuron[k][i].gradient ;
				
				sum += neuron[k][i].gradient * neuron[k][i].getOutput();
				

			
				sum = sum * learingRate / neuron[k].size();
			
			
				for(int j = 0;j<neuron[nowlayer].size();j++){
					
					neuron[k][i].weight[j] -= sum;	
				}
			}
				
			sum2 = sum2 * learingRate / neuron[nowlayer-1].size();

			bias[nowlayer] -= sum2;
		}


}

void ANN::train(vector<float> resultData,int counter){
	
	vector<float> y;
	
	for(int i = 0;i<10;i++){
		if(resultData[counter] == i){
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
			sum += bias[layerNum];
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
		bias.push_back(1.0);
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
	
	mynet.exportfile("C:\\Users\\ken\\Desktop\\COMP3046\\ken\\inputbefore.txt");
	vector<float> inputData;
	
	for(int i = 0;i<1;i++){
		inputData = X_train[i];
		mynet.feedforward(inputData);
		
		mynet.printResult();//output
		
		vector<float> resultData;
		resultData = y_train;
		mynet.train(resultData,i);
		mynet.exportfile("C:\\Users\\ken\\Desktop\\COMP3046\\ken\\inputafter.txt");
		
		cout <<"Answer me:" << y_train[i] << endl;
		mynet.feedforward(inputData);
		
		mynet.printResult();//output
	}
	
	
	return 0;
}