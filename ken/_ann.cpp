#include <vector>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
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
		ANN(vector<int> &input);
		void feedforward(vector<float> &inputData);
		float activationfunction(float sum);
		float derivativeactivationfunction(float sum);
		void train(int expectOutput);
		void printResult(int y);
		void lossfunction(vector<float> y);
		void updateWeight();
		void importfile(string filename);
		void exportfile(string filename);
		void importFile(string filename);
		int right = 0;
		int wrong = 0;
		int epoch = 2;
		vector< vector<float> > X_train;
		vector<float> y_train;
	private:
		vector<vector<Neuron>> neuron;
		vector<float> bias;
		float learingRate = 0.05;

		
};

void ANN::printResult(int y){
	float answer = 0;
	float answerNo = 0;
	
	//cout << "Output: [ " ;
	for(int i = 0; i < neuron.back().size();i++){
		float temp = neuron.back()[i].getOutput();
		//cout << i << " : " << temp << " | ";
		if(temp > answer){
			
			answerNo = i;
			answer = temp;
			
		}
	}
	//cout << "]" << endl;
	cout << "Output:  " << answerNo << " ("<< answer <<")"<< endl;
	if(y == answerNo){
		right++;
		cout << "Your answer is true" << endl;
	}
	else{
		wrong++;
		cout << "Your answer is false" << endl;
	} 
	cout << endl;
}

void ANN::lossfunction(vector<float> y){
	float totalLoss = 0;

	
	//cal Output layer
	for(int n = 0 ; n < neuron.back().size(); n++){
		//cout << "Y is " << y[n] << endl;
		neuron.back()[n].setLoss(((neuron.back()[n].getOutput())-y[n]) * derivativeactivationfunction(neuron.back()[n].getOutput()));
		//cout << n << " : "<< neuron.back()[n].getOutput() << endl;
		totalLoss += (neuron.back()[n].getOutput()-y[n]) * (neuron.back()[n].getOutput()-y[n]);
	}
	
	totalLoss = totalLoss/10;
	totalLoss = sqrt(totalLoss);
	
	
	//calculate hidden layer
	for(int hiddenlayer = neuron.size()-2; hiddenlayer > 0 ; hiddenlayer--){
		int nextsize = neuron[hiddenlayer+1].size();
		if(hiddenlayer != neuron.size()-2)nextsize--;
		
		for(int n = 0; n < neuron[hiddenlayer].size()-1; n++){
			float sum = 0;
			for(int nextN = 0; nextN < nextsize; nextN++){
				//cout << "Hidden weight val: " << neuron[hiddenlayer][n].weight[nextN] << endl;
				//cout << "Gradient val: " << neuron[hiddenlayer+1][nextN].getLoss() << endl;
				
				sum += neuron[hiddenlayer+1][nextN].getLoss() * neuron[hiddenlayer][n].weight[nextN];
				
			}
			
			//cout << "Hidden ouput val: " << neuron[hiddenlayer][n].getOutput() << endl;
			
			neuron[hiddenlayer][n].setLoss(sum * derivativeactivationfunction(neuron[hiddenlayer][n].getOutput()));
			
			//cout << n << " : "<< neuron[hiddenlayer][n].getLoss() << endl;
		}
	}
	
	
	
	
	
	
	
	updateWeight();
	//cout << "Total Loss : " << totalLoss << endl;
	
}

void ANN::updateWeight(){
	
	for(int layer = neuron.size()-1; layer > 0 ; layer--){
		int prevLayer = layer - 1;

		//cout<<"Layer! " << prevLayer<<endl;
		for(int n = 0; n < neuron[layer].size(); n++){
			//cout<<"Neuron!" << n<<endl;
			for(int prevN = 0; prevN < neuron[prevLayer].size(); prevN++){			
				float oldweight = neuron[prevLayer][prevN].weight[n];
				
				if(prevN == neuron[prevLayer].size()-1){
					//cout << n << endl;
					//cout << "oldWeight is " << neuron[prevLayer][prevN].weight[n];
					//cout << "loss is " << neuron[layer][n].getLoss() << endl;
					neuron[prevLayer][prevN].weight[n] = neuron[prevLayer][prevN].weight[n] - (learingRate * neuron[layer][n].getLoss());
				}else{
					//cout << oldweight << " - " << (learingRate * neuron[layer][n].getLoss() * neuron[prevLayer][prevN].getOutput());
					neuron[prevLayer][prevN].weight[n] = oldweight - (learingRate * neuron[layer][n].getLoss() * neuron[prevLayer][prevN].getOutput());
				}

				//cout << " update" << neuron[prevLayer][prevN].weight[n] << endl;
			}
			
		}
		
	}
	
	

}

void ANN::train(int expectOutput){
	
	vector<float> y(10,0);
	
	y[expectOutput] = 1;
	
	lossfunction(y);
	
	
}

float ANN::derivativeactivationfunction(float sum){
	return  activationfunction(sum) * (1.0 - activationfunction(sum));
}

float ANN::activationfunction(float sum){
	//cout << sum << endl;
	return 1/(1+exp(-sum));
}

void ANN::feedforward(vector<float> &inputData){
	assert(inputData.size() == (neuron[0].size() - 1));

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

ANN::ANN(vector<int> &input){
	
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
			neuron[i].back().setOutput(0);
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
	test.push_back(784); 		//input layer
	test.push_back(16);		//hidden layer
	test.push_back(10);		//output layer
	ANN mynet(test);

	
	mynet.importFile("train.txt");
	
	mynet.exportfile("./inputbefore.txt");	//export weight before
	vector<float> inputData;
	
	for(int x = 0; x < mynet.epoch ; x++){
		for(int i = 0;i < mynet.X_train.size();i++){
			inputData = mynet.X_train[i];
			mynet.feedforward(inputData);
			int expectOutput = mynet.y_train[i];

			mynet.train(expectOutput);
			mynet.exportfile("./inputafter.txt");	//export weight after

			//mynet.printResult(expectOutput);//output

		}
	}
	
	cout << "Done!" << endl;
	
	mynet.importFile("train_small.txt");
	for(int i = 0; i < mynet.X_train.size();i++){

		mynet.feedforward(mynet.X_train[i]);
		
		mynet.printResult(mynet.y_train[i]);
	}
	
	cout << endl;
	cout << "Correct: " << mynet.right << endl;
	cout << "Wrong: " << mynet.wrong << endl;
	return 0;
}

void ANN::importFile(string filename){
		ifstream myfile("../"+filename);
	X_train.clear();
	y_train.clear();
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
}