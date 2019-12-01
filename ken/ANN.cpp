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
		void getTopology(vector<vector<unsigned>> &topology);
		//Returns the number of input values read from the file:
		unsigned getNextInputs(vector<unsigned> &inputVals);
		unsigned getTargetOutputs(vector<unsigned> &targetOutputVals);
	
	private:
		ifstream m_trainingDataFile;
};

void TrainingData::getTopology(vector<vector<unsigned>> &topology){
	string line;
	
	
	while(getline(m_trainingDataFile,line)){
		stringstream ss(line);
		string s;
		ss >> s;
		vector<unsigned> temp;
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

unsigned TrainingData::getNextInputs(vector<unsigned> &inputVals){
	inputVals.clear();
	
	string line;
	getline(m_trainingDataFile,line);
	stringstream ss(line);
		string s;
		ss >> s;
		while (!ss.eof()){
			unsigned oneValue;
			ss >> oneValue;
			inputVals.push_back(oneValue);
		}
	return inputVals.size();
}

unsigned TrainingData::getTargetOutputs(vector<unsigned> &targetOutputVals){
	targetOutputVals.clear();
	
	string line;
	while(getline(m_trainingDataFile,line)){
		stringstream ss(line);
		
			unsigned oneValue;
			while(ss>>oneValue){
				targetOutputVals.push_back(oneValue);
			}
			
	}
	
	return targetOutputVals.size();
}

struct Connection{
	double weight;
	double deltaWeight;
};

class Neuron;

typedef vector<Neuron> Layer;

//*********************class Neuron*******************************
class Neuron {
	public:
		Neuron(unsigned numOutputs,unsigned myIndex);
		void setOutputVal(double val){m_outputVal = val;}
		double getOutputVal(void) const {return m_outputVal;}
		void feedForward(const Layer &prevLayer);
		void calcOutputGradients(double targetVal);
		void calcHiddenGradients(const Layer &nextLayer);
		void updateInputWeights(Layer &prevLayer);
	private:
		static double eta;//Individual input, magnified by the gradient and train rate:
		static double alpha;//Also add momentum = a fraction of the previous delta weight
		static double transferFunction(double x);
		static double transferFunctionDerivative(double x);
		static double randomWeight(void){return rand() / double(RAND_MAX);}
		double sumDOW(const Layer &nextLayer)const;
		double m_outputVal;
		vector<Connection> m_outputWeights;
		unsigned m_myIndex;
		double m_gradient;
};

double Neuron::eta = 0.1;//overall net learning rate,[0,0..1.0]
double Neuron::alpha = 0.5;//momentum,multiplier of last deltaweight,[0,0..n]

void Neuron::updateInputWeights(Layer &prevLayer){
	
	//The weight to be updated are in the Connection container
	//in the neurons in the preceding layer
	for(unsigned n = 0; n < prevLayer.size(); ++n){
		Neuron &neuron = prevLayer[n];
		double oldDeltaWeight = neuron.m_outputWeights[m_myIndex].deltaWeight;
		double newDeltaWeight = eta * neuron.getOutputVal() * m_gradient + alpha * oldDeltaWeight;
		
		
		neuron.m_outputWeights[m_myIndex].deltaWeight = newDeltaWeight;
		neuron.m_outputWeights[m_myIndex].weight = newDeltaWeight;
		
	}
}

double Neuron::sumDOW(const Layer &nextLayer)const {
	double sum = 0.0;
	
	//Sum our contributions of the errrors at the nodes we feed
	
	for(unsigned n = 0;n < nextLayer.size() - 1; ++n){
		sum += m_outputWeights[n].weight * nextLayer[n].m_gradient;
	}
	
	return sum;
}

void Neuron::calcHiddenGradients(const Layer &nextLayer){
	double dow = sumDOW(nextLayer);
	m_gradient = dow * Neuron::transferFunctionDerivative(m_outputVal);
}

void Neuron::calcOutputGradients(double targetVal){
	double delta = targetVal - m_outputVal;
	m_gradient = delta * Neuron::transferFunctionDerivative(m_outputVal);
}

double Neuron::transferFunction(double x){//active function
	//tanh - output range[-1.0..1.0]
	//return tanh(x);
	return 1 / (1 + exp(-x));
}

double Neuron::transferFunctionDerivative(double x){
	//tanh derivative
	return 1.0 - ( 1 / (1 + exp(-x)) );
}

void Neuron::feedForward(const Layer &prevLayer){
	double sum = 0.0;
	//Sum the previous layer's outputs (which are out inputs)
	//Include the bias node from the previous layer.
	for(unsigned n = 0;n < prevLayer.size();++n){
		sum += prevLayer[n].getOutputVal() *
				prevLayer[n].m_outputWeights[m_myIndex].weight;
	}
	
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
	Net(const vector<unsigned> &topology);
	void feedForward(const vector<unsigned> &inputVals);
	void backProp(const vector<unsigned> &targetVals);
	void getResults(vector<unsigned> &resultVals)const;
	double getRecentAverageError(void)const{return m_recentAverageError;}
	
private:
	vector<Layer> m_layers;
	double m_error;
	double m_recentAverageError;
	double m_recentAverageSmoothingFactor;
};

void Net::getResults(vector<unsigned> &resultVals)const{
	resultVals.clear();
	
	for(unsigned n = 0;n < m_layers.back().size() - 1;++n){
		resultVals.push_back(m_layers.back()[n].getOutputVal());
	}
}

void Net::backProp(const vector<unsigned> &targetVals){
	
	//Calculate overall net error(RMS of output neuron errors)
	Layer &outputLayer = m_layers.back();
	m_error = 0.0;
	
	for(unsigned n = 0; n < outputLayer.size() - 1;++n){
		double delta = targetVals[n] - outputLayer[n].getOutputVal();
		m_error += delta * delta; 
	}
	m_error = m_error*0.5; // get average error squared
	//m_error = sqrt(m_error);// RMS
	
	//Implement a recent average measurement
	
	//m_recentAverageError = (m_recentAverageError * m_recentAverageSmoothingFactor + m_error)/(m_recentAverageSmoothingFactor + 1.0);
	
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

void Net::feedForward(const vector<unsigned> &inputVals){
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
			cout << "Made a Neuron!" << endl;
		}
		
		//Force the bias node's output value to 1.0.It's the last neuron created above
		m_layers.back().back().setOutputVal(1.0);
	}
}

void showVectorVals(string label,vector<unsigned> &v){
	cout << label << " ";
	for(unsigned i = 0;i < v.size(); ++i){
		cout << v[i] << " ";
	}
	cout << endl;
}

int main(){
		
	TrainingData trainData("../train_small.txt");
	
	
	vector<vector<unsigned>> topology;
	trainData.getTopology(topology);
	vector<unsigned> x(16);
	Net myNet(x);

	vector<unsigned> inputVals,targetVals,resultVals;
	int trainingPass = 0;
	
	
	for(int i=0;i<topology.size();i++){
		cout << "Start Training" << endl;
		++trainingPass;
		cout << endl << "Pass" << trainingPass;
		
		//Get new Input data and feed it forward:
		inputVals = topology[i];
		showVectorVals(": Inputs:",inputVals);
		myNet.feedForward(inputVals);
		
		//Collect the net's actual output resules:
		myNet.getResults(resultVals);
		showVectorVals(": Outputs:",resultVals);
		
		//Train the net what the outputs should have been:
		trainData.getTargetOutputs(targetVals);
		showVectorVals(": Targets:",targetVals);
		assert(targetVals.size() == topology[0].back());
		
		myNet.backProp(targetVals);
		
		//Report how well the training is working. average over recent samples:
		cout << "Net recent average error:" << myNet.getRecentAverageError() << endl;
	}


	cout << endl << "Done" << endl;

	
}