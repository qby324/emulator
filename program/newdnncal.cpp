#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <sstream>
#include <string.h>
using namespace std;
#define UNDEFINE -87654
#define BIT20 2097151
#define BIT14fu 0x3fff
#define BIT14zhe 0x1fff
#define BIT16fu 0xffff
#define BIT16zhe 0x7fff
#define BIT28 536870911
#define BIT12 4094
#define MULSAV 0x1fffe0000	


/*
class Bitnumber{
	int digit;
	int* bitnumber;
	int ocxnumber;
	bool signumber;		//false是正数 true是负数
	Bitnumber(){
		digit=-UNDEFINE;
		bitnumber=NULL;
		ocxnumber=UNDEFINE;
		signumber=false;
	}
	Bitnumber(int  digit){
		this->digit=digit;
		this->bitnumber=new int[digit];
		ocxnumber=0;
	}
	Bitnumber(int digit,int number�E�bool siged){
		if(siged)
			this->digit=digit-1;
		else
			this->digit=digit;
		this->bitnumber=new int[digit];
		this->ocxnumber=number;
		int count=0;
		do{
			int t;
			t=ocxnumber%2;
			bitnumber[count++]=t;
			ocxnumber=(ocxnumber-t)/2;
		}while(count<this->digit);
		if(siged)
			if(number<0)
				signumber=true;
			else
				signumber=false;

	}
	~Bitnumber(){delete[] bitnumber;}
	void setocxnumber(int number){
		int count=0;
		this->ocxnumber=number;
		do{
			int t;
			t=ocxnumber%2;
			bitnumber[count++]=t;
			ocxnumber=(ocxnumber-t)/2;
		}while(count<digit);
	}
	void setdigit(int digit,int number){
		this->bitnumber[digit]=number;
		int temp=0;
		for(int i=0;i<digit;i++)
			temp+=(bitnumber[digit]<<digit);
		ocxnumber=temp;
	}
}
*/
int changebit(int orgnumber,int beginbit, int endbit,bool issymbol=true){
	int bittemp[50];
	for(int i=0;i<50;i++)
		bittemp[i]=0;
	int count=0;
	int flag=false;
	if(orgnumber<0){
		flag=true;
		orgnumber*=-1;
	}
	do{
		int t;
		t=orgnumber%2;
		bittemp[count++]=t;
		orgnumber=(orgnumber-t)/2;
	}while(orgnumber!=0);
	if(flag){
		for(int i=0;i<count;i++)
			bittemp[i]=bittemp[i]?0:1;
		int porflag=1;
		for(int i=0;i<count;i++){
			if(bittemp[i] && porflag)
				bittemp[i]=0;
			else if(porflag==1){
				bittemp[i]=1;
				porflag=0;
				break;
			}
		}
		for(int i=count;i<40;i++)
			bittemp[i]=1;
	}
	else
		for(int i=count;i<40;i++)
			bittemp[i]=0;
	int ocxtemp=0;
	count=0;
	if(bittemp[endbit]==1 ){
		int* newbit=new int[endbit-beginbit+2];
		for(int i=0;i<endbit-beginbit+2;i++)
			newbit[i]=0;
		for(int i=0;i<endbit-beginbit+1;i++)
			newbit[i]=bittemp[i+beginbit]?0:1;
		int porflag=1;
		for(int i=0;i<endbit-beginbit+2;i++){
			if(newbit[i] && porflag)
				newbit[i]=0;
			else if(porflag==1){
				newbit[i]=1;
				porflag=0;
				break;
			}
		}
		int i=0;
		do{
			ocxtemp+=newbit[i++]<<count++;
		}while(i<=endbit-beginbit+1);
		ocxtemp*=-1;
	}
	else{
		do{
			ocxtemp+=bittemp[beginbit++]<<count++;
		}while(beginbit<=endbit);
	}
	return ocxtemp;
}
/*
unsigned int changbitforsigmoid(unsigned orgnumber,int beginbit,int endbit){
	unsigned int bittemp[32];
	for(int i=0;i<32;i++)
		bittemp[i]=0;
	int count=0;
	int flag=false;
	do{
		int t;
		t=orgnumber%2;
		bittemp[count++]=t;
		orgnumber=(orgnumber-t)/2;
	}while(orgnumber!=0);
	int ocxtemp=0;
	count=0;
	do{
		ocxtemp+=bittemp[beginbit++]<<count++;
	}while(beginbit<=endbit);
	return ocxtemp;
}
*/
int sigmoid(int number){
	int temp;
	long int numberlong=number;
	temp=((numberlong*1024)/(abs(numberlong)+1024)+1024)/2;
	return temp;
}
class Node{
public:
	int* weight;
	int* input;
	int bias;
	int weightnumber;
	int output;
	bool flatweight;
	bool flatintput;
	bool flatbias;
	bool flatcal;
	Node(){
		weight=NULL;
		input=NULL;
		bias=output=weightnumber=UNDEFINE;
		flatweight=flatintput=flatbias=flatcal=0;
	}
	~Node(){
		delete[] weight;
		delete[] input;
	}
	void move(int* newinput,int beginnumber,int nodehash,int irtimes){
		int* moveinput=new int[weightnumber];
		for(int i=0;i<100;i++)
			moveinput[i]=this->input[i+1];
		if(beginnumber<irtimes)
			moveinput[100]=newinput[beginnumber*12+nodehash];
		else
			moveinput[100]=0;
		for(int i=0;i<101;i++)
			this->input[i]=moveinput[i];
		flatcal=0;
		flatintput=1;
/*
		int* moveinput=new int[weightnumber];
		for(int i=0;i<weightnumber-12;i++)
			moveinput[i]=this->input[i+12];
		int count=0;
		for(int i=weightnumber-12;i<weightnumber;i++)
			moveinput[i]=newinput[beginnumber*12+count++];
		for(int i=0;i<weightnumber;i++)
			input[i]=moveinput[i];
		flatcal=0;
		flatintput=1;*/
	}
	void init(int weightnumber){
		this->weight=new int[weightnumber];
		this->input=new int[weightnumber];
		for(int i=0;i<weightnumber;i++)
			this->input[i]=0;
		this->weightnumber=weightnumber;
	}
	void setbias(int bias){
		this->bias=changebit(bias,0,13);
		this->flatbias=1;
	}
	void setweight(const char* file){
		ifstream weightfile(file);
		if(!weightfile){
			cerr<<"file "<<file<<"not exist"<<endl;
			exit(0);
		}
		for(int i=0;i<this->weightnumber;i++){
			int temp;
			string stemp;
			stringstream sstemp;
			weightfile>>stemp;
			sstemp<<stemp;
			sstemp>>temp;
			this->weight[i]=changebit(temp,0,13);
		}
		this->flatweight=1;
		this->flatcal=0;
		weightfile.close();
	}
	void setinput(char* file){
		ifstream inputfile(file);
		if(!inputfile){
			cerr<<"file "<<file<<"not exist"<<endl;
			exit(0);
		}
		for(int i=0;i<this->weightnumber;i++){
			string stemp;
			int temp;
			stringstream sstemp;
			inputfile>>stemp;
			sstemp<<stemp;
			sstemp>>temp;
			sstemp.clear();
			this->input[i]=changebit(temp,0,15);
		}
		inputfile.close();
		this->flatintput=1;
		this->flatcal=0;
	}
	void setinput(int* input){
		for(int i=0;i<this->weightnumber;i++){
			this->input[i]=changebit(input[i],0,15);
		//	cerr<<"set input "<<this->input[i]<<endl;
		}
		this->flatintput=1;
		this->flatcal=0;
	}
	int calculationwithoutsigmoid(){
		if(!(flatbias & flatintput & flatweight & !flatcal))
			return -1;
		int temp=0;
		for(int i=0;i<weightnumber;i++){
		//	cerr<<"calculate "<<i<<" weight="<<weight[i]<<"   input="<<input[i]<<endl;
			temp+=(this->weight[i]*this->input[i])/1024;
		//	cerr<<"temp="<<temp<<endl;
		//	cerr<<"weight["<<i<<"]="<<weight[i]<<endl;
		//	cerr<<"input["<<i<<"]="<<input[i]<<endl;
		}
	//	temp+=59;
		temp=changebit(temp,0,26);
		temp+=this->bias;
	//	cerr<<"temp="<<temp<<endl;
	//	cerr<<endl;
	//	cerr<<endl;
//		this->output=(temp/(temp+1.0))*0.5;
	//	output=sigmoid(temp); 
	//	cerr<<"output="<<output<<endl;
	//	cerr<<endl;
	//	cerr<<endl;
		flatcal=1;
	//	cerr<<"calculate "<<this->output<<endl;
		return temp;
	}
	int calculation(){
		if(!(flatbias & flatintput & flatweight & !flatcal))
			return -1;
		int temp=0;
		for(int i=0;i<weightnumber;i++){
		//	cerr<<"calculate "<<i<<" weight="<<weight[i]<<"   input="<<input[i]<<endl;
			temp+=(this->weight[i]*this->input[i])/1024;
		//	cerr<<"temp="<<temp<<endl;
		//	cerr<<"weight["<<i<<"]="<<weight[i]<<endl;
		//	cerr<<"input["<<i<<"]="<<input[i]<<endl;
		}
	//	temp+=59;
		temp=changebit(temp,0,26);
		temp+=this->bias;
	//	cerr<<"temp="<<temp<<endl;
	//	cerr<<endl;
	//	cerr<<endl;
//		this->output=(temp/(temp+1.0))*0.5;
		output=sigmoid(temp); 
	//	cerr<<"output="<<output<<endl;
	//	cerr<<endl;
	//	cerr<<endl;
		flatcal=1;
	//	cerr<<"calculate "<<this->output<<endl;
		return output;
	}
};

