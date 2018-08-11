#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <values.h>
using namespace std;
typedef struct __htk_header__ {
  int32_t nSamples;
  int32_t sampPeriod;
  int16_t sampSize;
  int16_t parmKind;
} htk_header;
bool	
isLittle(void)
{
  int n = 1;
  if (*(char*)&n) {
    return(1);
  } else {
    return(0);
  }
}
int16_t *
bswapli16(int16_t *p)
{
   char temp,*q;
   
   if (isLittle()) {
     q = (char*) p;
     temp = *q; *q = *(q+1); *(q+1) = temp;
   }
   return p;
}

/* Apply byte swap if this machine uses little endian */
int32_t *
bswapli32(int32_t *p)
{
   char temp,*q;
   
   if (isLittle()) {
     q = (char*) p;
     temp = *q; *q = *(q+3); *(q+3) = temp;
     temp = *(q+1); *(q+1) = *(q+2); *(q+2) = temp;
   }
   return p;
}

/* apply byte swap if this machine uses little endian */
float *
bswaplf32(float *p)
{
   char temp,*q;
   
   if (isLittle()) {
     q = (char*) p;
     temp = *q; *q = *(q+3); *(q+3) = temp;
     temp = *(q+1); *(q+1) = *(q+2); *(q+2) = temp;
   }
   return p;
}

size_t
m_fread(void *ptr, size_t size, size_t nmemb, FILE *stream, char *msg)
{
  size_t retv;
  char buf[1000];

  retv = fread(ptr, size, nmemb, stream);
  if (retv == nmemb) {
    return(nmemb);
  } else {
    sprintf(buf, "Error read failed %s: retv %d size %d nmemb %d", msg, (int)retv, (int)size, (int)nmemb);
    perror(buf);
    exit(1);
  }
  return 0;
}

size_t
m_fwrite(void *ptr, size_t size, size_t nmemb, FILE *stream, char *msg)
{
  size_t retv;
  char buf[1000];

  retv = fwrite(ptr, size, nmemb, stream);
  if (retv == nmemb) {
    return(nmemb);
  } else {
    sprintf(buf, "Error write failed %s: retv %d size %d nmemb %d", msg, (int)retv, (int)size, (int)nmemb);
    perror(buf);
    exit(1);
  }
  return 0;
}

htk_header *
readhtkheader(FILE *fp, htk_header *hhp, char *msg)
{
  m_fread(&(hhp->nSamples), 4, 1, fp, msg);
  m_fread(&(hhp->sampPeriod), 4, 1, fp, msg);
  m_fread(&(hhp->sampSize), 2, 1, fp, msg);
  m_fread(&(hhp->parmKind), 2, 1, fp, msg);
  bswapli32(&(hhp->nSamples));
  bswapli32(&(hhp->sampPeriod));
  bswapli16(&(hhp->sampSize));
  bswapli16(&(hhp->parmKind));
  return hhp;
}
htk_header *
writehtkheader(FILE *fp, htk_header *hhp, char *msg)
{
  int32_t nSamples, sampPeriod;
  int16_t sampSize, parmKind;

  nSamples = hhp->nSamples;
  sampPeriod = hhp->sampPeriod;
  sampSize = hhp->sampSize;
  parmKind = hhp->parmKind;
  bswapli32(&nSamples);
  bswapli32(&sampPeriod);
  bswapli16(&sampSize);
  bswapli16(&parmKind);
  m_fwrite(&nSamples, 4, 1, fp, msg);
  m_fwrite(&sampPeriod, 4, 1, fp, msg);
  m_fwrite(&sampSize, 2, 1, fp, msg);
  m_fwrite(&parmKind, 2, 1, fp, msg);
  return hhp;
}
int main(int argc,char** argv){
	if(argc<4){
		cerr<<"need program dnnoutfile dtwinputfile dnnconfig"<<endl;
		exit(0);
	}
  ifstream dnnconfig(argv[3]);
  string temp;
  dnnconfig>>temp;
  dnnconfig>>temp;
  int layernumber=atoi(temp.data());
  dnnconfig>>temp;
  for(int i=0;i<layernumber;i++){
    dnnconfig>>temp;
    dnnconfig>>temp;
  }
  int outputnumber=atoi(temp.data());
	ifstream countfile(argv[1]);
	int count=0;
	while(!countfile.eof()){
		countfile>>temp;
		count++;
	}
	count--;
	countfile.close();
	ifstream dnnoutfile(argv[1]);
	FILE* dtwinputfile;
	dtwinputfile=fopen(argv[2],"wb");
	if(dtwinputfile==NULL){
		cerr<<"open failed in "<<argv[2]<<endl;
		exit(0);
	}
	float** allnumber=new float*[count/outputnumber];
	for(int i=0;i<count/outputnumber;i++){
		allnumber[i]=new float[outputnumber];
		for(int j=0;j<outputnumber;j++){
			dnnoutfile>>temp;
	//		float tempfloat=atof(temp.data());
	//		allnumber[i][j]=tempfloat/float(1024);
			float tempfloat;
			tempfloat=atof(temp.data());
			allnumber[i][j]=*(bswaplf32(&tempfloat));
		}
	}
	htk_header newhtk;
	newhtk.nSamples=count/outputnumber;
	newhtk.sampPeriod=100000;
	newhtk.sampSize=outputnumber*4;
	newhtk.parmKind=9;
	cout<<"nSamples="<<count<<endl;
	writehtkheader(dtwinputfile,&newhtk,"");
	for(int i=0;i<count/outputnumber;i++)
		if(fwrite(allnumber[i],outputnumber*4,1,dtwinputfile)!=1){
			cerr<<"write number "<<i<<" failed"<<endl;
			exit(0);
		}
	if(fclose(dtwinputfile)!=0){
		cerr<<"close dtwinputfile failed"<<endl;
		exit(0);
	}
	dnnoutfile.close();
	return 0;
}
