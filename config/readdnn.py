import numpy as np
import chainer
from chainer import cuda, Function, gradient_check, report, training, utils, Variable
from chainer import datasets, iterators, optimizers, serializers
from chainer import Link, Chain, ChainList
import chainer.functions as F
import chainer.links as L
from chainer.training import extensions
from chainer import Variable as V
from numpy import array as ar
from chainer.functions import squared_difference as dist
import pickle
import gc
import time
layer=[60,70,56,69,30]
layernumber=4

class DNN(Chain):
	def __init__(self):
		super(DNN, self).__init__()
		with self.init_scope():
			self.l=[]
			for i in range(layernumber):
				self.l.append(L.Linear(layer[i], layer[i+1]))
	def __call__(self, x):
		y=0
		return y

dnn=DNN()
file=open('test3mlp.final')
file_line=file.readline()
ltemp=file_line.split()
for k in range (layernumber):
	file_line=file.readline()
	ltemp=file_line.split()
	while(ltemp[0]!='['):
		file_line=file.readline()
		ltemp=file_line.split()
	weight=[[0 for i in range(layer[k])] for i in range(layer[k+1])]
	weight=ar(weight,dtype=np.float32)
	for i in range(layer[k+1]):
		file_line=file.readline()
		ltemp=file_line.split()
		for j in range(layer[k]):
			weight[i][j]=float(ltemp[j])
	file_line=file.readline()
	ltemp=file_line.split()
	bias=[0 for i in range(layer[k+1])]
	bias=ar(bias,dtype=np.float32)
	for i in range(layer[k+1]):
		bias[i]=float(ltemp[i+1])
	dnn.l[k].W.data=weight
	dnn.l[k].b.data=bias


with open('dnn.pkl','wb') as f:
	pickle.dump(dnn,f)