class Layer{
public:
	Node* node;
	int nodenumber;
	int prenodenumber;
	int* output;
	Layer(){
		this->node=NULL;
		this->nodenumber=this->prenodenumber=UNDEFINE;
		this->output=NULL;
	}
	~Layer(){
		delete[] node;
		delete[] output;
	}
	void init(int prenodenumber,int nodenumber){
		this->nodenumber=nodenumber;
		this->prenodenumber=prenodenumber;
		this->node=new Node[nodenumber];
		for(int i=0;i<nodenumber;i++)
			this->node[i].init(prenodenumber);
		this->output=new int[nodenumber];
	}
	void setbias(int layernumber){
		string filename;
		stringstream sstemp;
		sstemp<<"./weightfile/l"<<layernumber<<"bias.dat";
		sstemp>>filename;
		sstemp.clear();
		ifstream biasfile(filename.data());
		for(int i=0;i<nodenumber;i++){
			int temp;
			string stemp;
			stringstream sstemp;
			biasfile>>stemp;
			sstemp<<stemp;
			sstemp>>temp;
			sstemp.clear();
			node[i].setbias(temp);
		}
		biasfile.close();
	}
	void setweight(int layernumber){
		string filelocation="./weightfile/";
		stringstream sstemp;
		string filename;
		for(int i=0;i<nodenumber;i++){
			ifstream weightfile;
			sstemp<<filelocation<<"l"<<layernumber<<"n"<<i<<".dat";
			sstemp>>filename;
			sstemp.clear();
			this->node[i].setweight(filename.data());
			weightfile.close();
		}
	}
	void getoutput(){
		for(int i=0;i<nodenumber;i++){
	//		cerr<<"node "<<i<<" output"<<endl;
			output[i]=node[i].calculation();
		}
	}
	void getoutputwithoutsigmoid(){
		for(int i=0;i<nodenumber;i++)
			output[i]=node[i].calculationwithoutsigmoid();
	}

};
bool check(string s, string origen){
	if(s!=origen)
		return false;
	else
		return true;
}
void inputlayer(ifstream &weightfile, ofstream &dnnconf, int &layernumber){
	if(layernumber!=0){
		cerr<<"inputlayer is not the first layer"<<endl;
		exit(0);
	}
	dnnconf<<"0 [ ] input ";
	string temp;
	weightfile>>temp;
	weightfile>>temp;
	dnnconf<<temp<<" "<<temp<<endl;
	layernumber++;
}
void extractionlayer(ifstream &weightfile, ofstream &dnnconf, int &layernumber){
	string temp;
	weightfile>>temp;
	dnnconf<<layernumber<<" [ ";
	if(check(temp,"[")){
		weightfile>>temp;
		int* prelayer=new int[layernumber];
		int prelayercount=0;
		while(!check(temp,"]") && !weightfile.eof()){
			prelayer[prelayercount++]=atoi(temp.data());
			weightfile>>temp;
		}
		if(prelayercount>=layernumber || weightfile.eof()){
			cerr<<"in extractionlayer need a ]"<<endl;
			exit(0);
		}
		for(int i=0;i<prelayercount;i++)
			dnnconf<<prelayer[i]<<" ";
		if(check(temp,"]"))
			weightfile>>temp;
	}
	dnnconf<<"] extractionlayer "<<temp<<" ";	
	weightfile>>temp;
	dnnconf<<temp<<" ";
	weightfile>>temp;
	dnnconf<<temp<<" ";
	weightfile>>temp;
	dnnconf<<temp<<" "<<endl;
	layernumber++;
}
void sigmoidlayer(ifstream &weightfile, ofstream &dnnconf, int &layernumber){
	string temp;
	weightfile>>temp;
	weightfile>>temp;
	dnnconf<<layernumber<<" [ ] sigmoidlayer "<<temp<<" "<<temp<<endl;
	layernumber++;
}
void softmaxlayer(ifstream &weightfile, ofstream &dnnconf, int &layernumber){
	string temp;
	weightfile>>temp;
	weightfile>>temp;
	dnnconf<<layernumber<<" [ ] softmaxlayer "<<temp<<" "<<temp<<endl;
	layernumber++;	
}
void affinetransformlayer(ifstream &weightfile, ofstream &dnnconf, int &layernumber){
	string temp;
	dnnconf<<layernumber<<" [ ";
	weightfile>>temp;
	if(check(temp,"[")){
		weightfile>>temp;
		int* prelayer=new int[layernumber];
		int prelayercount=0;
		while(!check(temp,"]") && !weightfile.eof()){
			prelayer[prelayercount++]=atoi(temp.data());
			weightfile>>temp;
		}
		if(prelayercount>=layernumber || weightfile.eof()){
			cerr<<"in affinetransformlayer need a ]"<<endl;
			exit(0);
		}
		for(int i=0;i<prelayercount;i++)
			dnnconf<<prelayer[i]<<" ";
		if(check(temp,"]"))
			weightfile>>temp;
	}
	int outputnumber=atoi(temp.data());
	weightfile>>temp;
	int inputnumber=atoi(temp.data());
	weightfile>>temp;
	dnnconf<<"] affinetransformlayer "<<outputnumber<<" "<<inputnumber<<endl;

	stringstream ss;
	string filename;

	for(int i=0;i<outputnumber;i++){
		ss<<"./weightfile/l"<<layernumber<<"n"<<i<<".dat";
		ss>>filename;
		ss.clear();
		ofstream tempfile(filename.data());
		if(!tempfile){
			cerr<<"create weigth file failed in "<<filename<<endl;
			exit(0);
		}
		for(int j=0;j<inputnumber;j++){
			weightfile>>temp;
			float tempfloat=atof(temp.data());
			tempfloat*=1024;
			tempfile<<int(tempfloat)<<" ";
		}
		tempfile.close();
	}
	weightfile>>temp;
	if(!check(temp,"]")){
		cerr<<"after node number need ] in affinetransformlayer"<<endl;
		exit(0);
	}
	weightfile>>temp;
	if(!check(temp,"[")){
		cerr<<"before bias need [ in affinetransformlayer"<<endl;
		exit(0);
	}
	ss<<"./weightfile/l"<<layernumber<<"bias.dat";
	ss>>filename;
	ofstream biastempfile(filename.data());
	if(!biastempfile){
		cerr<<"create biasfile failed in "<<filename<<endl;
		exit(0);
	}
	for(int i=0;i<outputnumber;i++){
		weightfile>>temp;
		float tempfloat=atof(temp.data());
		tempfloat*=1024;
		biastempfile<<int(tempfloat)<<" ";
	}
	biastempfile.close();
	weightfile>>temp;
	if(!check(temp,"]")){
		cerr<<"after bias file need ] in affinetransformlayer"<<endl;
		exit(0);
	}
	layernumber++;
}


