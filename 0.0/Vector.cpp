#include <iostream>
#include <string>
#include<string.h>
#include<stdlib.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;
template<class T>
class Vector{
	public:
		bool row=true;
		vector<T> arr;
		Vector(){};
		Vector(bool r,vector<T> a):row(r),arr(a){}; 
		int lengths(){return arr.size();}
		void set(int index,T value){
			arr[index]=value;
		}
		void add(T value){
			arr.push_back(value);
		}
		T get(int index){
			return arr[index];
		}
		double mean(){ 
		    double mean=0.0;
		    for(int i=0;i<lengths();i++){
				mean+=arr[i];
	        }
	        mean/=lengths();
	        return mean;
		};
		void lnorm(){//L1 norm
		    T sum=0;
		    for(int i=0;i<lengths();i++){
				sum+=abs(arr[i]);
			}
			cout<<"L1 norm of vector is "<<sum<<endl;
		};
		void enorm(){//Euclidean norm
		    T sum=0;
			for(int i=0;i<lengths();i++){
				sum+=pow(arr[i],2);
			}
			cout<<"Euclidean norm of vector is "<<sqrt(sum)<<endl;
		};
		void scalar(T a){// Scalar-Vector multiplication
		    for(int i=0;i<lengths();i++){
					arr[i]*=a;
			}
			cout<<"Vector is multipled by "<<a<<endl;
		};
		void display(){
    		cout<<"Vector:( ";
    	    for(int i=0;i<lengths()-1;i++){
    			 cout<<arr[i];
    			 cout<<", ";
    		}
    		cout<<arr[lengths()-1]<<" )"<<endl;
		};
};
using namespace std;
template<class T>
double dot(Vector<T> a,Vector<T> b){//Dot product of two vectors
	if (a.lengths()==b.lengths()){
		double sum=0;
        for ( int i = 0; i < a.lengths(); i++ ){
			sum+=a.arr[i]*b.arr[i];
    	}
		return sum;
	}
	else{
		cout<<"Wrong length"<<endl;
		cout<<a.lengths()<<","<<b.lengths()<<endl;
		return 0.0;
	}
}