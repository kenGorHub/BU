#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
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
	
	for(unsigned n = 0; n < outputLayer.size() - 1;++n){
		float delta = targetVals[n] - outputLayer[n].getOutputVal();
		m_error += delta * delta;
	}
	//m_error /= outputLayer.size() - 1;
	m_error = sqrt(m_error);
	
	m_recentAverageError = (m_recentAverageError  + m_error);//m_recentAverageSmoothingFactor
	cout<<"Error:"<<m_recentAverageError<<endl;
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
		
	auto start = high_resolution_clock::now();
	
	vector<unsigned> topology;
	topology.push_back(784);
	topology.push_back(4);
	topology.push_back(10);
	Net myNet(topology);
	
	ifstream myfile("train_small.txt");
	string line;
	int count=1;
	while(getline(myfile, line)){
		cout<<"Train case"<<count<<":"<<endl;
		count++;
		int x,y;
		stringstream ss(line);
		ss>>y;
		vector<float> inputVals;
		for (int i = 0; i < 28 * 28; i++) {
			ss >> x;
			inputVals.push_back(x/255.0);
		}
		myNet.feedForward(inputVals);
		/*cout<<"Input:"<<endl;
		for (int i = 1; i < 28 * 28+1; i++) {
				if(inputVals[i] == 0){
					cout << "0";
				}else{
					cout << "*";
				}
				if(i%28 == 0){
				cout << endl;
			}
		}
		cout<<endl;*/
		
		vector<float> targetVals;
		for(int i = 0; i < 10; i++){
			if(i == y){
				targetVals.push_back(1);
			}else{
				targetVals.push_back(-1);
			}
		}
		myNet.backProp(targetVals);
		
		vector<float> resultVals;
		myNet.getResults(resultVals);
		float max=0;
		int n=0;
		cout<<"output:"<<endl;
		for(int i=0;i<resultVals.size();i++){
			cout<<resultVals[i]<<"  ";
			if(max<resultVals[i]){
				max=resultVals[i];
				n=i;
			}
		}
		cout<<endl<<"this? "<<n<<"("<<max<<")"<<endl;
		cout<<"correct answer:"<<y<<endl<<endl;
	}
	
	auto stop = high_resolution_clock::now(); 
	
	auto duration = duration_cast<microseconds>(stop - start); 
  
    cout << "Training Time taken : "<< duration.count() << " microseconds" << endl; 
		 
	myNet.exportfile("a.txt");
	/*ifstream myfile2("test.txt");
	count=1;
	while(getline(myfile2, line)&&count<3){
		int x,y;
		count++;
		stringstream ss(line);
		ss>>y;
		vector<float> inputVals;
		for (int i = 0; i < 28 * 28; i++) {
			ss >> x;
			inputVals.push_back(x/255.0);
		}
		cout<<"correct:"<<y<<endl;
		myNet.inference(inputVals);
	}*/
}