void creatweightfile(char* path){
	ifstream weightfile(path);
	string temp;
	weightfile>>temp;
	if(!check(temp,"<Nnet>")){
		cerr<<"in weightfile need <Nnet>"<<endl;
		exit(0);
	}
	weightfile>>temp;
	int prenumber=0;
	int nodenumber=0;
	int layercount=1;
/*
	while(!check(temp,"</Nnet>")){
		if(check(temp,"<extraction>"))
			extractionlayer(weightfile,dnnconf,layernumber);
		else if(check(temp,"<input>"))
			inputlayer(weightfile,dnnconf,layernumber);
		else if(check(temp,"<affinetransform>"))
			affinetransformlayer(weightfile,dnnconf,layernumber);
		else if(check(temp,"<sigmoid>"))
			sigmoidlayer(weightfile,dnnconf,layernumber);
		else if(check(temp,"<softmax>"))
			softmaxlayer(weightfile,dnnconf,layernumber);
		else{
			cerr<<"need layer name that in setting"<<endl;
			exit(0);
		}
		weightfile>>temp;
	}
	weightfile.close();
	dnnconf.close();*/

///////////transform first layer////////////////
/*	weightfile>>temp;
	if(!check(temp,"<input>"))
		exit(0);
	weightfile>>temp;
	weightfile>>temp;
	prenumber=atoi(temp.data());
	weightfile>>temp;
	if(!check(temp,"<affinetransform>"))
		exit(0);
	*/
	if(!check(temp,"<input>")){
		cerr<<"in weightfile need <input>"<<endl;
		exit(0);
	}
	weightfile>>temp;
	int inputnumber=atoi(temp.data());
	weightfile>>temp;
	weightfile>>temp;
	weightfile>>temp;
	prenumber=atoi(temp.data());
	nodenumber=inputnumber/prenumber;
	ofstream l1biasfile("./weightfile/l1bias.dat");
	for(int i=0;i<nodenumber;i++){
		while(!check(temp,"<affinetransform>"))
			weightfile>>temp;
		while(!check(temp,"]"))
			weightfile>>temp;
		weightfile>>temp;
		weightfile>>temp;
		weightfile>>temp;
		if(!check(temp,"[")){
			cerr<<"need [ in affinetransform layer 0 node "<<i<<endl;
			exit(0);
		}
		ofstream tempfile;
		string filename;
		stringstream s;
		s<<"./weightfile/l"<<layercount<<"n"<<i<<".dat";
		s>>filename;
		s.clear();
		tempfile.open(filename.data());
		for(int j=0;j<prenumber;j++){
			weightfile>>temp;
			float temint;
			temint=atof(temp.data());
			temint*=1024;
			tempfile<<int(temint)<<" ";
		}
		tempfile.close();
		weightfile>>temp;
		if(!check(temp,"]")){
			cerr<<"need ] in affinetransform layer 0 node "<<i<<endl;
			exit(0);
		}
		weightfile>>temp;
		if(!check(temp,"[")){
			cerr<<"need [ before bias in affinetransform layer 0 node "<<i<<endl;
			exit(0);
		}
		weightfile>>temp;
		float temint=atof(temp.data());
		temint*=1024;
		l1biasfile<<int(temint)<<" ";
	}
	layercount++;
	l1biasfile.close();
	while(!check(temp,"<affinetransform>"))
			weightfile>>temp;
	while(!check(temp,"]"))
		weightfile>>temp;
	weightfile>>temp;
	nodenumber=atoi(temp.data());
	weightfile>>temp;
	prenumber=atoi(temp.data());
	weightfile>>temp;
	if(!check(temp,"[")){
		cerr<<"need [ in affinetransform layer "<<layercount<<endl;
		exit(0);
	}
	for(int i=0;i<nodenumber;i++){
		stringstream ss;
		ss<<"./weightfile/l"<<layercount<<"n"<<i<<".dat";
		string filename;
		ss>>filename;
		ss.clear();
		ofstream tempweightfile(filename.data());
		for(int j=0;j<prenumber;j++){
			weightfile>>temp;
			float floattemp=atof(temp.data());
			floattemp*=1024;
			tempweightfile<<int(floattemp)<<" ";
		}
		tempweightfile.close();
	}
	weightfile>>temp;
	if(!check(temp,"]")){
		cerr<<"need [ in affinetransform layer "<<layercount<<endl;
		exit(0);
	}
	weightfile>>temp;
	if(!check(temp,"[")){
		cerr<<"need ] before bias in affinetransform layer "<<layercount<<endl;
		exit(0);
	}
	ofstream biasfile;
	biasfile.open("./weightfile/l2bias.dat");
	for(int j=0;j<nodenumber;j++){
			weightfile>>temp;
			float temint;
			temint=atof(temp.data());
			temint*=1024;
			biasfile<<int(temint)<<" ";
	}
	biasfile.close();
	weightfile>>temp;
	if(!check(temp,"]")){
		cerr<<"need ] after bias in affinetransform layer "<<layercount<<endl;
		exit(0);
	}
	weightfile>>temp;


	while(!check(temp,"<softmax>")){
		layercount++;
		if(!check(temp,"<sigmoid>")){
			cerr<<"need <sigmoid> in layer "<<layercount<<endl;
			exit(0);
		}
		weightfile>>temp;
		weightfile>>temp;
		weightfile>>temp;
		if(!check(temp,"<affinetransform>")){
			cerr<<"need <affinetransform> in layer "<<layercount<<endl;
			exit(0);
		}
		weightfile>>temp;
		nodenumber=atoi(temp.data());
		weightfile>>temp;
		prenumber=atoi(temp.data());
		weightfile>>temp;
		if(!check(temp,"[")){
			cerr<<"need [ in affinetransform layer"<<layercount<<endl;
			exit(0);
		}
		for(int i=0;i<nodenumber;i++){
			ofstream tempfile;
			string filename;
			stringstream s;
			s<<"./weightfile/l"<<layercount<<"n"<<i<<".dat";
			s>>filename;
			s.clear();
			tempfile.open(filename.data());
			for(int j=0;j<prenumber;j++){
				weightfile>>temp;
				float temint;
				temint=atof(temp.data());
				temint*=1024;
				tempfile<<int(temint)<<" ";
			}
			tempfile.close();
		}
		weightfile>>temp;
		if(!check(temp,"]")){
			cerr<<"need ] in affinetransform layer "<<layercount<<endl;
			exit(0);
		}
		weightfile>>temp;
		if(!check(temp,"[")){
			cerr<<"need [ before bias in affinetransform layer "<<layercount<<endl;
			exit(0);
		}
		string ttemp;
		stringstream s;
		s<<"./weightfile/l"<<layercount<<"bias.dat";
		s>>ttemp;
		s.clear();
		biasfile.open(ttemp.data());
		for(int j=0;j<nodenumber;j++){
			weightfile>>temp;
			float temint;
			temint=atof(temp.data());
			temint*=1024;
			biasfile<<int(temint)<<" ";
		}
		biasfile.close();
		weightfile>>temp;
		if(!check(temp,"]")){
			cerr<<"need ] after affinetransform layer "<<layercount<<endl;
			exit(0);
		}
		weightfile>>temp;
	}
	weightfile.close();
}
/*
double* softmaxlayer(int* input,int inputnumber){
	double domi=0;
	for(int i=0;i<inputnumber;i++)
		domi+=double(1)/exp(input[i]);
	double* output=new double[inputnumber];
	for(int i=0;i<inputnumber;i++)
		output[i]=exp(input[i])/domi;
	return output;
}
*/
/*
int* softmaxlayerverilog(int* input, int inputnumber){
 	int max=0;
 	for(int i=0;i<inputnumber;i++)
 		if(input[i]>max)
 			max=input[i];
 	int* x_int=new int[inputnumber];
 	int* x_frac1=new int[inputnumber];
 	int* x_frec2=new int[inputnumber];
 	int* expint=new int[inputnumber];
 	int* expfrac1=new int[inputnumber];
 	int* expfrac2=new int[inputnumber];
 	for(int i=0;i<inputnumber;i++){
 		input[i]-=max;
 		x_int[i]=changebit(input[i],10,19);
 		x_frac1[i]=changebit(input[i],5,9,0);
 		x_frac2[i]=changebit(input[i],0,4,0);
 		switch(x_int){
 			case 0: expint=1024;break;
 			case -1: expint=512;break;
 			case -2: expint=256;break;
 			case -3: expint=128;break;
 			case -4: expint=64;break;
 			case -5: expint=32;break;
 			case -6: expint=16;break;
 			case -7: expint=8;break;
 			case -8: expint=4;break;
 			case -9: expint=2;break;
 			default: expint=1;
 		}
 		switch(x_frac1){
 			case 0:expfrac1=1024;break;
			case 1:expfrac1=1046;break;
			case 2:expfrac1=1069;break;
			case 3:expfrac1=1092;break;
			case 4:expfrac1=1116;break;
			case 5:expfrac1=1141;break;
			case 6:expfrac1=1166;break;
			case 7:expfrac1=1191;break;
			case 8:expfrac1=1217;break;
			case 9:expfrac1=1244;break;
			case 10:expfrac1=1271;break;
			case 11:expfrac1=1299;break;
			case 12:expfrac1=1327;break;
			case 13:expfrac1=1357;break;
			case 14:expfrac1=1386;break;
			case 15:expfrac1=1417;break;
			case 16:expfrac1=1448;break;
			case 17:expfrac1=1479;break;
			case 18:expfrac1=1512;break;
			case 19:expfrac1=1545;break;
			case 20:expfrac1=1579;break;
			case 21:expfrac1=1613;break;
			case 22:expfrac1=1649;break;
			case 23:expfrac1=1685;break;
			case 24:expfrac1=1722;break;
			case 25:expfrac1=1759;break;
			case 26:expfrac1=1798;break;
			case 27:expfrac1=1837;break;
			case 28:expfrac1=1878;break;
			case 29:expfrac1=1919;break;
			case 30:expfrac1=1961;break;
			case 31:expfrac1=2004;break;
 		}
 		switch(){
 			case 0:expfrac2= 2048;break;
			case 1:expfrac2= 2049;break;
			case 2:expfrac2= 2050;break;
			case 3:expfrac2= 2052;break;
			case 4:expfrac2= 2053;break;
			case 5:expfrac2= 2054;break;
			case 6:expfrac2= 2056;break;
			case 7:expfrac2= 2057;break;
			case 8:expfrac2= 2059;break;
			case 9:expfrac2= 2060;break;
			case 10:expfrac2= 2061;break;
			case 11:expfrac2= 2063;break;
			case 12:expfrac2= 2064;break;
			case 13:expfrac2= 2066;break;
			case 14:expfrac2= 2067;break;
			case 15:expfrac2= 2068;break;
			case 16:expfrac2= 2070;break;
			case 17:expfrac2= 2071;break;
			case 18:expfrac2= 2073;break;
			case 19:expfrac2= 2074;break;
			case 20:expfrac2= 2075;break;
			case 21:expfrac2= 2077;break;
			case 22:expfrac2= 2078;break;
			case 23:expfrac2= 2080;break;
			case 24:expfrac2= 2081;break;
			case 25:expfrac2= 2082;break;
			case 26:expfrac2= 2084;break;
			case 27:expfrac2= 2085;break;
			case 28:expfrac2= 2087;break;
			case 29:expfrac2= 2088;break;
			case 30:expfrac2= 2090;break;
			case 31:expfrac2= 2091;break;
 		}


 	}

 }
*/

