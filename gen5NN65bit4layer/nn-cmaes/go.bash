#! /bin/bash -e

export nntop=$1

. ${nntop}/data.conf
#. ./geneconf_test
time ${nntop}/nn.bash 

echo done
