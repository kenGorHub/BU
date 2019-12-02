#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>

using namespace std;

//Silly class to read training data from a file

class TrainingData{
	public:
		TrainingData(const string filename);
		bool isEof(void){return m_trainingDataFile.eof();}
		void getTopology(vector<vector<float>> &topology,vector<float> &targetOutputVals);
		//Returns the number of input values read from the file:
		unsigned getNextInputs(vector<float> &inputVals);
		unsigned getTargetOutputs(vector<float> &targetOutputVals);
	
	private:
		ifstream m_trainingDataFile;
};

void TrainingData::getTopology(vector<vector<float>> &topology,vector<float> &targetOutputVals){
	string line;
	
	
	while(getline(m_trainingDataFile,line)){
		stringstream ss(line);
		float s;
		ss >> s;
		targetOutputVals.push_back(s);
		vector<float> temp;
		while (!ss.eof()){
			float n;
			ss >> n;
			temp.push_back(n);
		}
		topology.push_back(temp);
	}
	return;
}

TrainingData::TrainingData(const string filename){
	m_trainingDataFile.open(filename.c_str());
}

unsigned TrainingData::getNextInputs(vector<float> &inputVals){
	inputVals.clear();
	
	string line;
	getline(m_trainingDataFile,line);
	stringstream ss(line);
		string s;
		ss >> s;
		while (!ss.eof()){
			float oneValue;
			ss >> oneValue;
			inputVals.push_back(oneValue);
		}
	return inputVals.size();
}

unsigned TrainingData::getTargetOutputs(vector<float> &targetOutputVals){
	targetOutputVals.clear();
	
	string line;
	while(getline(m_trainingDataFile,line)){
		stringstream ss(line);
		
			float oneValue;
			while(ss>>oneValue){
				targetOutputVals.push_back(oneValue);
			}
			
	}
	
	return targetOutputVals.size();
}

struct Connection{
	float weight;
	float deltaWeight;
};

class Neuron;

typedef vector<Neuron> Layer;

//*********************class Neuron*******************************
class Neuron {
	public:
		Neuron(unsigned numOutputs,unsigned myIndex);
		void setOutputVal(float val){m_outputVal = val;}
		float getOutputVal(void) const {return m_outputVal;}
		void feedForward(const Layer &prevLayer);
		void calcOutputGradients(float targetVal);
		void calcHiddenGradients(const Layer &nextLayer);
		void updateInputWeights(Layer &prevLayer);
	private:
		static float eta;//Individual input, magnified by the gradient and train rate:
		static float alpha;//Also add momentum = a fraction of the previous delta weight
		static float transferFunction(float x);
		static float transferFunctionDerivative(float x);
		static float randomWeight(void){return rand() / float(RAND_MAX);}
		float sumDOW(const Layer &nextLayer)const;
		float m_outputVal;
		vector<Connection> m_outputWeights;
		unsigned m_myIndex;
		float m_gradient;
};

float Neuron::eta = 0.1;//overall net learning rate,[0,0..1.0]
float Neuron::alpha = 0.5;//momentum,multiplier of last deltaweight,[0,0..n]

void Neuron::updateInputWeights(Layer &prevLayer){
	
	//The weight to be updated are in the Connection container
	//in the neurons in the preceding layer
	for(unsigned n = 0; n < prevLayer.size(); ++n){
		Neuron &neuron = prevLayer[n];
		float oldDeltaWeight = neuron.m_outputWeights[m_myIndex].deltaWeight;
		float newDeltaWeight = eta * neuron.getOutputVal() * m_gradient + alpha * oldDeltaWeight;
		
		
		neuron.m_outputWeights[m_myIndex].deltaWeight = newDeltaWeight;
		neuron.m_outputWeights[m_myIndex].weight = newDeltaWeight;
		
	}
}

float Neuron::sumDOW(const Layer &nextLayer)const {
	float sum = 0.0;
	
	//Sum our contributions of the errrors at the nodes we feed
	
	for(unsigned n = 0;n < nextLayer.size() - 1; ++n){
		sum += m_outputWeights[n].weight * nextLayer[n].m_gradient;
	}
	
	return sum;
}

void Neuron::calcHiddenGradients(const Layer &nextLayer){
	float dow = sumDOW(nextLayer);
	m_gradient = dow * Neuron::transferFunctionDerivative(m_outputVal);
}

void Neuron::calcOutputGradients(float targetVal){
	float delta = targetVal - m_outputVal;
	m_gradient = delta * Neuron::transferFunctionDerivative(m_outputVal);
}

