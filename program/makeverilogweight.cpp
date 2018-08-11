#include <iostream>
#include <fstream>
#include <stdlib.h>
using namespace std;

bool check(string s1,string s2){
	if(s1==s2)
		return 1;
	else
		return 0;
}

int main(int argc,char** argv){
	if(argc<3){
		cerr<<"need program mlp.final nnpara.coe"<<endl;
		exit(0);
	}
	int innumber=0;
	int outnumner=0;
	int** biasstore;
	int prenumber=0;
	int nodenumber=0;
	int layercount=0;
	biasstore=new int*[10];
	ifstream mlpfile(argv[1]);
	ofstream nnparafile(argv[2]);
	if(!mlpfile){
		cerr<<"can't open mlp file in "<<argv[1]<<endl;
		exit(0);
	}
	if(!nnparafile){
		cerr<<"create file nnpara failed in "<<argv[2]<<endl;
		exit(0);
	}
	nnparafile<<"MEMORY_INITIALIZATION_RADIX=10;"<<endl;
	nnparafile<<"MEMORY_INITIALIZATION_VECTOR="<<endl;
	string temp;
	mlpfile>>temp;
	if(!check(temp,"<Nnet>")){
		cerr<<"need <Nnet> in mlp file"<<endl;
		exit(0);
	}
	cout<<"make the file"<<endl;
	mlpfile>>temp;
	mlpfile>>temp;
	int inputnumber=atoi(temp.data());
	mlpfile>>temp;
	mlpfile>>temp;
	mlpfile>>temp;
	prenumber=atoi(temp.data());
	nodenumber=inputnumber/prenumber;
	biasstore[layercount]=new int[nodenumber+1];
	biasstore[layercount][0]=nodenumber;
	int biascount=1;
	cout<<"first layer"<<endl;
	for(int i=0;i<nodenumber;i++){
		while(!check(temp,"<affinetransform>"))
			mlpfile>>temp;
		while(!check(temp,"]"))
			mlpfile>>temp;
		mlpfile>>temp;
		mlpfile>>temp;
		mlpfile>>temp;
		if(!check(temp,"[")){
			cerr<<"need [ in affinetransform layer 0 node "<<i<<endl;
			exit(0);
		}
		for(int j=0;j<prenumber;j++){
			mlpfile>>temp;
			float temint;
			temint=atof(temp.data());
			temint*=512;
			nnparafile<<int(temint)<<", ";
		}
		nnparafile<<endl;
		mlpfile>>temp;
		if(!check(temp,"]")){
			cerr<<"need ] in affinetransform layer 0 node "<<i<<endl;
			exit(0);
		}
		mlpfile>>temp;
		if(!check(temp,"[")){
			cerr<<"need [ before bias in affinetransform layer 0 node "<<i<<endl;
			exit(0);
		}
		mlpfile>>temp;
		float temint=atof(temp.data());
		temint*=1024;
		biasstore[layercount][biascount++]=int(temint);
	}
	layercount++;
	cout<<"second layer"<<endl;
	while(!check(temp,"<affinetransform>"))
			mlpfile>>temp;
	while(!check(temp,"]"))
		mlpfile>>temp;
	mlpfile>>temp;
	nodenumber=atoi(temp.data());
	mlpfile>>temp;
	prenumber=atoi(temp.data());
	mlpfile>>temp;
	biasstore[layercount]=new int[nodenumber+1];
	biasstore[layercount][0]=nodenumber;
	if(!check(temp,"[")){
		cerr<<"need [ in affinetransform layer "<<layercount<<endl;
		exit(0);
	}
	for(int i=0;i<nodenumber;i++){
		for(int j=0;j<prenumber;j++){
			mlpfile>>temp;
			float floattemp=atof(temp.data());
			floattemp*=512;
			nnparafile<<int(floattemp)<<", ";
		}
		nnparafile<<endl;
	}
	mlpfile>>temp;
	if(!check(temp,"]")){
		cerr<<"need [ in affinetransform layer "<<layercount<<endl;
		exit(0);
	}
	mlpfile>>temp;
	if(!check(temp,"[")){
		cerr<<"need ] before bias in affinetransform layer "<<layercount<<endl;
		exit(0);
	}
	for(int j=0;j<nodenumber;j++){
			mlpfile>>temp;
			float temint;
			temint=atof(temp.data());
			temint*=512;
			biasstore[layercount][j+1]=int(temint);
	}
	mlpfile>>temp;
	if(!check(temp,"]")){
		cerr<<"need ] after bias in affinetransform layer "<<layercount<<endl;
		exit(0);
	}
	mlpfile>>temp;


	while(!check(temp,"<softmax>")){
		layercount++;
		cout<<layercount<<" layer"<<endl;
		if(!check(temp,"<sigmoid>")){
			cerr<<"need <sigmoid> in layer "<<layercount<<endl;
			exit(0);
		}
		mlpfile>>temp;
		mlpfile>>temp;
		mlpfile>>temp;
		if(!check(temp,"<affinetransform>")){
			cerr<<"need <affinetransform> in layer "<<layercount<<endl;
			exit(0);
		}
		mlpfile>>temp;
		nodenumber=atoi(temp.data());
		mlpfile>>temp;
		prenumber=atoi(temp.data());
		mlpfile>>temp;
		if(!check(temp,"[")){
			cerr<<"need [ in affinetransform layer"<<layercount<<endl;
			exit(0);
		}
		for(int i=0;i<nodenumber;i++){
			for(int j=0;j<prenumber;j++){
				mlpfile>>temp;
				float temint;
				temint=atof(temp.data());
				temint*=512;
				nnparafile<<int(temint)<<", ";
			}
			nnparafile<<endl;
		}
		mlpfile>>temp;
		if(!check(temp,"]")){
			cerr<<"need ] in affinetransform layer "<<layercount<<endl;
			exit(0);
		}
		mlpfile>>temp;
		if(!check(temp,"[")){
			cerr<<"need [ before bias in affinetransform layer "<<layercount<<endl;
			exit(0);
		}
		biasstore[layercount]=new int[nodenumber+1];
		biasstore[layercount][0]=nodenumber;
		for(int j=0;j<nodenumber;j++){
			mlpfile>>temp;
			float temint;
			temint=atof(temp.data());
			temint*=512;
			biasstore[layercount][j+1]=int(temint);
		}
		mlpfile>>temp;
		if(!check(temp,"]")){
			cerr<<"need ] after affinetransform layer "<<layercount<<endl;
			exit(0);
		}
		mlpfile>>temp;
}
for(int i=0;i<layercount+1;i++){
	cout<<"in layer"<<i<<" do "<<biasstore[i][0]<<endl;
	for(int j=1;j<biasstore[i][0]+1;j++){
		nnparafile<<biasstore[i][j];
		cout<<biasstore[i][j]<<" ";
		if(i==layercount && j==biasstore[i][0])
			nnparafile<<";";
		else
			nnparafile<<", ";
	}
	cout<<endl;
	cout<<endl;
	nnparafile<<endl;
}






	mlpfile.close();
	nnparafile.close();
	return 0;
}