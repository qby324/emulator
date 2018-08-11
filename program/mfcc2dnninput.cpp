#include <iostream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <math.h>
#include <fstream>
using namespace std;
unsigned long int changebit(unsigned long int n,int b,int c, bool sign=true){
  unsigned long int a=0;
  unsigned long int con=1;
  for(int i=b;i<=c;i++){
    a+=(n&(con<<i))>>b;
  }
  if(sign&&n&(con<<c)){
    for(int i=c-b+1;i<64;i++)
      a+=(con<<i);
  }
  return a;
}

string H2B(string Hnumber){
	string::iterator i;
	string temp="";
	//int j=3;
	for(i=Hnumber.begin();i!=Hnumber.end();i++){
		switch(*i){
		case 'f':temp+="1111";
		break;

		case 'e':temp+="1110";
		break;

		case 'd':temp+="1101";
		break;

		case 'c':temp+="1100";
		break;

		case 'b':temp+="1011";
		break;

		case 'a':temp+="1010";
		break;

		case '9':temp+="1001";
		break;

		case '8':temp+="1000";
		break;

		case '7':temp+="0111";
		break;	

		case '6':temp+="0110";
		break;

		case '5':temp+="0101";
		break;

		case '4':temp+="0100";
		break;

		case '3':temp+="0011";
		break;

		case '2':temp+="0010";
		break;

		case '1':temp+="0001";
		break;

		case '0':temp+="0000";
		break;

		default:temp+="erro";
		break;
		}
	//	j--;
	}
	return temp;
}
bool check(string s, string origen,int line=-1){
	if(s!=origen)
		return false;
	else
		return true;
};

int getconfig(int* mean,int* var, char* filename){
	ifstream configfile(filename);
	string temp;
	stringstream sstemp;
	float number;
	while(!check(temp,"<MEAN>") && !configfile.eof())
		configfile>>temp;
	cout<<"if <MEAN>="<<temp<<endl;
	if(configfile.eof()){
		cerr<<"<MEAN> not exist in configfile"<<endl;
		exit(0);
	}
	configfile>>temp;
	cout<<"meannumber="<<temp<<endl;
	int meannumber=atoi(temp.data());
//	mean=(int*)malloc(4*meannumber);
	for(int i=0;i<meannumber;i++){
		configfile>>temp;
		cout<<temp<<" ";
		sstemp<<temp;
		sstemp>>number;
		number*=16384;
		mean[i]=number;
		sstemp.clear();
	}
	cout<<endl;
	configfile>>temp;
	cout<<"if <VARIANCE>="<<temp<<endl;
	if(!check(temp,"<VARIANCE>")){
		cerr<<"need <VARIANCE> in configfile"<<endl;
		exit(0);
	}
	configfile>>temp;
	cout<<"variance number="<<temp<<endl;
	int vernumber=atoi(temp.data());
//	var=(int*)malloc(4*vernumber);
	for(int i=0;i<vernumber;i++){
		configfile>>temp;
		cout<<temp<<" ";
		sstemp<<temp;
		sstemp>>number;
		number=1.0/sqrt(number);
		number*=16832;
		var[i]=changebit(number,0,13);
		sstemp.clear();
	}
	cout<<endl;
	return meannumber;
}
float transform(string hexnumber){
	string bnumber=H2B(hexnumber);
//	cerr<<bnumber<<endl;
	string::iterator itr=bnumber.begin();
	bool flag=false;
	int N=0;
	float M=0;
	if(*itr=='1')
		flag=true;
	itr++;
	for(int i=7;i>=0;i--){
		if(*itr=='1')
			N+=(1<<i);
		itr++;
	}
	N-=127;
	for(int i=1;i<=23;i++){
		if(*itr=='1')
			M+=pow(2.0,-i);
		itr++;
	}
	M++;
	M*=pow(2.0,N);
	if(flag)
		M*=-1;
	return M;
}

int main(int argc,char**argv){
	if(argc<4){
		cerr<<"program mfccfile configfile outputfile"<<endl;
		exit(0);
	}
	int count=0;
	string temp;
	float tempf;
	long int number;
	ifstream countnunmber(argv[1]);
	if(!countnunmber){
		cerr<<"mfccfile not exist in: "<<argv[1]<<endl;
		exit(0);
	}
	while(!countnunmber.eof()){
		string tempp;
		countnunmber>>tempp;
		count++;
	}
	countnunmber.close();
	ifstream mfccfile(argv[1]);
	int mean[12];
	int var[12];
//	int* mean=NULL;
//	int* var=NULL;
	int meannumber;
	meannumber=getconfig(mean,var,argv[2]);
	ofstream outputfile(argv[3]);
	outputfile<<"<NUMBER>"<<endl;
	outputfile<<int((count-4)/13*6)<<endl;
	outputfile<<"<INPUT>"<<endl;
	mfccfile>>temp;
	mfccfile>>temp;
	mfccfile>>temp;
	mfccfile>>temp;
//	cout<<"ready to read number"<<endl;
	while(!mfccfile.eof()){
		for(int i=0;i<meannumber;i++){
			if(mfccfile.eof())
				break;
			mfccfile>>temp;
			//ll
	//		cout<<temp<<" ";
			float tempf=atof(temp.data());
			tempf*=16384;
			//			tempf=0;
			number=tempf;
			number-=mean[i];
			number=changebit(number,0,19);	//to 20bit signed number
			number*=var[i];
			number=int(changebit(number,18,32));	
			outputfile<<number<<" ";
		}
		for(int i=0;i<14;i++)
			mfccfile>>temp;
	}

	return 0;
}
