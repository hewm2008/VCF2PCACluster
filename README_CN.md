
# VCF2PCACluster
A new simple and efficient software to PCA and Cluster For popolation VCF File

###  1 Introduction
<b>VCF2PCACluster</b>  是于基于VCF开发的PCA分析和聚类软件，同时兼并了Genotype 等格式软件。
即只要对应的一个输入文件进来，这PCA和作图分组等一位到位。
</br>
</br> keyword  : VCF2PCA ;&nbsp;&nbsp;&nbsp;&nbsp;    VCF2Kinship ;&nbsp;&nbsp;&nbsp;&nbsp; cluster;&nbsp;&nbsp;&nbsp;&nbsp; k-means ;&nbsp;&nbsp;&nbsp;&nbsp;EMGaussian &nbsp;&nbsp;&nbsp;&nbsp; DBSCAN ;&nbsp;&nbsp;&nbsp;&nbsp;

</br>亮点：
</br>1  The result is the same with [tassel](https://www.maizegenetics.net/tassel),[gapit](https://zzlab.net/GAPIT/) and [gcta](https://yanglab.westlake.edu.cn/software/gcta/#Overview) , just the difference in precision.
</br>2  功能有 1 5种kinship矩阵 2  PCA结果 3  聚类结果  和4 以cluster染色并作图。
</br>3  一个VCF输入，一步到位，方便用户使用.
</br>4  边读边算，内存剥离受位点多少的影响（时空组是剥离受基因数量多少的影响），内存只受样品量影响，故上100k的样品当也行，在这个基础上开发时空细胞PCA和聚类,虽然时空组学上主要是样品多。（80K 60G内存）
</br>5  Kmean聚类和DBSCAN聚类。   Keman分析，并找出最佳K值，和Structure和K值一样. 作图以此染色。
</br>6  提作作图小脚本，可以用这个脚本优化作图等。

</br>

</br>程序是给一些有基础的生信朋友用的，若是小白看不懂就算了。
</br>
</br><b>VCF2PCACluster</b> VCF2PCA is a PCA analysis software format developed based on VCF. It also incorporates Genotype, etc., That is, as long as the input is satisfied, the PCA and the cluster group are of the same output.


###  2 Download and Install
------------
The <b>new version</b> will be updated and maintained in <b>[hewm2008/VCF2PCACluster](https://github.com/hewm2008/VCF2PCACluster)</b>, please click below website to download the latest version
</br><p align="center"><b>[hewm2008/VCF2PCACluster](https://github.com/hewm2008/VCF2PCACluster)</b></p>

<b> 2.1. linux/MaxOS&nbsp;&nbsp;&nbsp;   [Download](https://github.com/hewm2008/VCF2PCACluster/archive/v1.40.tar.gz)</b>
  
  </br> <b>2.2 Pre-install</b>
  </br> VCF2PCACluster is for Linux/Unix/macOS only.
  </br>Before installing,please make sure the following pre-requirements are ready to use.
  </br> 1) [OpenMP c/c++](https://www.openmp.org/wp-content/uploads/OpenMP3.1-CCard.pdf) command is recommended to be pre-installed
  </br> 2) g++   : g++ with [--std=c++11](https://gcc.gnu.org/) > 4.8+ is recommended
  </br> 3) zlib  : [zlib](https://zlib.net/) > 1.2.3 is recommended
  </br> 4) R     : [R](https://www.r-project.org/) with [ggplot](http://ggplot.yhathq.com/) is recommended

</br> <b>2.3 Install</b>
</br> Users can install it with the following options:
</br> Option 1，we provide a static version for Linux/Unix 
<pre>
        git clone https://github.com/hewm2008/VCF2PCACluster.git
        cd VCF2PCACluster;	chmod 755 -R bin/*
        ./bin/VCF2PCACluster  -h 
</pre>

</br> Option 2:    Linux/Unix/macOS
<pre>
        git clone https://github.com/hewm2008/VCF2PCACluster.git
	cd VCF2PCACluster ; chmod 755 configure  ;  ./configure;
        make;   # sh make.sh 
        mv VCF2PCACluster  bin/;    #     [rm *.o]
</pre>

**Note:** For <b>macOS</b> , user can run the following command first
  </br><i>ln -s  /opt/homebrew/bin/g++-11     /opt/homebrew/bin/g++  ;   export  PATH=/opt/homebrew/bin/:$PATH</i>



###  3 Parameter description
------------
</br><b>3.1 VCF2PCACluster</b>
</br><b>3.1.1 Main parameter</b>

```php

	Usage: VCF2PCACluster  -InVCF  in.vcf.gz  -OutPut outPrefix [options]

		-InVCF         <str>      Input SNP VCF Format
		-InGenotype    <str>      InPut Genotype File
		-InKinship     <str>      Input SNP K Kinship File Format
		-OutPut        <str>      OutPut File Prefix(Kinship PCA etc)


		-KinshipMethod <int>      Method of Kinship [1-5],defaut [1]
		                          1:Normalized_IBS[(Yang/BaldingNicolsKinship]
		                          2:Centered_IBS(VanRaden)
		                          3:IBSKinshipImpute 4:IBSKinship 5:p_dis
		-ClusterMethod <str>      Method For Cluster[EM/Kmean/DBSCAN] [EM]

		-help                     Show more Parameters and help [hewm2008]


```
</br> brief description for function:
<pre>
	   #   用法一看即明，最基础的为 一个输入和输出即可 
	   #    更多说明后面将在知乎更新

          ### run without pop.info
          #   VCF2PCACluster	-InVCF	Khuman.vcf.gz	-OutPut	OUT
          ### run with  pop.info
        VCF2PCACluster	-InVCF	Khuman.vcf.gz	-OutPut	OUT	-InSampleGroup	pop.info 

</pre>

</br><b>3.1.2  Detail parameters</b>
```php

	More Help document please see the pdf/doc help	Para [-i] is show for [-InVCF], Para [-o] is show for [-OutPut]


	Usage: VCF2PCACluster  -InVCF in.vcf.gz  -OutPut outPrefix [options]

		-InVCF         <str>      Input SNP VCF Format
		-InKinship     <str>      Input SNP K Kinship File Format
		-OutPut        <str>      OutPut File Prefix(Kinship PCA etc)

		-KinshipMethod <int>      Method of Kinship [1-5],defaut [1]
		                          1:Normalized_IBS(Yang/BaldingNicolsKinship)
		                          2:Centered_IBS(VanRaden)
		                          3:IBSKinshipImpute 4:IBSKinship 5:p_dis
		-ClusterMethod <str>      Method For Cluster[EM/Kmean/DBSCAN] [EM]

		-help          v1.40      Show more Parameters and help [hewm2008]

	    InFile:
		-InGenotype    <str>      InPut Genotype File for no VCF file
		-InSubSample   <str>      Only keep samples from subsample List for PCA[ALLsample]
		-InSampleGroup <str>      InFile of sample Group info,format(sample groupA)

	    SNP Filtering:
		-MAF           <float>    Min minor allele frequency filter [0.001]
		-Miss          <float>    Max ratio of miss allele filter [0.25]
		-Het           <float>    Max ratio of het allele filter [1.00]
		-HWE           <float>    Exact test of Hardy-Weinberg Equilibrium for SNP Pvalue[0]
		-Fchr          <str>      Filter the chrX chr[chrX,chrY,X,Y]
		-KeepRemainVCF            keep the VCF after filter

	    Clustering:
		-RandomCenter             Random diff-center to Re-Run Cluster for Kmean
		-BestKManually <int>      manually set the Best K (Num of Cluster) (auto)
		-BestKRatio    <float>    Get the best K Cluster by deta-SSE Ratio[0.15]
		-MinPointNum   <int>      Minimum point number of D-cluster[4]
		-Epsilon       <float>    Epsilon for DBSCAN_Distance/EM_convergence (auto)
		-Iterations    <int>      iterations number for EM clustering[1000]

	    OutPut:
		-PCnum         <int>      Num of PC eig [10]



```

</br><b>3.2.2 Other parameters</b>
</br> program also provides the graphing software perl  script (R:ggplot2), the brief parameters of the graphing script are as follows

```php
ploteig  -h

	Version:1.40         hewm2008@gmail.com

	Usage: ploteig  -InFile  pca.eigenvec -OutPut Fig


		Options

		-InFile       <s>   : InPut File of PCA
		-OutPut       <s>   : OutPut file prefix

		-help               : Show more help [hewm2008]

		-ShowEval           : Show eval%(PC percentages) on the fig
		-ColShap            : colour <=> shape for cluster or subpop
		-columns      <s>   : the columns to plot a:b [3:4]
		-border       <i>   : how to plot the border (1,2,4,8,3,31) [3]
		-title        <s>   : title (legend) [PCA]
		-keystyle     <s>   : put key at top right  default(in) [outside]box [outside]
		-pointsize    <i>   : point size for plot [3]

		-BinDir       <s>   : The Bin Dir of gnuplot/R/ps2pdf/convert [$PATH]

```

</br><b>3.3 Output files</b>


|Module |    outFlie    |       Description                                                |
|:-----:|:-------------------|:------------------------------------------------------------|
| List  |                    |                                                             |
|       |out.kinship         |输出的亲缘矩阵，各样品的两两关系                             |
|       |out.eigenvec        |输出最优聚类和pca结果                                        |
|       |out.eigenval        |输出pca结果的特征向量                                        |
|       |out.PC1_PC2.pdf     |输出按cluster染色后的pc 1 2图                               |
|       |out.PC1PC2PC3.pdf   |输出按cluster染色后的pc 1 2 3图                               |


示例图见上面应用场景给的图。示例图和格式当一看即明，相关图可以见example 1 和2


###  4 Example
------------

</br>See more detailed usage in the&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <b>[Chinese Documentation](https://github.com/hewm2008/VCF2PCACluster/blob/main/Manual_CN.pdf)</b>
</br>See more detailed usage in the&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <b>[English Documentation](https://github.com/hewm2008/VCF2PCACluster/blob/main/Manual_EN.pdf)</b>
</br>See the example directory and  Manual.pdf for more detail.
</br>具体见这儿  Manual.pdf for more detail 里面的示意数据和脚本，后期将在某些网址释放一些教程
</br></br> 
../../bin/VCF2PCACluster      -InVCF        in.vcf.gz     -OutPut outPrefix
</br>  目录  Example/example*/　里面有输入和输出和脚本用法。


* Example 1)千人VCF重测序SNP基因型
</br> 共从K 人数据chr22  dbsnp里面随机挑出了1194个位点，挑 CEU（49） ， CHB（46） ， JPT（56）和 YRI （52）共203 个样品来分析。
</br>EM Gaussian 默认参数的PC1 PC2结果
</br>![PC12.png](https://github.com/hewm2008/VCF2PCACluster/blob/main/example/Example1/PC12.png)
</br>EM Gaussian 默认参数的PC1 PC3结果
</br>![PC13.png](https://github.com/hewm2008/VCF2PCACluster/blob/main/example/Example1/PC13.png)
</br>EM Gaussian 默认参数的PCA 3D 结果
</br>![PC3D.png](https://github.com/hewm2008/VCF2PCACluster/blob/main/example/Example1/PC3D.png)



* Example 2) 千人数据测试  

</br> To test the accuracy and the efficiency of VCF2PCACluster, we used data of this web site(https://ftp.1000genomes.ebi.ac.uk/vol1/ftp/release/20130502) to test follow software, only used the chr22 (minimal chromosome SNP database)  (2504 sample with 1,055,401 SNP numbers)to test these software.
</br> the result is accuracy with [tassel](https://www.maizegenetics.net/tassel) and [gcta64](https://yanglab.westlake.edu.cn/software/gcta/#Overview), see more at the  Manual.pdf. waiting time  ~12.5min at 8 threads , Memory is less than 2M

<pre>
echo Start Time :
date
##   download the really data  ###
#wget  -c https://ftp.1000genomes.ebi.ac.uk/vol1/ftp/release/20130502/ALL.chr22.phase3_shapeit2_mvncall_integrated_v5b.20130502.genotypes.vcf.gz
#wget  -c https://ftp.1000genomes.ebi.ac.uk/vol1/ftp/release/20130502/integrated_call_samples_v3.20130502.ALL.panel
# cut -f  1,3   integrated_call_samples_v3.20130502.ALL.panel  > sample.group ;gzip sample.group
time	../../bin/VCF2PCACluster  -InVCFALL.chr22.phase3_shapeit2_mvncall_integrated_v5b.20130502.genotypes.vcf.gz	-InSampleGroup sample.group.gz 	-OutPut	OUT1
##  to re-set the best K (4--->5)
time	../../bin/VCF2PCACluster  -InKinship OUT1.Normalized_IBS.Kinship -InSampleGroup sample.group.gz 	-OutPut	OUT2   -BestKManually  5
echo End Time :
date
</pre>
</br> PCA and cluster Result ,the correlation coefficient for  group and clustering is <b>0.995</b>  (R:cor);
</br> <center>![ALL_chr22.png](https://github.com/hewm2008/VCF2PCACluster/blob/main/example/Example2/ALL_chr22.png) </center>


###  5 Advantages

</br>速度快，少内存    fast speed, low memory
</br>简明易用    Simple and easy to use  (-i  -o)
</br>免安装   Free installation
</br>一步到图    only one step from vcf  to final drawing

###  6 Discussing
------------
- [:email:](https://github.com/hewm2008/VCF2PCACluster) hewm2008@gmail.com / hewm2008@qq.com
- join the<b><i> QQ Group : 125293663</b></i>

######################swimming in the sky and flying in the sea #############################

