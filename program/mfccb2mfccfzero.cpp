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
	if(argc<3){
		cerr<<"need program mfccoutbinary mfccoutchar"<<endl;
		exit(0);
	}
	ofstream mfccoutchar(argv[2]);
	FILE* mfccoutbinary=NULL;
	mfccoutbinary=fopen(argv[1],"rb");
	if(mfccoutbinary==NULL){
		cerr<<"open mfccoutfile failed"<<endl;
		exit(0);
	}
	htk_header* header;
	char* msg=NULL;
	header=readhtkheader(mfccoutbinary,header,msg);
	mfccoutchar<<header->nSamples<<" "<<header->sampPeriod<<" "<<header->sampSize<<" "<<header->parmKind<<endl;
	float* samp=new float[1];
	int flag=1;
	flag=fread(samp,4,1,mfccoutbinary);
	while(flag){
		samp=bswaplf32(samp);
		mfccoutchar<<"0 ";
//		cout<<samp[0]<<endl;
		delete[] samp;
		samp=new float[1];
		flag=fread(samp,4,1,mfccoutbinary);
	}

	mfccoutchar.close();
	fclose(mfccoutbinary);
	return 0;
}