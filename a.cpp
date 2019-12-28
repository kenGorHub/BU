#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <time.h>
#include <windows.h>
#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <chrono>

using namespace std;
using namespace std::chrono;


vector <float> operator-(const vector <float>& m1, const vector <float>& m2){
    
    const unsigned long VECTOR_SIZE = m1.size();
    vector <float> difference (VECTOR_SIZE);
    
    for (unsigned i = 0; i != VECTOR_SIZE; ++i){
        difference[i] = m1[i] - m2[i];
    };
    
    return difference;
}


vector <float> operator*(const vector <float>& m1, const vector <float>& m2){
    
    const unsigned long VECTOR_SIZE = m1.size();
    vector <float> product (VECTOR_SIZE);
    
    for (unsigned i = 0; i != VECTOR_SIZE; ++i){
        product[i] = m1[i] * m2[i];
    };
    
    return product;
}

class neuron{
	public:
		neuron(int length);
		vector<float> weight;
		float output;
		void forward(vector<float> &input);
};

neuron::neuron(int length){
	for(int j=0;j<length;j++){
		weight.push_back(rand() / float(RAND_MAX));
	}
}

void neuron::forward(vector<float> &input){
	float sum=0;
	for(int i=0;i<weight.size();i++){
		sum+=weight[i]*input[i];
	}
	output=sum;
}

class Layer{
	public:
		Layer(int input, int length);
		vector<float> inputs;
		vector<neuron> neurons;
		vector<float> result;
		vector<float> output;
		void forward(vector<float> &input);
		float sigmoid(float sum);
		float bias;
};

Layer::Layer(int input, int length){
	for(int j=0;j<length;j++){
		neuron temp(input);
		neurons.push_back(temp);
	}
	vector<float> tem(length);
	output = tem;
	result = tem;
	bias=1;
}

void Layer::forward(vector<float> &input){
	for(int i=0;i<neurons.size();i++){
		neurons[i].forward(input);
		result[i]=neurons[i].output+bias;
		output[i]=sigmoid(result[i]);
	}
	inputs=input;
}

float Layer::sigmoid(float sum){
	return 1 / (1 + exp(-sum));
}

class network{
	public:
		network(vector<int> layer);
		vector<Layer> Layers;
		float lr; //Learning Rate
		void feedforward(vector<float> &input);
		void backpropagation(vector<float> answer, float &correct);
		vector<float> sigmoid_d(vector<float> result);
		vector<float> dot(vector<neuron> m1,vector<float> m2);
		vector<float> update(Layer &m1,vector<float> m2);
		int max(vector<float> num);
};

network::network(vector<int> layer){
	for(int i=0;i<layer.size()-1;i++){
		Layer l(layer[i],layer[i+1]);
		Layers.push_back(l);
	}
	lr=0.5;
}

vector<float> network::update(Layer &m1,vector<float> m2){
	for(int i=0;i<m1.neurons.size();i++){
		for(int j=0;j<m1.neurons[i].weight.size();j++)
			m1.neurons[i].weight[j]-=lr*((m2*m1.inputs)[i]);
	}
}

vector<float> network::dot(vector<neuron> m1,vector<float> m2){
	vector<float> temp(m2.size(),0);
	//cout<<"dot start"<<endl;
	for(int i=0;i<m1.size();i++)
		for(int j=0;j<m1[i].weight.size();j++)
			temp[i]+=m1[i].weight[j]*m2[j];
	//cout<<"dot end"<<endl;
	return temp;
}

vector<float> network::sigmoid_d(vector<float> result){
	vector<float> temp(result.size());
	//cout<<"sigmoid start"<<endl;
	for(int i=0;i<result.size();i++)
		temp[i]=(1 / (1 + exp(-result[i])))*(1-(1 / (1 + exp(-result[i]))));
	//cout<<"sigmoid end"<<endl;
	return temp;
}

