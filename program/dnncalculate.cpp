#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <sstream>
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
int sigmoid(int number){/*
	unsigned int dividend=0,divisor=0,divresult=0,divcnt=0;
	bool flag=false;
	if(number<0){
		flag=1;
		dividend=changebitunsigned((changebitunsigned(~number+1,0,26)<<1),0,26);
		divisor=changebitunsigned(changebitunsigned(number,0,26)+1025,0,26);
	}
	else{
		dividend=changebitunsigned(number<<1,0,26);
		divisor=changebitunsigned(number+1024,0,26);
	}
//	cerr<<dividend<<endl;
//	cerr<<divisor<<endl;
	for(divcnt=0;divcnt<10;divcnt++){
		if(dividend>divisor){
			dividend=changebitunsigned((dividend-divisor)<<1,0,26);
			int temp=1<<(9-divcnt);
		//	cerr<<"temp1:"<<temp<<endl;
			divresult=changebitunsigned(divresult | temp,0,11);
		}
		else{
			dividend=changebitunsigned(dividend<<1,0,26);
			int temp=~(1<<(9-divcnt));
		//	cerr<<"temp0:"<<temp<<endl;
			divresult=changebitunsigned(divresult & temp,0,11);
		}
	//	cerr<<dividend<<endl;
	//	cerr<<divisor<<endl;
	}
	if(flag)
		divresult=~divresult+1025;
	else
		divresult+=1024;
//	cerr<<"divresult="<<divresult<<endl;
	divresult=changebitunsigned(divresult,1,11);
	if(divresult & (1<<10))
		divresult=divresult | 0xFFFFF800;
	return divresult;
*/
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
	void move(int* newinput,int beginnumber){
		int* moveinput=new int[weightnumber];
		for(int i=0;i<weightnumber-12;i++)
			moveinput[i]=this->input[i+12];
		int count=0;
		for(int i=weightnumber-12;i<weightnumber;i++)
			moveinput[i]=newinput[beginnumber+count++];
		for(int i=0;i<weightnumber;i++)
			input[i]=moveinput[i];
		flatcal=0;
		flatintput=1;
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
		//	cerr<<"node "<<i<<" output"<<endl;
			output[i]=node[i].calculation();
		}
	}

};
bool check(string s, string origen,int line=-1){
	if(s!=origen){
		if(origen!="<softmax>")
			cerr<<"file wrong need "<<origen<<"in line "<<line<<endl;
		return false;
	}
	else
		return true;
}



void creatweightfile(char* path){
	ifstream weightfile(path);
	string temp;
	int prenumber=-1;
	int nodenumber=-1;
	int layercount=1;
	weightfile>>temp;
	if(!check(temp,"<Nnet>"))
		exit(0);
///////////transform first layer///////////////////
	weightfile>>temp;
	if(!check(temp,"<input>"))
		exit(0);
	weightfile>>temp;
	weightfile>>temp;
	prenumber=atoi(temp.data());
	weightfile>>temp;
	if(!check(temp,"<affinetransform>"))
		exit(0);
	weightfile>>temp;
	nodenumber=atoi(temp.data());
	weightfile>>temp;
	weightfile>>temp;
	if(!check(temp,"["))
		exit(0);
	for(int i=0;i<nodenumber;i++){
		ofstream tempfile;
		string filename;
		stringstream s;
		s<<"./weightfile/l"<<layercount<<"n"<<i<<".dat";
		s>>filename;
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
	if(!check(temp,"]"))
		exit(0);
	weightfile>>temp;
	if(!check(temp,"["))
		exit(0);
	ofstream biasfile;
	biasfile.open("./weightfile/l1bias.dat");
	for(int j=0;j<nodenumber;j++){
			weightfile>>temp;
			float temint;
			temint=atof(temp.data());
			temint*=1024;
			biasfile<<int(temint)<<" ";
	}
	biasfile.close();
	weightfile>>temp;
	if(!check(temp,"]"))
		exit(0);
	weightfile>>temp;
	while(!check(temp,"<softmax>")){
		check(temp,"<sigmoid>");
		weightfile>>temp;
		weightfile>>temp;
		prenumber=atoi(temp.data());
		weightfile>>temp;
		check(temp,"<affinetransform>");
		weightfile>>temp;
		nodenumber=atoi(temp.data());
		weightfile>>temp;
		weightfile>>temp;
		check(temp,"[");
		layercount++;
		for(int i=0;i<nodenumber;i++){
			ofstream tempfile;
			string filename;
			stringstream s;
			s<<"./weightfile/l"<<layercount<<"n"<<i<<".dat";
			s>>filename;
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
		check(temp,"]",157);
		weightfile>>temp;
		check(temp,"[");
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
		check(temp,"]",168);
		weightfile>>temp;
	}
	weightfile.close();
}



int main(int argc,char** argv){
	if(argc<5){
		cerr<<"program inputnumberfile weightfile config outputfile"<<endl;
		exit(0);
	}
	char *cinputfile=argv[1];
	char *cweightfile=argv[2];
	char *coutputfile=argv[4];
	char *cnetworkconfig=argv[3];

	creatweightfile(cweightfile);
	
	//creatweightfile(cweightfile);
	string tempstring;
	stringstream sstemp;
	int layernumber;
	string weightfilelocation="./weightfile/";	//格式lxnx.dat lxbias.dat
	Layer* network=NULL;
	ifstream fnetworkconfig(cnetworkconfig);
	if(!fnetworkconfig){
		cerr<<"config file is not exist"<<endl;
		exit(0);
	}
	fnetworkconfig>>tempstring;
	if(!check(tempstring,"<LAYERNUMBER>"))
		exit(0);
	fnetworkconfig>>tempstring;
	sstemp<<tempstring;
	sstemp>>layernumber;
	sstemp.clear();
	network=new Layer[layernumber];
	fnetworkconfig>>tempstring;
	if(!check(tempstring,"<NODENUMBER>"))
		exit(0);
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
	if(!check(tempstring,"<NUMBER>"))
		exit(0);
	sstemp.clear();
	finputfile>>tempstring;
	sstemp<<tempstring;
	int inputnumber;
	sstemp>>inputnumber;
	sstemp.clear();

	int* inputarray=new int[inputnumber];
	int irtimes=inputnumber/12;
	finputfile>>tempstring;
	if(!check(tempstring,"<INPUT>"))
		exit(0);
	for(int i=0;i<inputnumber;i++){
		finputfile>>tempstring;
		sstemp<<tempstring;
		int number;
		sstemp>>number;
		inputarray[i]=number;
		sstemp.clear();
	}
	ofstream foutputfile(coutputfile);
	for(int i=0;i<irtimes;i++){
		//cerr<<"calculate "<<i<<" round"<<endl;
		for(int j=0;j<network[0].nodenumber;j++)
			network[0].node[j].move(inputarray,i*12);
		for(int j=0;j<layernumber-1;j++){
		//	cerr<<"calculate layer "<<j<<" output"<<endl;
			network[j].getoutput();
			for(int k=0;k<network[j+1].nodenumber;k++)
				network[j+1].node[k].setinput(network[j].output);
		}
		//cerr<<"calculate layer "<<layernumber-1<<" output"<<endl;
		network[layernumber-1].getoutput();
		for(int j=0;j<network[layernumber-1].nodenumber;j++)
			foutputfile<<network[layernumber-1].output[j]<<" ";
		foutputfile<<endl;
		foutputfile<<endl;
	}
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