int main(int argc,char** argv){
	if(argc<5){
		cerr<<"program inputnumberfile weightfile config outputfile"<<endl;
		exit(0);
	}
	char *cinputfile=argv[1];
	char *cweightfile=argv[2];
	int filelength=strlen(cweightfile);
	char* weightdir=new char[filelength-9];
	char *coutputfile=argv[4];
	char *cnetworkconfig=argv[3];
	cout<<"ready to create weight file"<<endl;
	//	creatweightfile(cweightfile);
	cout<<"create weight file finished"<<endl;
	string tempstring;
	stringstream sstemp;
	int layernumber;
	string weightfilelocation="./weightfile/";	//格式lxnx.dat lxbias.dat
	Layer* network=NULL;
	ifstream fnetworkconfig(argv[3]);
	if(!fnetworkconfig){
		cerr<<"config file is not exist"<<endl;
		exit(0);
	}
	fnetworkconfig>>tempstring;
	cout<<"read configs"<<endl;
	if(!check(tempstring,"<LAYERNUMBER>")){
		cerr<<"in dnnconfig file need <LAYERNUMBER>"<<endl;
		exit(0);
	}
	fnetworkconfig>>tempstring;
	sstemp<<tempstring;
	sstemp>>layernumber;
	sstemp.clear();
	network=new Layer[layernumber];
	fnetworkconfig>>tempstring;
	if(!check(tempstring,"<NODENUMBER>")){
		cerr<<"in dnnconfig file need <NODENUMBER>"<<endl;
		exit(0);
	}
	for(int i=0;i<layernumber;i++){
		int nodenumber=0;
		int prenodenumber=0;
		fnetworkconfig>>tempstring;
		sstemp<<tempstring;	
		sstemp>>prenodenumber;
		sstemp.clear();
		fnetworkconfig>>tempstring;
		sstemp<<tempstring;
		sstemp>>nodenumber;
		sstemp.clear();
		network[i].init(prenodenumber,nodenumber);
		network[i].setweight(i+1);
		network[i].setbias(i+1);
	}
	ifstream finputfile(cinputfile);
	if(!finputfile){
		cerr<<"input file not exist in: "<<cinputfile<<endl;
		exit(0);
	}
	finputfile>>tempstring;
	cout<<"read numbers"<<endl;
	if(!check(tempstring,"<NUMBER>")){
		cerr<<"in input file need <NUMBER>"<<endl;
		exit(0);
	}
	sstemp.clear();
	finputfile>>tempstring;
	sstemp<<tempstring;
	int inputnumber;
	sstemp>>inputnumber;
	sstemp.clear();

	int* inputarray=new int[inputnumber];
	int irtimes=inputnumber/12;
	finputfile>>tempstring;
	if(!check(tempstring,"<INPUT>")){
		cerr<<"in input file need <INPUT>"<<endl;
		exit(0);
	}
	for(int i=0;i<inputnumber;i++){
		finputfile>>tempstring;
		sstemp<<tempstring;
		int number;
		sstemp>>number;
		inputarray[i]=number;
		sstemp.clear();
	}
	ofstream foutputfile(coutputfile);
	if(!foutputfile){
		cerr<<"create foutputfile failed in "<<coutputfile<<endl;
		exit(0);
	}
//	ofstream softout("./softout.log");
	cout<<"begin calculate"<<endl;
	for(int i=0;i<irtimes;i++){
		cout<<"calculate "<<i<<" round"<<endl;
		for(int j=0;j<network[0].nodenumber;j++)
			network[0].node[j].move(inputarray,i,j,irtimes);
		for(int j=0;j<layernumber-1;j++){
			cout<<"calculate layer "<<j<<" output"<<endl;
			network[j].getoutput();
			for(int k=0;k<network[j+1].nodenumber;k++)
				network[j+1].node[k].setinput(network[j].output);
		}
		cout<<"calculate layer "<<layernumber-1<<" output"<<endl;
		network[layernumber-1].getoutput();
		for(int j=0;j<network[layernumber-1].nodenumber;j++)
			foutputfile<<network[layernumber-1].output[j]<<" ";
		foutputfile<<endl;
		foutputfile<<endl;
	//	double* softout=new double[network[layernumber-1].nodenumber];
	//	softout=softmaxlayer(network[layernumber-1].output,network[layernumber-1].nodenumber);
	}
//	for(int i=0;i<network[layernumber-1].nodenumber;i++)
//		softout<<softout[i]<<" ";
//	softout.close();
	fnetworkconfig.close();
	delete[] network;
/*
	int** inputarray=NULL;
	int irtimes=int(inputnumber/network[0].prenodenumber);
	inputarray=new int*[irtimes];
	finputfile>>tempstring;
	if(!check(tempstring,"<INPUT>"))
		exit(0);
	for(int i=0;i<irtimes;i++)
		inputarray[i]=new int[network[0].prenodenumber];
	for(int i=0;i<irtimes;i++)
		for(int j=0;j<network[0].prenodenumber;j++){
			finputfile>>tempstring;
			sstemp<<tempstring;
			int number;
			sstemp>>number;
	//	cerr<<"inputarray["<<i<<"]["<<j<<"]="<<number<<endl;
			inputarray[i][j]=number;
			sstemp.clear();
		}
	ofstream foutputfile(coutputfile);
	for(int i=0;i<irtimes;i++){
		for(int j=0;j<network[0].nodenumber;j++)
			network[0].node[j].setinput(inputarray[i]);
		for(int j=0;j<layernumber-1;j++){
			//cerr<<"calculate layer "<<j<<" output"<<endl;
			network[j].getoutput();
			for(int k=0;k<network[j].nodenumber;k++)
			//	cerr<<"network["<<j<<"].node["<<k<<"]="<<network[j].output[k]<<endl;
			for(int k=0;k<network[j+1].nodenumber;k++)
				network[j+1].node[k].setinput(network[j].output);
		}
		network[layernumber-1].getoutput();
		for(int j=0;j<network[layernumber-1].nodenumber;j++)
			foutputfile<<network[layernumber-1].output[j]<<" ";
	}
	fnetworkconfig.close();
	delete[] network;
*/
	return 0;
}	
