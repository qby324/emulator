/*
  yohoiadp.c
  A DP matching program
  Takahiro Shinozaki
  Thu Aug 14 19:18:58 JST 2014

  To compile:
  gcc -Wall -o yohoiadp yohoiadp.c -lm
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <values.h>



#define BUFSIZE 10000
int warnlevel=1;

typedef struct __dpopt__ {
  int startrng;
  int endrng;
  int maxhmove;
  int maxvmove;
  int backtrack;
  int ntype;
  int printfrminfo;
  float hmovepenalty;
  float vmovepenalty;
  char *outdirname;
  char *dformat;
} DPOPT;

typedef struct __dpdata_head__ {
  int nSamples;
  int sampPeriod;
  int sampSize;
  int sampDim;
  int parmKind;
  char *id;
} DPDATA_HEAD;

typedef struct __dpdata__ {
  DPDATA_HEAD dpdh;
  float **data;
} DPDATA;

typedef struct __htk_header__ {
  int32_t nSamples;
  int32_t sampPeriod;
  int16_t sampSize;
  int16_t parmKind;
} htk_header;


void
checkrequirements()
{
  if (sizeof(float)!=4) {
    fprintf(stderr, "unsupported float size\n");
    exit(1);
  }
}

void *
m_malloc(size_t size)
{

#ifdef CHECKMALLOC
  return check_malloc(size);
#else
  void *pt;

  if ((pt=malloc(size))==NULL) {
    perror("m_malloc");
    exit(1);
  }
  return pt;
#endif
}

void *
m_realloc(void *ptr, size_t size)
{
#ifdef CHECKMALLOC
  return check_realloc(ptr, size);
#else
  void *pt;

  if ((pt=realloc(ptr, size))==NULL) {
    perror("m_realloc");
    exit(1);
  }
  return pt;
#endif
}


void
m_free(void *ptr)
{
#ifdef CHECKMALLOC
  check_free(ptr);
  return;
#else 
  free(ptr);
  return;
#endif
}

/* See if this machine is little endian or not */
int
isLittle(void)
{
  int n = 1;
  if (*(char*)&n) {
    return(1);
  } else {
    return(0);
  }
}

/* Apply byte swap if this machine uses little endian */
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

FILE *
m_fopen(const char *path, const char *mode)
{
  FILE *fp;
  if ((fp=fopen(path, mode))==NULL) {
    perror(path);
    exit(1);
  }
  return fp;
}

