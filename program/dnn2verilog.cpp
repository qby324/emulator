#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
using namespace std;
int main(int argc,char** argv){
	if(argc<3){
		cerr<<"need program dnnfile vrilogfile"<<endl;
		exit(0);
	}
	ifstream dnnfile(argv[1]);
	ofstream verilogfile(argv[2]);
	if(!dnnfile){
		cerr<<"dnnfile not exist in "<<argv[1]<<endl;
		exit(0);
	}
	verilogfile<<"case (index)"<<endl;
	verilogfile<<"default:begin"<<endl;
	verilogfile<<"end"<<endl;
	int count=1;
	while(!dnnfile.eof()){
		verilogfile<<count<<":begin"<<endl;
		for(int i=0;i<65;i++){
			verilogfile<<"vec2["<<i<<"] <= ";
			string temp;
			dnnfile>>temp;
			if(dnnfile.eof())
				temp="0";
			verilogfile<<temp<<";"<<endl;
		}
		verilogfile<<"end"<<endl;
		count++;
	}
	verilogfile<<"endcase"<<endl;
	verilogfile.close();
	dnnfile.close();


	return 0;
}