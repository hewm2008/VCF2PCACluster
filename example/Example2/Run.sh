#!/bin/sh
#$ -S /bin/sh
#Version1.0	hewm@genomics.org.cn	2023-03-04
echo Start Time : 
date
##   download the really data  ###
#wget  -c https://ftp.1000genomes.ebi.ac.uk/vol1/ftp/release/20130502/ALL.chr22.phase3_shapeit2_mvncall_integrated_v5b.20130502.genotypes.vcf.gz 
#wget  -c https://ftp.1000genomes.ebi.ac.uk/vol1/ftp/release/20130502/integrated_call_samples_v3.20130502.ALL.panel
# cut -f  1,3   integrated_call_samples_v3.20130502.ALL.panel   > sample.group ; gzip  sample.group
time	../../bin/VCF2PCACluster  -InVCF	ALL.chr22.phase3_shapeit2_mvncall_integrated_v5b.20130502.genotypes.vcf.gz	-InSampleGroup sample.group.gz  	-OutPut	OUT1
##  to re-set the best K (4--->5)
time	../../bin/VCF2PCACluster  -InKinship OUT1.Normalized_IBS.Kinship -InSampleGroup sample.group.gz 	-OutPut	OUT2   -BestKManually  5
echo End Time : 
date