int
m_fclose(FILE *stream)
{
  if (fclose(stream)!=0) {
    perror("fclose");
    exit(1);
  }
  return 0;
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

DPDATA_HEAD *
readdatahead(FILE *fp, DPDATA_HEAD *dpdhp, DPOPT *dpoptp, char *msg)
{
  htk_header hh;

  if (!strcmp(dpoptp->dformat, "htk")) {
    /* HTK file format */
    readhtkheader(fp, &hh, msg);
    dpdhp->nSamples = hh.nSamples;
    dpdhp->sampPeriod = hh.sampPeriod;
    dpdhp->sampSize = hh.sampSize;
    dpdhp->sampDim = hh.sampSize/4;
    dpdhp->parmKind = hh.parmKind;

    if (0<warnlevel && hh.sampSize > 10000) {
      /* probably reading a wrong file */
      fprintf(stderr, "WARN: very large feature size %d\n", hh.sampSize);
    }
  } else {
    /* for future extension */
    fprintf(stderr, "not implemented yet\n");
    exit(1);
  }
  return dpdhp;
}

float *
readdataframe(FILE *fp, float *fbuf, DPDATA_HEAD *dpdhp, DPOPT *dpoptp, char *msg)
{
  int i;

  if (!strcmp(dpoptp->dformat, "htk")) {
    /* HTK file format */
    m_fread(fbuf, dpdhp->sampSize, 1, fp, msg);
    for (i=0; i<dpdhp->sampDim; i++) {
      bswaplf32(&fbuf[i]); /* swap bytes if necessary */
      if (0<warnlevel && (fbuf[i] > 1000000 || fbuf[i] < -1000000)) {
	/* The data might be corrupted */
	fprintf(stderr, "WARN: extreme feature value %s %d: %f\n", msg, i, fbuf[i]);
      }
    }
  } else {
    /* for future extension */
    /* XXX format */
    fprintf(stderr, "not implemented yet\n");
    exit(1);
  }
  return fbuf;
}

/* Read whole data file and store the data in memory */
/* (the larger the data, the more memory is consumed) */
DPDATA *
readdataf(char *fname, DPOPT *dpoptp)
{
  FILE *fp;
  DPDATA *datap;
  float **fseq;
  int i;

  fp = m_fopen(fname, "r");
  datap=(DPDATA *)m_malloc(sizeof(DPDATA));
  readdatahead(fp, &datap->dpdh, dpoptp, fname);

  /* allocate memory to store a frame sequence */
  fseq = (float **)m_malloc(sizeof(float *)*(datap->dpdh.nSamples+2));
  datap->data = fseq;

  fseq[0] = NULL;
  for (i=1; i<=datap->dpdh.nSamples; i++) {
    /* read frame data */
    fseq[i] = (float *)m_malloc(datap->dpdh.sampSize);
    readdataframe(fp, fseq[i], &(datap->dpdh), dpoptp, fname);
  }
  fseq[datap->dpdh.nSamples+1] = NULL;

  return datap;
}

DPDATA *
free_dpdata(DPDATA *datap) {
  m_free(datap->data);
  m_free(datap);
  datap=NULL;
  return datap;
}

/*
 DP matching search space

Length of
template
 ^ 
 |

l+1 _|_|_|_|_|_|_|_|_|_|_|
    _|_|_|_|_|_|_|_|_|_|_|
    _|_|_|_|_|_|_|_|_|_|_|
1   _|_|_|_|_|_|_|_|_|_|_|
0   _|_|_|_|_|_|_|_|_|_|_|
                        --> Time 

    (0) 1 2 3  ...       T 

 */

float
frmdist(float *vec1, float *vec2, int dim, int ntype)
{
  int i;
  int temp;
  float acc=0.0;
  printf("in frmdist \n dim=%d \n",dim);
 // scanf("%d",&temp);

  switch (ntype) {
  case 1:
    /* L1 norm */
    for (i=0; i<dim; i++) {
      printf("for i=%d vec1[i]=%f vec2[i]=%f \n",i,vec1[i],vec2[i]);
    //  scanf("%d",&temp);
      acc += abs(vec1[i]-vec2[i]);
    }
    break;
  case 2:
    /* L2 norm */
    for (i=0; i<dim; i++) {
      printf("for i=%d vec1[i]=%f vec2[i]=%f \n",i,vec1[i],vec2[i]);
     // scanf("%d",&temp);
      acc += (vec1[i]-vec2[i])*(vec1[i]-vec2[i]);
    }
    acc = sqrt(acc);
    break;
  default:
    fprintf(stderr, "Norm type %d is not implemented yet\n", ntype);
    exit(1);
  }
  return(acc);
}

/* perform backtrack */
int
backtrack(char **lat, int tpln, int tgtn)
{
  int *alitpl, *alitgt;
  int i, j, k;
  int alilen;

  alitpl = (int *)m_malloc(sizeof(int)*(tpln+tgtn));
  alitgt = (int *)m_malloc(sizeof(int)*(tpln+tgtn));

  k = 0;
  i = tpln-1; /* template frame */
  j = tgtn-1; /* target frame */
  while (0<i || 0<j) {
    if (i==tpln-1) {
      alitpl[k] = 0;
    } else {
      alitpl[k] = i;
    }
    alitgt[k] = j;
    switch (lat[j][i]) {
    case 1:
      j--;
      break;
    case 2:
      i--;
      j--;
      break;
    case 3:
      i--;
      break;
    default:
      fprintf(stderr, "Internal error in backtracking (1)\n");
      exit(1);
    }
    if (i<0 || j<0) {
      fprintf(stderr, "Internal error in backtracking (2)\n");
    }
    k++;
  }
  alilen = k;
  /*
  for (k=0; k<alilen-1; k++) {
    printf("%d %d\n", alitpl[alilen-k-1], alitgt[alilen-k-1]);
  }
  */
  printf("TPL:");
  for (k=0; k<alilen-1; k++) {
    printf(" %d", alitpl[alilen-k-1]);
  }
  printf("\n");
  printf("TGT:");
  for (k=0; k<alilen-1; k++) {
    printf(" %d", alitgt[alilen-k-1]);
  }
  printf("\n");
  m_free(alitpl);
  m_free(alitgt);
  return alilen;
}

/* DP matching main function */
int
dpmatch(DPDATA *templatep, FILE *tgtfp, DPDATA_HEAD *tgthp, DPOPT *dpoptp)
{
  int temp;
  float *accscr[2];
  int *acclen[2];
  int *acchmove[2], *accvmove[2];
  int *tgtstart[2];
  int tgtend[2];
  char **lat=NULL;
  //  float *tgtfrm;
  int tplsize, tgtsize; 
  int i,t;
  int j;
  int ff, ffnxt;
  float *fvec;
  float dist;

  tplsize = templatep->dpdh.nSamples;
  tgtsize = tgthp->nSamples;
  fvec = (float *)m_malloc(tgthp->sampSize);

  printf("tplsize=%d tgtsize=%d \n",tplsize,tgtsize);
 // scanf("%d",&temp);


  if (dpoptp->backtrack) {
    if (tgtsize <= 0) {
      fprintf(stderr, "Target length information is required to perform backtracking\n");
      exit(1);
    }
    lat=(char **)m_malloc(sizeof(char *)*(tgtsize+1));
    for (t=0; t<tgtsize+1; t++) {
      lat[t] = (char *)m_malloc(sizeof(char)*(tplsize+2));
      for (i=0; i<tplsize+2; i++) {
        lat[t][i] = 0;
      }
    }
  }
  for (t=0; t<2; t++) {
    accscr[t] = (float *)m_malloc(sizeof(float)*(tplsize+2));
    acclen[t] = (int *)m_malloc(sizeof(int)*(tplsize+2));
    acchmove[t] = (int *)m_malloc(sizeof(int)*(tplsize+2));
    accvmove[t] = (int *)m_malloc(sizeof(int)*(tplsize+2));
    tgtstart[t] = (int *)m_malloc(sizeof(int)*(tplsize+2));
  }

  /* initialize (t=0) */
  for (i=0; i<tplsize+2; i++) {
    accscr[0][i] = accscr[1][i] = FLT_MAX;
    acclen[0][i] = acclen[1][i] = 0;
    acchmove[0][i] = acchmove[1][i] = 0;
    accvmove[0][i] = accvmove[1][i] = 0;
    tgtstart[0][i] = tgtstart[1][i] = 0;
  }
  tgtend[0] = tgtend[1] = 0;
  accscr[0][0] = 0.0;

  /* main processing of DP */
  switch (dpoptp->printfrminfo) {
  case 0:
    break;
  case 1:
    printf("time, normalized score, start, end\n");
    break;
  case 2:
  case 3:
  default:
    printf("time, normalized score, start, end, len, hmove, vmove\n");
    break;
  }

  t=1; /* t is an index of time frame of target */
  ff=0;
  int loopcount=0;
  while (1) { /* loop for time frames of target */////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /* read one frame from target */
  printf("loopcount=%d \n",loopcount);
//scanf("%d",&temp);

    if (0<tgthp->nSamples) {
      if (tgthp->nSamples < t) {
        break;
      } 
      else {
        readdataframe(tgtfp, fvec, tgthp, dpoptp, "target data");
        int iii=0;
        for(iii=0;iii<tgthp->sampSize;iii++)
          printf("fvec[%d]=%f  \n",iii,fvec[iii]);
      }
    }
    else {
      readdataframe(tgtfp, fvec, tgthp, dpoptp, "target data");
      if (fvec == NULL) {
        break;
      }
      else{
        int iii=0;
        for(iii=0;iii<tgthp->sampSize;iii++)
          printf("fvec[%d]=%f  \n",iii,fvec[iii]);
      }
    }

    /* flip the array index */
    ffnxt = (ff+1)%2;

    i = 0; /* i is an index of time frame of template */
    /* horizontal move (exempted from horizontal move penalty for i=0) */
    printf("t=%d dpoptp->startrng=%d \n",t,dpoptp->startrng);
  //  scanf("%d",&temp);

    if (t-1 < dpoptp->startrng) {
      accscr[ffnxt][i] = accscr[ff][i];
      if (dpoptp->backtrack && (0 < tgtsize)) {
      lat[t][i] = 1; /* 1 indicates horizontal move */
      }
    } 
    else {
      accscr[ffnxt][i] = FLT_MAX;
    }
    /* initialize horizontal and vertical move counter */
    acchmove[ffnxt][i] = accvmove[ffnxt][i] = 0;
    /* initialize hypthesis length counter */
    acclen[ffnxt][i] = 0;
    printf("i=%d \n",i);
    printf("accscr[ff][i]=%f acclen[ff][i]=%d acchmove[ff][i]=%d accvmove[ff][i]=%d tgtstart[ff][i]=%d tgtend[ff]=%d \n",accscr[ff][i],acclen[ff][i],acchmove[ff][i],accvmove[ff][i],tgtstart[ff][i],tgtend[ff]);

    printf("accscr[ffnxt][i]=%f acclen[ffnxt][i]=%d acchmove[ffnxt][i]=%d accvmove[ffnxt][i]=%d tgtstart[ffnxt][i]=%d tgtend[ffnxt]=%d \n",accscr[ffnxt][i],acclen[ffnxt][i],acchmove[ffnxt][i],accvmove[ffnxt][i],tgtstart[ffnxt][i],tgtend[ffnxt]);

    for (i=1; i<=tplsize; i++) {/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      printf("\n ------------------------------------for-start--------------------------------------------------------\n \n ");
      printf("tplsizecount=%d \n",i);
    //  scanf("%d",&temp);

      dist = frmdist(templatep->data[i], fvec, templatep->dpdh.sampDim, dpoptp->ntype);

      printf("dist=%f \n",dist);
   //   scanf("%d",&temp);

      printf("acchmove[ff][i]=%d \n",acchmove[ff][i]);
  //    scanf("%d",&temp);

      /* horizontal move (initialize accs) */
      if (acchmove[ff][i] < dpoptp->maxhmove) {
      /* Assumes FLT_MAX + x = FLT_MAX */
        accscr[ffnxt][i] = accscr[ff][i] + dpoptp->hmovepenalty + dist;
        if (dpoptp->backtrack && (0 < tgtsize)) {
            lat[t][i] = 1; /* 1 indicates horizontal move */
        }
        printf("accscr[ffnxt][i]=%f \n",accscr[ffnxt][i]);
    //    scanf("%d",&temp);
      } 
      else {
        accscr[ffnxt][i] = FLT_MAX;
      }
      acclen[ffnxt][i] = acclen[ff][i] + 1;
      acchmove[ffnxt][i] = acchmove[ff][i] + 1;
      accvmove[ffnxt][i] = accvmove[ff][i];
      tgtstart[ffnxt][i] = tgtstart[ff][i];
          printf("accscr[ff][i]=%f acclen[ff][i]=%d acchmove[ff][i]=%d accvmove[ff][i]=%d tgtstart[ff][i]=%d tgtend[ff]=%d \n",accscr[ff][i],acclen[ff][i],acchmove[ff][i],accvmove[ff][i],tgtstart[ff][i],tgtend[ff]);

      printf("accscr[ffnxt][i]=%f acclen[ffnxt][i]=%d acchmove[ffnxt][i]=%d accvmove[ffnxt][i]=%d tgtstart[ffnxt][i]=%d \n",accscr[ffnxt][i],acclen[ffnxt][i],acchmove[ffnxt][i],accvmove[ffnxt][i],tgtstart[ffnxt][i]);
    //  scanf("%d",&temp);
      /* diagonal move (update accs) */
      if ((accscr[ff][i-1]+dist)/(acclen[ff][i-1]+1) < accscr[ffnxt][i]/acclen[ffnxt][i] ) {
        accscr[ffnxt][i] = accscr[ff][i-1] + dist;
        acclen[ffnxt][i] = acclen[ff][i-1] + 1;
        acchmove[ffnxt][i] = acchmove[ff][i-1];
        accvmove[ffnxt][i] = accvmove[ff][i-1];
        if (dpoptp->backtrack && (0 < tgtsize)) {
        lat[t][i] = 2; /* 2 indicates diagonal move */
        }
        if (i==1) {
          tgtstart[ffnxt][i] = t;
        } 
        else {
          tgtstart[ffnxt][i] = tgtstart[ff][i-1];
        }
      }
          printf("accscr[ff][i]=%f acclen[ff][i]=%d acchmove[ff][i]=%d accvmove[ff][i]=%d tgtstart[ff][i]=%d tgtend[ff]=%d \n",accscr[ff][i],acclen[ff][i],acchmove[ff][i],accvmove[ff][i],tgtstart[ff][i],tgtend[ff]);

      printf("accscr[ffnxt][i]=%f acclen[ffnxt][i]=%d acchmove[ffnxt][i]=%d accvmove[ffnxt][i]=%d tgtstart[ffnxt][i]=%d \n",accscr[ffnxt][i],acclen[ffnxt][i],acchmove[ffnxt][i],accvmove[ffnxt][i],tgtstart[ffnxt][i]);
    //  scanf("%d",&temp);
      /* vertical move (update accs) */
      if ((accscr[ffnxt][i-1] + dpoptp->vmovepenalty + dist)/(acclen[ffnxt][i-1]+1) < accscr[ffnxt][i]/acclen[ffnxt][i]) {
        if (accvmove[ffnxt][i-1] < dpoptp->maxvmove) {
          accscr[ffnxt][i] = accscr[ffnxt][i-1] + dpoptp->vmovepenalty + dist;
          if (dpoptp->backtrack && (0 < tgtsize)) {
            lat[t][i] = 3; /* 3 indicates vertical move */
          }
          acclen[ffnxt][i] = acclen[ffnxt][i-1] + 1;
          acchmove[ffnxt][i] = acchmove[ffnxt][i-1];
          accvmove[ffnxt][i] = accvmove[ffnxt][i-1] + 1;
          if (i==1) {
            tgtstart[ffnxt][i] = t;
          } 
          else {
          tgtstart[ffnxt][i] = tgtstart[ffnxt][i-1];
          }
        }
      }
          printf("accscr[ff][i]=%f acclen[ff][i]=%d acchmove[ff][i]=%d accvmove[ff][i]=%d tgtstart[ff][i]=%d tgtend[ff]=%d \n",accscr[ff][i],acclen[ff][i],acchmove[ff][i],accvmove[ff][i],tgtstart[ff][i],tgtend[ff]);

      printf("accscr[ffnxt][i]=%f acclen[ffnxt][i]=%d acchmove[ffnxt][i]=%d accvmove[ffnxt][i]=%d tgtstart[ffnxt][i]=%d \n",accscr[ffnxt][i],acclen[ffnxt][i],acchmove[ffnxt][i],accvmove[ffnxt][i],tgtstart[ffnxt][i]);
    //  scanf("%d",&temp);
      printf("\n-------------------------------------for-end---------------------------------------------------------\n \n");
    }//for

    i = tplsize + 1;
    /* horizontal move (exempted from horizontal move penalty) */
    if (dpoptp->endrng==0) {
      accscr[ffnxt][i] = FLT_MAX;
    } 
    else {
      if (dpoptp->endrng <= t) {
        accscr[ffnxt][i] = accscr[ff][i];
        if (dpoptp->backtrack && (0 < tgtsize)) {
        lat[t][i] = 1; /* 1 indicates horizontal move */
        }
      } 
      else {
        accscr[ffnxt][i] = FLT_MAX;
      }
    }
    acclen[ffnxt][i] = acclen[ff][i];
    acchmove[ffnxt][i] = acchmove[ff][i];
    accvmove[ffnxt][i] = accvmove[ff][i];
    tgtstart[ffnxt][i] = tgtstart[ff][i];
    tgtend[ffnxt] = tgtend[ff];
        printf("accscr[ff][i]=%f acclen[ff][i]=%d acchmove[ff][i]=%d accvmove[ff][i]=%d tgtstart[ff][i]=%d tgtend[ff]=%d \n",accscr[ff][i],acclen[ff][i],acchmove[ff][i],accvmove[ff][i],tgtstart[ff][i],tgtend[ff]);

    printf("accscr[ffnxt][i]=%f acclen[ffnxt][i]=%d acchmove[ffnxt][i]=%d accvmove[ffnxt][i]=%d tgtstart[ffnxt][i]=%d tgtend[ffnxt]=%d \n",accscr[ffnxt][i],acclen[ffnxt][i],acchmove[ffnxt][i],accvmove[ffnxt][i],tgtstart[ffnxt][i],tgtend[ffnxt]);
 //   scanf("%d",&temp);
    /* vertical move (Not subject to the vertical move penalty) */
    if (accscr[ffnxt][i-1]/acclen[ffnxt][i-1] < accscr[ffnxt][i]/acclen[ffnxt][i]) {
      accscr[ffnxt][i] = accscr[ffnxt][i-1];
      acclen[ffnxt][i] = acclen[ffnxt][i-1];
      acchmove[ffnxt][i] = acchmove[ffnxt][i-1];
      accvmove[ffnxt][i] = accvmove[ffnxt][i-1];
      tgtstart[ffnxt][i] = tgtstart[ffnxt][i-1];
      tgtend[ffnxt] = t;
      if (dpoptp->backtrack && (0 < tgtsize)) {
      lat[t][i] = 3; /* 3 indicates vertical move */
      }
    }
        printf("accscr[ff][i]=%f acclen[ff][i]=%d acchmove[ff][i]=%d accvmove[ff][i]=%d tgtstart[ff][i]=%d tgtend[ff]=%d \n",accscr[ff][i],acclen[ff][i],acchmove[ff][i],accvmove[ff][i],tgtstart[ff][i],tgtend[ff]);

    printf("accscr[ffnxt][i]=%f acclen[ffnxt][i]=%d acchmove[ffnxt][i]=%d accvmove[ffnxt][i]=%d tgtstart[ffnxt][i]=%d,tgtend[ffnxt]=%d \n",accscr[ffnxt][i],acclen[ffnxt][i],acchmove[ffnxt][i],accvmove[ffnxt][i],tgtstart[ffnxt][i],tgtend[ffnxt]);
  //  scanf("%d",&temp);


    switch (dpoptp->printfrminfo) {
    case 0:
      break;
    case 1:
      if (accscr[ffnxt][i] < FLT_MAX) {
        printf("%d %e %d %d\n", t, accscr[ffnxt][i]/acclen[ffnxt][i], tgtstart[ffnxt][i], tgtend[ffnxt]);
      } 
      else {
        printf("%d NaN - -\n", t);
      }
      break;
    case 2:
      if (accscr[ffnxt][i] < FLT_MAX) {
        printf("%d %e %d %d %d %d %d\n", t, accscr[ffnxt][i]/acclen[ffnxt][i], tgtstart[ffnxt][i], tgtend[ffnxt],
          acclen[ffnxt][i], acchmove[ffnxt][i], accvmove[ffnxt][i]);
      } 
      else {
        printf("%d NaN - - - - - -\n", t);
      }
      break;
    case 3:
    default:
      if (accscr[ffnxt][i] < FLT_MAX) {
        printf("%d %e %d %d %d %d %d\n", t, accscr[ffnxt][i]/acclen[ffnxt][i], tgtstart[ffnxt][i], tgtend[ffnxt], 
	       acclen[ffnxt][i], acchmove[ffnxt][i], accvmove[ffnxt][i]);
        printf("FDMP");
        for (j=0; j<=tplsize+1; j++) {
          printf(" %e", accscr[ffnxt][i]/acclen[ffnxt][i]);
        }
      } 
      else {
        printf("%d NaN - - - - - -\n", t);
        printf("FDMP -- ");
      }
      printf("\n");
      break;
    }//switch
    t++;
    ff=ffnxt;
    loopcount++;
    printf("\n------------------------------------------while end--------------------------\n \n ");
  }//while

  if (dpoptp->backtrack) {
    int iii,kkk;
    for(iii=0;ikk<tgtsize+1;iii++){
      for(kkk=0;kkk<tplsize+1){
        printf("lat[%d][%d]=%s \n",lat[iii][kkk]);
      }
      printf("\n");
    }
    backtrack(lat, tplsize+2, tgtsize+1);
    for (t=0; t<tgtsize+1; t++) {
      m_free(lat[t]);
    }
    m_free(lat);
  }

  m_free(fvec);
  m_free(accscr[0]);
  m_free(accscr[1]);
  m_free(acclen[0]);
  m_free(acclen[1]);
  m_free(acchmove[0]);
  m_free(acchmove[1]);
  m_free(accvmove[0]);
  m_free(accvmove[1]);
  return 0;
}

int
init_dpopt(DPOPT *dpoptp)
{
  dpoptp->startrng = MAXINT; /* start position is between 0 <= t <= startrng */
  dpoptp->endrng = 1;        /* end position is between N <= t <= end of target. N=0 means end of target */
  dpoptp->maxhmove = 10;     /* maximum allowd horizontal (target axis) move in the lattice */
  dpoptp->maxvmove = 10;     /* maximum allowd vertical (template axis) move in the lattice */
  dpoptp->hmovepenalty = 0;  /* penalty score for horizontal move */
  dpoptp->vmovepenalty = 0;  /* penalty score for vertical move */
  dpoptp->backtrack = 0;     /* flag to perform backtracking */
  dpoptp->outdirname = "";   /* path to output directoy */
  dpoptp->dformat = "htk";   /* data format */
  dpoptp->printfrminfo = 1;   /* print frame info level */
  dpoptp->ntype = 2;         /* norm type for distance evaluation */
  return 0;
}

void
usage(char *myname, DPOPT *dpoptp)
{
  printf("%s [options] [template [target...]]\n", myname);
  printf("\n");
  printf("Option                                              Defalut\n");
  printf("\n");
  printf(" -s N     :set start range to [0 N]                 %d\n", dpoptp->startrng);
  printf(" -e N     :set end range to [N end]                 %d\n", dpoptp->endrng);
  printf(" -h N     :set max horizontal move to N             %d\n", dpoptp->maxhmove);
  printf(" -v N     :set max vertical move to N               %d\n", dpoptp->maxvmove);
  printf(" -p f     :set h-move penalty to f                  %f\n", dpoptp->hmovepenalty);
  printf(" -q f     :set v-move penalty to f                  %f\n", dpoptp->vmovepenalty);
  printf(" -b N     :set backtracking flag to N (0 or 1)      %d\n", dpoptp->backtrack);
  //  printf(" -o d     :set output dir to d                      %s\n", dpoptp->outdirname);
  printf(" -n N     :set norm type to N (1 or 2)              %d\n", dpoptp->ntype);
  printf(" -f N     :set print frame info level to N          %d\n", dpoptp->printfrminfo);
  printf(" -w N     :set warning level to d                   %d\n", warnlevel);
  printf(" -S f     :set script file to f                     %s\n", "");
  printf(" -F s     :set data format to fmt                   %s\n", dpoptp->dformat);
  printf(" -?       :show usage\n");
  exit(0);
}

void
printsettings(DPOPT *dpoptp)
{
  printf("start range: %d\n", dpoptp->startrng);
  printf("end range: %d\n", dpoptp->endrng);
  printf("max horizontal move: %d\n", dpoptp->maxhmove);
  printf("max vertical move: %d\n", dpoptp->maxvmove);
  printf("h-move penalty: %f\n", dpoptp->hmovepenalty);
  printf("v-move penalty: %f\n", dpoptp->vmovepenalty);
  printf("backtracking flag: %d\n", dpoptp->backtrack);
  printf("norm type: L%d\n", dpoptp->ntype);
  printf("frame info level %d\n", dpoptp->printfrminfo);
  printf("data format: %s\n", dpoptp->dformat);
}

int main(int argc, char **argv){
  int c;
  FILE *scpfp=NULL;
  FILE *tgtfp=NULL;
  char *tgtid=NULL;
  DPOPT dpopt;
  char buf[BUFSIZE];
  DPDATA *template=NULL, *lc_template=NULL;
  DPDATA_HEAD tgth;
  char *bufp1, *bufp2;
  char *templatefname;

  checkrequirements();

  if (argc==1) {
    usage(argv[0], &dpopt);
    exit(0);
  }
  init_dpopt(&dpopt);

  while (1){
    c = getopt(argc, argv, "s:e:h:v:p:q:b:n:f:w:S:F:?");
    if (c == -1) break;
    switch (c) {
    case 's':
      dpopt.startrng = atoi(optarg);
      break;
    case 'e':
      dpopt.endrng = atoi(optarg);
      break;
    case 'h':
      dpopt.maxhmove = atoi(optarg);
      break;
    case 'v':
      dpopt.maxvmove = atoi(optarg);
      break;
    case 'p':
      dpopt.hmovepenalty = atof(optarg);
      break;
    case 'q':
      dpopt.vmovepenalty = atof(optarg);
      break;
    case 'b':
      dpopt.backtrack = atoi(optarg);
      break;
    case 'n':
      dpopt.ntype = atoi(optarg);
      break;
    case 'f':
      dpopt.printfrminfo = atoi(optarg);
      break;
    case 'w':
      warnlevel = atoi(optarg);
      break;
    case 'S':
      scpfp = m_fopen(optarg, "r");
      break;
    case 'F':
      dpopt.dformat = optarg;
      break;
    case '?':
      usage(argv[0], &dpopt);
      exit(0);
      break;
    default:
      printf ("Error: Unknown option %c\n", c);
      usage(argv[0], &dpopt);
      exit(1);
    }
  }//while

  printsettings(&dpopt);

  if (0<argc-optind ) {
    /* read template data */
    template=readdataf(argv[optind], &dpopt);
    templatefname = argv[optind];

    for (optind++; optind<argc; optind++) {
      /* process a target file specified in the command line */
      tgtfp = m_fopen(argv[optind], "r");
      tgtid = basename(argv[optind]); /* (assuming GNU version of basename) */
      readdatahead(tgtfp, &tgth, &dpopt, tgtid);
      printf("#\n");
      printf("template: %s, length = %d\n", templatefname, template->dpdh.nSamples);
      printf("target: %s, length = %d\n", argv[optind], tgth.nSamples);
      tgth.id = tgtid;
      dpmatch(template, tgtfp, &tgth, &dpopt);
      m_fclose(tgtfp);
      printf("##\n");
    }//for
  }//if

  /* process target files specified in the scp list */
  if (scpfp!=NULL) {
    while (!feof(scpfp)) {
      if (fgets(buf, BUFSIZE, scpfp) == NULL) {
	      break;
      }
      if (strlen(buf) == BUFSIZE-1) {
        fprintf(stderr, "path name is too long: %s\n", buf);
        exit(1);
      }
      if (buf[strlen(buf)-1] == '\n') {
        buf[strlen(buf)-1] = '\0';
      }
      bufp1 = strtok(buf, " \t");
      bufp2 = strtok(NULL, " \t");
      if (bufp1 == NULL) {
        continue;
      } 
      else if (bufp2 ==NULL) {
        if (template==NULL) {
          fprintf(stderr, "No template is specified\n");
          exit(1);
        }
        printf("#\n");
        printf("template: %s, length = %d\n", templatefname, template->dpdh.nSamples);
        tgtfp = m_fopen(bufp1, "r");
        tgtid = basename(bufp1); /* (assuming GNU version of basename) */
        readdatahead(tgtfp, &tgth, &dpopt, tgtid);
        printf("target: %s, length = %d\n", bufp1, tgth.nSamples);
        tgth.id = tgtid;
        dpmatch(template, tgtfp, &tgth, &dpopt);
        m_fclose(tgtfp);
        printf("##\n");
      }
      else {
        lc_template=readdataf(bufp1, &dpopt);
        printf("#\n");
        printf("template: %s, length = %d\n", bufp1, lc_template->dpdh.nSamples);
        tgtfp = m_fopen(bufp2, "r");
        tgtid = basename(bufp2); /* (assuming GNU version of basename) */
        readdatahead(tgtfp, &tgth, &dpopt, tgtid);
        printf("target: %s, length = %d\n", bufp2, tgth.nSamples);
        tgth.id = tgtid;
        dpmatch(lc_template, tgtfp, &tgth, &dpopt);
        m_fclose(tgtfp);
        free_dpdata(lc_template);
        printf("##\n");
      }
    }
  }
  if (template != NULL) {
    free_dpdata(template);
  }

  return 0;
}