int network::max(vector<float> num){
	float max=*max_element(num.begin(),num.end());
	return find(num.begin(),num.end(),max)-num.begin();
}

void network::feedforward(vector<float> &input){
	vector<float> in=input;
	for(int i=0;i<Layers.size();i++){
		Layers[i].forward(in);
		in=Layers[i].output;
	}
}
void prints(vector<float> t){
	for(int i=0;i<t.size();i++)
		cout<<t[i]<<" ";
	cout<<endl;
}
void network::backpropagation(vector<float> answer, float &correct){
	if(max(Layers.back().output)==max(answer))
		correct++;
	//cout<<max(Layers.back().output)<<":"<<max(answer)<<endl;
	//prints(Layers.back().output);
	
	//calculate error of each layer
	//cout<<"errors start"<<endl;
	vector<float> loss = (Layers.back().output - answer);
	//cout<<"loss done"<<endl;
	vector<vector<float>> errors(2);
	//cout<<loss.size()<<":"<<sigmoid_d(Layers.back().result).size()<<endl;
	errors[0]=loss*sigmoid_d(Layers.back().result);
	//cout<<"errors 0 done"<<endl;
	errors[1]=dot(Layers[1].neurons,errors[0])*sigmoid_d(Layers[0].result);
	//cout<<"errors 1 done"<<endl;
	
	//update weight
	update(Layers.back(),errors[0]);
	update(Layers[0],errors[1]);
	
	/*for(int i=0;i<Layers.back().output.size();i++)
		cout<<Layers.back().output[i]<<" ";
	cout<<endl;*/
}

void loadFile(string filename, vector<vector<float>> &X, vector<float> &y){
	ifstream myfile(filename);
	
	if (myfile.is_open())
	{
		cout << "Loading data ...\n";
		string line;
		int num = 0;
		while (getline(myfile, line))
		{
			float a, b;
			vector<float> temp;
			stringstream ss(line);
			ss >> b;
			y.push_back(b);
			for (int i = 0; i < 28 * 28; i++) {
				ss >> a;
				temp.push_back(a/255.0);
			}
			X.push_back(temp);
			num++;
		}

		myfile.close();
		cout << "Loading data finished.\n"<<num<<" data.\n";
	} 
	else 
		cout << "Unable to open file" << '\n';
}

void print(vector<float> input){
	for (int i = 1; i < 28 * 28+1; i++) {
		if(input[i] == 0){
			cout << "0";
		}else{
			cout << "*";
		}
		if(i%28 == 0){
			cout << endl;
		}
	}
}

int main(int argc, const char * argv[]) {

	vector< vector<float> > X_train;
	vector<float> y_train;
	
	loadFile("test.txt",X_train,y_train);
	
	vector<int> structure;
	structure.push_back(784);
	structure.push_back(64);
	structure.push_back(10);
	
	network net(structure);
	vector<int> temp;
	for(int i=0;i<X_train.size();i++)
		temp.push_back(i);
	int epoch=10;
	int batch_size = 500;
	cout<<"Train on "<<X_train.size()<<" samples, validate on "<<batch_size<<" samples"<<endl;
	for(int i=0;i<epoch;i++){
		float correct=0;
		shuffle(temp.begin(),temp.end(),default_random_engine(i));
		cout<<"Epoch "<<i+1<<"/"<<epoch<<endl;
		auto start = high_resolution_clock::now(); 
		for(int j = 0; j < batch_size;j++){
			net.feedforward(X_train[temp[j]]);
			vector<float> answer(10,0.0);
			answer[y_train[temp[j]]]=1.0;
			net.backpropagation(answer,correct);
		}
		auto stop = high_resolution_clock::now(); 
		auto duration = duration_cast<microseconds>(stop - start); 
		cout<<"Time: "<<duration.count()<<"ms\t Correct: "<<correct/batch_size<<endl;
		//correct=0;
	}
	
	return 0;
}