float Neuron::transferFunction(float x){//active function
	//tanh - output range[-1.0..1.0]
	//return tanh(x);
	return 1 / (1 + exp(-x));
}

float Neuron::transferFunctionDerivative(float x){
	//tanh derivative
	return 1.0 - ( 1 / (1 + exp(-x)) );
}

void Neuron::feedForward(const Layer &prevLayer){
	float sum = 0.0;
	//Sum the previous layer's outputs (which are out inputs)
	//Include the bias node from the previous layer.
	for(unsigned n = 0;n < prevLayer.size();++n){
		sum += prevLayer[n].getOutputVal() *
				prevLayer[n].m_outputWeights[m_myIndex].weight;
	}
	//sum += prevLayer.back().getOutputVal();
	m_outputVal = Neuron::transferFunction(sum);
}

Neuron::Neuron(unsigned numOutputs, unsigned myIndex){
	for(unsigned c = 0; c < numOutputs; ++c){
		m_outputWeights.push_back(Connection());
		m_outputWeights.back().weight = randomWeight();
	}
	
	m_myIndex = myIndex;
}
//*********************class Net*******************************
class Net{
public:
	Net(const vector<unsigned> &tp);
	void feedForward(const vector<float> &inputVals);
	void inference(const vector<float> &inputVals);
	void backProp(const vector<float> &targetVals);
	void getResults(vector<float> &resultVals)const;
	float getRecentAverageError(void)const{return m_recentAverageError;}
	
private:
	vector<Layer> m_layers;
	float m_error;
	float m_recentAverageError;
	float m_recentAverageSmoothingFactor;
};

void Net::getResults(vector<float> &resultVals)const{
	
	resultVals.clear();
	
	for(unsigned n = 0;n < m_layers.back().size() - 1;++n){
		
		resultVals.push_back(m_layers.back()[n].getOutputVal());
	}
}

void Net::backProp(const vector<float> &targetVals){
	
	//Calculate overall net error(RMS of output neuron errors)
	Layer &outputLayer = m_layers.back();
	m_error = 0.0;
	
	for(unsigned n = 0; n < outputLayer.size() - 1;++n){
		float delta = targetVals[n] - outputLayer[n].getOutputVal();
		m_error += delta * delta; 
	}
	m_error = m_error*0.5; // get average error squared
	m_error = sqrt(m_error);// RMS
	
	//Implement a recent average measurement
	
	m_recentAverageError = (m_recentAverageError * m_recentAverageSmoothingFactor + m_error)/(m_recentAverageSmoothingFactor + 1.0);
	
	//Calculate output layer gradients
	
	for(unsigned n = 0; n < outputLayer.size() - 1; ++n){
		outputLayer[n].calcOutputGradients(targetVals[n]);
	}
	
	
	//Calculate gradients on hiddenLayer 
	
	for(unsigned layerNum = m_layers.size() - 2;layerNum > 0;--layerNum){
		Layer &hiddenLayer = m_layers[layerNum];
		Layer &nextLayer = m_layers[layerNum + 1];
		
		for(unsigned n = 0; n < hiddenLayer.size(); ++n){
			hiddenLayer[n].calcHiddenGradients(nextLayer);
		}
	}
	
	//For all layers from outputs to first hidden layer,
	//update connection weights
	
	for(unsigned layerNum = m_layers.size() - 1; layerNum > 0; --layerNum){
		Layer &layer = m_layers[layerNum];
		Layer &prevLayer = m_layers[layerNum - 1];
		
		for(unsigned n = 0; n < layer.size() - 1;++n){
			layer[n].updateInputWeights(prevLayer);
		}
	}
	
	
}

void Net::feedForward(const vector<float> &inputVals){
	assert(inputVals.size() == m_layers[0].size() - 1);
	
	// Assign (latch) the input values into the input neurons
	for(unsigned i = 0;i < inputVals.size(); ++i){
		m_layers[0][i].setOutputVal(inputVals[i]);
	}
	
	//forward propagate
	for(unsigned layerNum = 1; layerNum < m_layers.size(); ++layerNum){
		Layer &prevLayer = m_layers[layerNum - 1];
		for(unsigned n = 0;n < m_layers[layerNum].size() - 1; ++n){
			m_layers[layerNum][n].feedForward(prevLayer);
		}
	}
}

