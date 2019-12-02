#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>

using namespace std;


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
		//m_layers.back().back().setOutputVal(1.0);
	}
}



int main(){
	vector<unsigned> topology;
	topology.push_back(784);
	topology.push_back(1);
	topology.push_back(10);
	
	Net myNet(topology);
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
	
	for(int counter = 0; counter < X_train.size();counter++){
		myNet.feedForward(X_train[counter]);
		cout << "input:" <<endl;
		for (int i = 1; i < 28 * 28; i++) {
				if(X_train[counter][i] == 0){
					cout << "0";
				}else{
					cout << "*";
				}
				if(i%28 == 0){
				cout << endl;
			}
		}
		cout << "0" << endl;
		vector<float> resultsStorage;
		myNet.getResults(resultsStorage);
		cout << "output:" << endl;
		cout<<"Outputs: ";
			float num = resultsStorage[0];
			int index = 0;
			for (int x=0;x<10;x++){
				if (num<resultsStorage[x]){
					num=resultsStorage[x];
					index=x;
				}
			}
			cout<<"Prob "<<index<<" ("<<num<<")" << endl;
		vector<float> answer;
		for(int i = 0; i < 10; i++){
			if(i == y_train[counter+1]){
				answer.push_back(1);
			}else{
				answer.push_back(-1);
			}
		}
		myNet.backProp(answer);
		cout << "Answer is "<< y_train[counter] << endl;
		
	}
	cout << "Done!"<< endl;
	
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