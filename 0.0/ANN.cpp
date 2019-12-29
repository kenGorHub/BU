#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm> 
#include <chrono> 

using namespace std;
using namespace std::chrono; 

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
		float getOutputVal() const {return m_outputVal;}
		void feedForward(const Layer &prevLayer);
		void calcOutputGradients(float targetVal);
		void calcHiddenGradients(const Layer &nextLayer);
		void updateInputWeights(Layer &prevLayer);
		vector<Connection> m_outputWeights;
	private:
		static float eta;
		static float alpha;
		static float transferFunction(float x);
		static float transferFunctionDerivative(float x);
		static float randomWeight(void){return  rand() / float(RAND_MAX);}
		float sumDOW(const Layer &nextLayer)const;
		float m_outputVal;
		unsigned m_myIndex;
		float m_gradient;
};

float Neuron::eta = 0.1;
float Neuron::alpha = 0.5;

void Neuron::updateInputWeights(Layer &prevLayer){
	for(unsigned n = 0; n < prevLayer.size(); ++n){
		Neuron &neuron = prevLayer[n];
		float oldDeltaWeight = neuron.m_outputWeights[m_myIndex].deltaWeight;
		float newDeltaWeight = eta * neuron.getOutputVal() * m_gradient ;//alpha * oldDeltaWeight;+ oldDeltaWeight
		
		
		neuron.m_outputWeights[m_myIndex].deltaWeight = newDeltaWeight;
		neuron.m_outputWeights[m_myIndex].weight = newDeltaWeight;
		
	}
}

float Neuron::sumDOW(const Layer &nextLayer)const {
	float sum = 0.0;
	
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

float Neuron::transferFunction(float x){
	return 1 / (1 + exp(-x));
}

float Neuron::transferFunctionDerivative(float x){
	return (1 / (1 + exp(-x)))*(1-(1 / (1 + exp(-x))));
	//return (x)*(1-x);
}

void Neuron::feedForward(const Layer &prevLayer){
	float sum = 0.0;
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
	void feedForward(const vector<float> &inputVals);
	void backProp(const vector<float> &targetVals);
	void getResults(vector<float> &resultVals)const;
	void importfile(string filename);
	void exportfile(string filename);
	void inference(const vector<float> &inputVals);
	
private:
	vector<Layer> m_layers;
	float m_error;
	float m_recentAverageError;
	float m_recentAverageSmoothingFactor;
};

void Net::exportfile(string filename){
	ofstream outFile;
    outFile.open(filename);

    for (int i = 0; i < m_layers.size(); i++){
		for(int j = 0; j < m_layers[i].size(); j++){
			for(int k = 0; k < m_layers[i][j].m_outputWeights.size(); k++){		
				outFile << m_layers[i][j].m_outputWeights[k].weight << endl;
			}
		}
    } 

    outFile.close();
}

void Net::importfile(string filename){
	ifstream inFile;
    inFile.open(filename);

	for (int i = 0; i < m_layers.size(); i++){
		for(int j = 0; j < m_layers[i].size(); j++){
			for(int k = 0; k < m_layers[i][j].m_outputWeights.size(); k++){		
				inFile >> m_layers[i][j].m_outputWeights[k].weight;
			}
		}
    }
}

void Net::getResults(vector<float> &resultVals)const{
	resultVals.clear();
	
	for(unsigned n = 0;n < m_layers.back().size() - 1;++n){
		resultVals.push_back(m_layers.back()[n].getOutputVal());
	}
}

void Net::backProp(const vector<float> &targetVals){
	
	Layer &outputLayer = m_layers.back();
	m_error = 0.0;
	for(int n=0;n<10;n++){
		for(unsigned n = 0; n < outputLayer.size() - 1;++n){
			float delta = targetVals[n] - outputLayer[n].getOutputVal();
			m_error += delta * delta;
		}
		m_error /= outputLayer.size() - 1;
		m_error = sqrt(m_error);
	}
	m_recentAverageError = (m_recentAverageError  + m_error);//m_recentAverageSmoothingFactor
	//cout<<"Total loss value:"<<m_recentAverageError<<endl;
	for(unsigned n = 0; n < outputLayer.size() - 1; ++n){
		outputLayer[n].calcOutputGradients(targetVals[n]);
	}
	
	for(unsigned layerNum = m_layers.size() - 2;layerNum > 0;--layerNum){
		Layer &hiddenLayer = m_layers[layerNum];
		Layer &nextLayer = m_layers[layerNum + 1];
		
		for(unsigned n = 0; n < hiddenLayer.size(); ++n){
			hiddenLayer[n].calcHiddenGradients(nextLayer);
		}
	}
	
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
	
	for(unsigned i = 0;i < inputVals.size(); ++i){
		m_layers[0][i].setOutputVal(inputVals[i]);
	}
	
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
		
		for(unsigned neuronNum = 0; neuronNum <= topology[layerNum]; ++neuronNum){
			m_layers.back().push_back(Neuron(numOutputs, neuronNum));
			//cout << "Made a Neuron!" << endl;
		}
		
		m_layers.back().back().setOutputVal(1.0);
	}
}

void Net::inference(const vector<float> &inputVals){
	feedForward(inputVals);
	
	vector<float> n;
	getResults(n);
	cout<<"output:"<<endl;
	float max=0;
	int num=0;
	for(int i=0;i<n.size();i++){
		cout<<n[i]<<"  ";
		if(max<n[i]){
			max=n[i];
			num=i;
		}
	}
	cout<<endl<<"this? "<<num<<"("<<max<<")"<<endl;
}

int main(){
	
	vector<unsigned> topology;
	topology.push_back(784);
	topology.push_back(4);
	topology.push_back(10);
	Net myNet(topology);
	
	ifstream myfile("train_small.txt");
	vector< vector<float> > X_train;
	vector<float> y_train;
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
	int epoch=4;
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	vector<int> index;
	for(int i=0;i<X_train.size();i++)index.push_back(i);

	shuffle(index.begin(),index.end(),default_random_engine(seed));

	vector<vector<int>> temp;
	for(int j=0;j<epoch;j++){
		vector<int> a;
		for(int i=0+j*index.size()/epoch;i<(j+1)*index.size()/epoch;i++)
			a.push_back(index[i]);
		temp.push_back(a);
	}
	vector<float> inputVals;
	vector<int> indVal;
	
	for(int i=0;i<epoch;i++){
		
		indVal = temp[i];
		
		cout<<"Epoch "<<i+1<<"/"<<epoch<<endl;
		auto start = high_resolution_clock::now();
		for(int j=0;j<indVal.size();j++){
			inputVals=X_train[indVal[j]];
			myNet.feedForward(inputVals);
			
			vector<float> targetVals;
			vector<float> answer(10,-1);
			answer[y_train[indVal[j]]]=1;
			myNet.backProp(answer);
			
		}
		auto stop = high_resolution_clock::now(); 
	
		auto duration = duration_cast<milliseconds>(stop - start); 
		
		cout << "CPU Running Time : "<< duration.count() << " ms" << endl; 
	}
	
	
  
    
	
}