Net::Net(const vector<unsigned> &topology){
	unsigned numLayers = topology.size();
	for(unsigned layerNum = 0; layerNum < numLayers; ++layerNum){
		m_layers.push_back(Layer());
		
		unsigned numOutputs = layerNum == topology.size() - 1 ? 0 : topology[layerNum + 1];
		
		//We have a new layer, now fill it with neurons,and
		// add a bias neuron in each layer
		for(unsigned neuronNum = 0; neuronNum <= topology[layerNum]; ++neuronNum){
			m_layers.back().push_back(Neuron(numOutputs, neuronNum));
		}
		
		//Force the bias node's output value to 1.0.It's the last neuron created above
		m_layers.back().back().setOutputVal(1.0);
	}
}

void showVectorVals(string label,vector<float> &v){
	cout << label << " ";
	for(unsigned i = 0;i < v.size(); ++i){
		cout << v[i] << " ";
	}
	cout << endl;
}

int main(){
		
	TrainingData trainData("../train_small.txt");
	
	
	vector<vector<float>> topology;
	vector<float> inputVals,targetVals,resultVals;
	
	trainData.getTopology(topology,targetVals);
	vector<unsigned> tp;
	tp.push_back(784);
	tp.push_back(16);
	tp.push_back(10);
	Net myNet(tp);

	int trainingPass = 0;
	
	
	for(int i=0;i<2;i++){
		cout << "Start Training" << endl;
		++trainingPass;
		cout << endl << "Pass" << trainingPass;
		
		//Get new Input data and feed it forward:
		inputVals = topology[i];
		//showVectorVals(": Inputs:",inputVals);
		cout<<": Inputs:"<<endl;
		for(int j=1;j<28*28+1;j++){
			int n = inputVals[j-1]-'0';
			if(n>0)
				cout<<"*";
			else
				cout<<"0";
			if(j%28==0)
				cout<<endl;
		}
		cout<<"Targets:"<<targetVals[i]<<endl;
		myNet.feedForward(inputVals);
		
		//Collect the net's actual output resules:
		myNet.getResults(resultVals);
		//showVectorVals(": Outputs:",resultVals);
		cout<<"Outputs: ";
		float num = 0;
		int index = 0;
		for (int x=0;x<10;x++){
			if (num<resultVals[x]){
				num=resultVals[x];
				index=x;
			}
		}
		cout<<"Prob "<<index<<" ("<<num<<")";
		cout<<endl;
		//Train the net what the outputs should have been:
		//trainData.getTargetOutputs(targetVals);
		//showVectorVals(": Targets:",targetVals);
		//assert(targetVals.size() == topology[0].back());
		
		vector<float> temp;
        for (int x=0;x<10;x++){
            if (x==targetVals[i]){
                temp.push_back(1);
            }else{
                temp.push_back(0);
            }
        }
		myNet.backProp(temp);
		//Report how well the training is working. average over recent samples:
		cout << "Net recent average error:" << myNet.getRecentAverageError() << endl;
	}


	cout << endl << "Done" << endl;
	
	cout << "testing" << endl;
	vector< vector<float> > test;
	
	ifstream myfile("../test.txt");
	if (myfile.is_open())
	{
		string line;
		while(getline(myfile, line)){
			int x;
			vector<float> X;
			stringstream ss(line);
			float s;
			ss >> s;
			for (int i = 0; i < 28 * 28; i++) {
				
					ss >> x;
					X.push_back(x/255.0);
				}
			test.push_back(X);
			
		}
		myfile.close();
		
	}
	//for(int i = 0 ; i < test.size();i++){}
		
	
	myNet.inference(test[1]);
	myNet.getResults(resultVals);
	
	cout<<"Outputs: ";
		float num = resultVals[0];
		int index = 0;
		for (int x=1;x<10;x++){
			if (num<resultVals[x]){
				num=resultVals[x];
				index=x;
			}
		}
		cout<<"Answer "<<index<<" ("<<num<<")";
		cout<<endl;
}

void Net::inference(const vector<float> &inputVals){
	assert(inputVals.size() == m_layers[0].size() - 1);
	
	// Assign (latch) the input values into the input neurons
	for(unsigned i = 0;i < inputVals.size(); ++i){
		m_layers[0][i].setOutputVal(inputVals[i]);
	}
	
	//forward propagate
	for(unsigned layerNum = 1; layerNum < m_layers.size(); ++layerNum){
		Layer &prevLayer = m_layers[layerNum - 1];
		for(unsigned n = 0;n < m_layers[layerNum].size() - 1; ++n){
			m_layers[layerNum][n].feedForward(prevLayer);
		}
	}
}