#!/bin/sh
#$ -S /bin/sh
#Version1.0	  hewm@genomics.cn	  2023-04-10
echo Start Time : 
date
GCC=g++

echo  Detection system platform  is
if [ "$(uname)" == "Darwin" ];then
	echo MacOS
	sed 's/memrchr/memchr/g'  include/json_parser/subbuffer.h >  include/json_parser/subbuffer.h.tmp
	mv include/json_parser/subbuffer.h.tmp include/json_parser/subbuffer.h
	which "g++" >/dev/null 2>&1
	if [ $? -eq 0 ];
	then
		echo We recommend compiling this program with g++ > [GCC 4.8] on macOS
	else
		echo We recommend compiling this program with g++ > [GCC 4.8] on macOS
		echo you should install the [g++ >= GCC 4.8],and then re-install me
	fi
	# Mac OS
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ];then
	echo Linux
	# GNU/Linux
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ];then
	echo windows sys no run this
	# Windows NT
fi


echo $GCC  -std=c++11  -fvisibility=hidden -fvisibility-inlines-hidden   -fopenmp -pthread -static-libgcc -static-libstdc++ 	  -g	  -O3      VCF2PCACluster.cpp 	  -lz	  -L/usr/lib/	  -L./include/zlib/	  -L	  /hwfssz4/BC_PUB/Software/01.Usr/lib/	  -o	  ../bin/VCF2PCACluster 	  -lm	  -lc
$GCC  -std=c++11  -fvisibility=hidden -fvisibility-inlines-hidden   -fopenmp -pthread -static-libgcc -static-libstdc++ 	  -g	  -O3      VCF2PCACluster.cpp 	  -lz	  -L/usr/lib/	  -L./include/zlib/	  -L	  /hwfssz4/BC_PUB/Software/01.Usr/lib/	  -o	  ../bin/VCF2PCACluster 	  -lm	  -lc
chmod 755 ../bin/*
echo see bin at [ ../bin/VCF2PCACluster ]
echo End Time : 
date
