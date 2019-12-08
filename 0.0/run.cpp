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

using namespace std;

class Layer {
	public:
		Layer(int row,int col);
		void randomWeights();
		vector<vector<float>> weights;
		float bias=1;
		vector<float> output;
		vector<float> error;
};

Layer::Layer(int row, int col){
	vector<float> x(col);
	for(int i = 0; i < row; i++){
		weights.push_back(x);
	}
	randomWeights();
}

void Layer::randomWeights(){
	for(int i=0;i<weights.size();i++){
		for(int j=0;j<weights[i].size();j++){
			weights[i][j] = rand() / float(RAND_MAX);
		}
	}
}

class network {
	public:
		void setInputLayerNum(int num);
		void setOutputLayerNum(int num);
		void addHiddenLayerNum(int num);
		void setLearningRate(float num);
		void initialize();
		void feedFoward(vector<float> temp);
		void sigmoid(vector<float> &output);
		void backPropagation(vector<float> ans);
		//float loss(vector<float> output, vector<float> answer);
		vector<float> calculate(Layer &layer, vector<float> &input);
		void predict();
	private:
		int InputLayerNum;
		int OutputLayerNum;
		float lr;	//LearningRate
		vector<int> NeuronNum;
		vector<float> InputLayer;
		vector<Layer> connection;
		vector<float> OutputLayer;
};

void network::setInputLayerNum(int num){
	InputLayerNum=num;
}

void network::setOutputLayerNum(int num){
	OutputLayerNum=num;
}

void network::setLearningRate(float num){
	lr=num;
}

void network::addHiddenLayerNum(int num){
	NeuronNum.push_back(num);
}
/*float network::loss(vector<float> output, vector<float> answer){
	assert(output.size()==answer.size());
	
	float sum=0;
	for(int i=0;i<output.size();i++){
		sum += pow((output[i]-answer[i]),2);
	}
	
	return sum/2;
}*/

vector<float> network::calculate(Layer &layer, vector<float> &input){
	vector<vector<float>> weight = layer.weights;
	float bias = layer.bias;
	vector<float> output;
	for(int i = 0; i < weight.size() ; i++){
		float sum = 0.0;
		for(int j = 0; j < weight[i].size() ; j++){
			sum += weight[i][j] * input[j];
		}
		//sum += bias[i];
		output.push_back(sum);
	}
	return output;
}

void network::sigmoid(vector<float> &output){
	for(int i=0 ; i < output.size(); i++){
		output[i] = 1 / (1 + exp(-output[i]));
	}
}

void network::initialize(){
	vector<float> temp1(InputLayerNum);
	InputLayer = temp1;
	Layer inputWeight(NeuronNum[0],InputLayerNum);
	connection.push_back(inputWeight);
	for(int i = 0; i < NeuronNum.size()-1 ; i++){
		Layer hiddenWeight(NeuronNum[i],NeuronNum[i+1]);
		connection.push_back(hiddenWeight);
	}
	vector<float> temp2(OutputLayerNum);
	OutputLayer = temp2;
	Layer outputWeight(OutputLayerNum,NeuronNum.back());
	connection.push_back(outputWeight);
}


void network::feedFoward(vector<float> temp){
	assert(temp.size()==InputLayerNum);
	
	InputLayer=temp;
	vector<float> output = InputLayer;
	
	for(int i = 0; i < connection.size() ; i++){
		output = calculate(connection[i],output);
		sigmoid(output);
		connection[i].output = output;
		
	}
		cout<<"output"<<":\n";
		float num=0;
		int n=0;
		for(int i = 0; i < output.size() ; i++){
			cout<< output[i] << "  ";
			if (num<output[i]){
				num=output[i];
				n=i;
			}
		}
		cout<<endl<<"this?:"<<n<<endl;
	
}

void network::backPropagation(vector<float> ans){
	//output layer
	connection.back().error.clear();
	
	for(int i=0;i<connection.back().output.size();i++){
		float predict = connection.back().output[i];
		float error = predict*(1-predict)*(ans[i]-predict);
		connection.back().error.push_back(error);
	}
	cout<<"error:\n";
	for(int i=0;i<connection.back().error.size();i++){
		cout<<connection.back().error[i]<<"  ";
	}
	cout<<endl;
	/*cout<<"outputttt:"<<endl;
	for(int i=0;i<connection[0].output.size();i++){
		cout<<connection[0].output[i]<<"\t";
	}
	cout<<endl;*/
	for(int i=0;i<connection.back().weights.size();i++){
		cout<<"output weight"<<i<<":"<<endl;
		for(int j=0;j<connection.back().weights[i].size();j++){
			connection.back().weights[i][j]+=lr*connection.back().error[i]*connection[0].output[j];
			/*cout<<"lr:"<<lr<<" error:"<<connection.back().error[i]<<" output:"<<connection[0].output[j]<<endl;*/
			cout<<connection.back().weights[i][j]<<"  ";
		}
		cout<<endl;
	}
	//hidden layer
	connection[0].error.clear();
	cout<<"hidden:"<<endl;
	for(int i=0;i<connection[0].output.size();i++){
		float predict = connection[0].output[i];
		for(int j=0;j<connection[1].weights.size();j++){
			float error = predict*(1-predict)*connection[1].error[j]*connection[1].weights[j][i];
			/*cout<<"i:"<<i<<"j:"<<j<<endl;
			cout<<"1: "<<connection[1].error[j]<<" 2:"<<connection[1].weights[j][i]<<" 3:"<<predict<<endl;
			cout<<error<<endl;*/
			connection[0].error.push_back(error);
		}
	}
	cout<<endl;
	//cout<<"hidden weight:"<<endl;
	for(int i=0;i<connection[0].weights.size();i++){
		for(int j=0;j<connection[0].weights[i].size();j++){
			//cout<<"old: "<<connection[0].weights[i][j]<<" error: "<<connection[0].error[i]<<" input: "<<InputLayer[j]<<endl;
			connection[0].weights[i][j]+=lr*connection[0].error[i]*InputLayer[j];
			//cout<<"new: "<<connection[0].weights[i][j]<<endl;
		}
	}
}

void network::predict(){
	cout<<"Output: "<<endl;
	for(int i=0;i<connection.back().output.size();i++){
		cout<<connection.back().output[i]<<"\t";
	}
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
	
	loadFile("train_small.txt",X_train,y_train);
	
	network net;
	net.setInputLayerNum(784);
	net.addHiddenLayerNum(4);
	net.setOutputLayerNum(10);
	net.setLearningRate(10);
	net.initialize();
	for(int i = 0; i < 200;i++){//
		net.feedFoward(X_train[i]);
		//net.predict();
		vector<float> answer;
		cout<<endl<<"Correct Answer: "<<y_train[i]<<endl;
		for(int j=0;j<10;j++){
			if(j==y_train[i]){
				answer.push_back(1);
			}
			else{
				answer.push_back(0);
			}
		}
		net.backPropagation(answer);
	}
	
	return 0;
}