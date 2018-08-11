#! /bin/bash
#Jan 13 2017
#Qian
date
filename=$1

find ./program -type f -not \( -name '*c' -or -name '*cpp' \) -delete
for file in `ls program`
do
	echo impiling $file
	g++ ./program/$file -o ./program/${file%%.*}
done
wait

./program/mfccb2mfccf ./matlabmfcc/${filename}.mfcc ./matlabmfcc/${filename}.mfc
date
./program/mfcc2dnninput ./matlabmfcc/${filename}.mfc ./config/0208nn.xnorm ./mfcc/${filename}.din
date
echo "DNN calculating ..."
./program/dnncalculate ./mfcc/${filename}.din ./config/0208mlp.final ./config/0208dnnconf ./dnn/${filename}.dnn
date 1
./program/dnn2verilog ./dnn/${filename}.dnn ./verilogin/${filename}.vdnn
date
./program/makeverilogweightnoextra ./config/0208mlp.final 1024 ./coe/${filename}.coe
date
echo "transfering DNN to DTW ..."
./program/dnnout2dtwin ./dnn/${filename}.dnn ./dtw/${filename}.dtw ./config/0208dnnconf
date