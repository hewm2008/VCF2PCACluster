#!/bin/sh
#$ -S /bin/sh
#Version1.0	 hewm@genomics.org.cn	 2022-07-03
echo Start Time : 
date

### run without pop.info
#../../bin/VCF2PCACluster	 -InVCF	 Khuman.vcf.gz	 -OutPut	 OUT	 


### run with  pop.info  cluster by EMGaussian
../../bin/VCF2PCACluster	 -InVCF	 Khuman.vcf.gz	 -OutPut	 OUT1	 -InSampleGroup	 pop.info	 -ClusterMethod  EM 

### run with  pop.info  cluster by Kmean
../../bin/VCF2PCACluster	 -InVCF	 Khuman.vcf.gz	 -OutPut	 OUT2	 -InSampleGroup	 pop.info	  -ClusterMethod Kmean

### run with  pop.info  cluster by  DBSCAN
 #  (Input the kinship file without reading the vcf file to improve calculation efficiency  #
../../bin/VCF2PCACluster	 -InKinship	 OUT1.Normalized_IBS.Kinship	 -OutPut	 OUT3	 -InSampleGroup	 pop.info	  -ClusterMethod DBSCAN


echo End Time : 
date
