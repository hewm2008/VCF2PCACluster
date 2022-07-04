
# MingPCACluster
A new simple and efficient software to PCA and Cluster For popolation VCF File or STOmics gem File 

###  1 Introduction
<b>MingPCACluster</b>  是于基于VCF开发的PCA分析和聚类软件，同时兼并了Genotype 等格式软件，同时开发针对时空单细胞表达量的格式（xx.gem.gz）文件（beta功能）。
即只要对应的一个输入文件进来，这PCA和作图分组等一位到位。
</br>
</br> keyword  : VCF2PCA ;&nbsp;&nbsp;&nbsp;&nbsp;    VCF2Kinship ;&nbsp;&nbsp;&nbsp;&nbsp; cluster;&nbsp;&nbsp;&nbsp;&nbsp; k-means ;&nbsp;&nbsp;&nbsp;&nbsp; cellbin ;&nbsp;&nbsp;&nbsp;&nbsp;STOmics

</br>亮点：
</br>1  The result is the same with [tassel](https://www.maizegenetics.net/tassel),[gapit](https://zzlab.net/GAPIT/) and [gcta](https://yanglab.westlake.edu.cn/software/gcta/#Overview) , just the difference in precision.
</br>2  功能有 1 多种kinship矩阵 2  PCA结果 3  聚类结果  和4 以cluster染色并作图。
</br>3  一个VCF输入，一步到位，方便用户使用.
</br>4  边读边算，内存剥离受位点多少的影响（时空组是剥离受基因数量多少的影响），内存只受样品量影响，故上100k的样品当也行，在这个基础上开发时空细胞PCA和聚类,虽然时空组学上主要是样品多。（80K 60G内存）
</br>5  Kmean聚类分析，并找出最佳K值，和Structure和K值一样. 作图以此染色。
</br>6  提作作图小脚本，可以用这个脚本优化作图等。

</br>

</br>程序是给一些有基础的生信朋友用的，若是小白看不懂就算了。
</br>
</br><b>MingPCACluster</b> MingPCAC is a PCA analysis software format developed based on VCF. It also incorporates Genotype, etc., and develops a file (beta function) for the expression of spatiotemporal cells. That is, as long as the input is satisfied, the PCA and the cluster group are of the same output.


###  2 Download and Install
------------
The <b>new version</b> will be updated and maintained in <b>[hewm2008/MingPCACluster](https://github.com/hewm2008/MingPCACluster)</b>, please click below website to download the latest version
</br><p align="center"><b>[hewm2008/MingPCACluster](https://github.com/hewm2008/MingPCACluster)</b></p>

<b> 2.1. linux/MaxOS&nbsp;&nbsp;&nbsp;   [Download](https://github.com/hewm2008/MingPCACluster/archive/v1.00.tar.gz)</b>
  
  </br> <b>2.2 Pre-install</b>
  </br> MingPCACluster is for Linux/Unix/macOS only.
  </br>Before installing,please make sure the following pre-requirements are ready to use.
  </br> 1) [convert](https://linux.die.net/man/1/convert) command is recommended to be pre-installed, although it is not required
  </br> 2) g++   : g++ with [--std=c++11](https://gcc.gnu.org/) > 4.8+ is recommended
  </br> 3) zlib  : [zlib](https://zlib.net/) > 1.2.3 is recommended
  </br> 4) R     : [R](https://www.r-project.org/) with [ggplot](http://ggplot.yhathq.com/) is recommended

</br> <b>2.3 Install</b>
</br> Users can install it with the following options:
</br> Option 1: 
<pre>
        git clone https://github.com/hewm2008/MingPCACluster.git
        cd MingPCACluster;	chmod 755 -R bin/*
        ./bin/MingPCACluster  -h 
</pre>


###  3 Parameter description
------------
</br><b>3.1 MingPCACluster</b>
</br><b>3.1.1 Main parameter</b>

```php

	Usage: Ming2PCACluster  -InVCF  <in.vcf.gz>  -OutPut <outPrefix>

		-InVCF        <str>      Input SNP VCF Format
		-InGenotype   <str>      InPut Genotype File
		-InSTOgem     <str>      InPut STOmics gem File of MIDCounts(beta)
		-InKinship    <str>      Input SNP K Kinship File Format
		-OutPut       <str>      OutPut File Prefix(Kinship PCA etc)


		-SubPop       <str>      SubGroup Sample File List[ALLsample]
		-Method       <int>      Method of Kinship [1-4],defaut [1]
		                         1:BaldingNicolsKinship(VanRaden/Normalized_IBS)
		                         2:IBSKinshipImpute 3:IBSKinship 4:p_dis

		-help                    Show more Parameters and help [hewm2008]


```
</br> brief description for function:
<pre>
	   #   用法一看即明，最基础的为 一个输入和输出即可 
     #       输入文件基因组格式见  pdf.主要为VCF和gem文件
	   #    更多说明后面将在知乎更新

	    Ming2PCACluster	-InSTOgem	Test.gem.gz	-OutPut	Test	-CellBin	100

          ### run without pop.info
          #   Ming2PCACluster	-InVCF	Khuman.vcf.gz	-OutPut	OUT
          ### run with  pop.info
            Ming2PCACluster	-InVCF	Khuman.vcf.gz	-OutPut	OUT	-InSampleGroup	pop.info 

</pre>

</br><b>3.1.2  Detail parameters</b>
```php

	Usage: Ming2PCACluster  -InVCF  <in.vcf.gz>  -OutPut <outPrefix>

		-InVCF        <str>      Input SNP VCF Format
		-InGenotype   <str>      InPut Genotype File
		-InSTOgem     <str>      InPut STOmics gem File of MIDCounts(beta)
		-InKinship    <str>      Input SNP K Kinship File Format
		-OutPut       <str>      OutPut File Prefix(Kinship PCA etc)


		-SubPop       <str>      SubGroup Sample File List[ALLsample]
		-Method       <int>      Method of Kinship [1-4],defaut [1]
		                         1:BaldingNicolsKinship(VanRaden/Normalized_IBS)
		                         2:IBSKinshipImpute 3:IBSKinship 4:p_dis

		-help                    Show more Parameters and help [hewm2008]


		-MAF           <float>    Min minor allele frequency filter [0.001]
		-Fchr          <str>      Filter the chrX chr[chrX,chrY,X,Y]
		-Miss           <float>    Max ratio of miss allele filter [0.25]
		-Het            <float>    Max ratio of het allele filter [1.00]
		-HWE           <float>    Exact test of Hardy-Weinberg Equilibrium for SNP Pvalue[0]
		-CellBin         <int>      STOmics cell bin[50]
		-KeepRemainVCF            keep the VCF after filter

		-InSampleGroup <string>   In File of sampleGroup info,format(sample groupA)

		-PCANum        <int>      Num of PCA eig [10]
		-MaxCluNum     <int>      Max Cluster Num to find Best K [12]
		-BestKRatio       <float>    Get the best K Cluster by deta-SSE Ratio[0.1]
		-STOName       <string>   STOmics Sample Name STOName


```

</br><b>3.2.2 Other parameters</b>
</br>程序也提供了作图软件perl 作图脚本（这个脚本后面将会优化更动较大，主要是最近时间较忙），作图脚本的简要参数说明如下：

```php
ploteig  -h

	Version:1.0         hewm2008@gmail.com

	Usage: ploteig  -InPCA  pca.eigenvec -OutPrefix Fig


		Options

		-InPCA        <s>   : InPut File of PCA
		-OutPrefix    <s>   : OutPut file prefix

		-BinDir       <s>   : The Bin Dir of gnuplot/R/ps2pdf/convert [$PATH]

		-help               : Show more help [hewm2008]

		-columns      <s>   : the columns to plot a:b [3:4]
		-pops         <s>   : Populations to plot, eg  -p GA:GB:GC [ALL]
		-border       <i>   : how to plot the border (1,2,4,8,3,31 ) [3]
		-title        <s>   : title (legend) [PCA]
		-keystyle     <s>   : put key at top right  default(in) [outside]box [outside]
		-pointsize    <i>   : point size for plot [3]


```

</br><b>3.3 Output files</b>


|Module |    outFlie    |       Description                                                |
|:-----:|:-------------------|:------------------------------------------------------------|
| List  |                    |                                                             |
|       |out.kinship         |输出的亲缘矩阵，各样品的两两关系                                |
|       |out.eigenvec        |输出最优聚类和pca结果                                          |
|       |out.eigenval        |输出最优聚类和pca结果                                          |
|       |out.PCA1_PCA2.pdf   |输出按cluster染色后的pca 1 2图                                 |
|       |out.K.pdf           |输出cluster K图                                               |
|       |out.cluster         |输出的各种K的cluster聚类结果                                   |
|       |Out.cellbin.gz      |输出bin50 cell的结果，若是 -InSTOgem                          |
|       |Out.cluster pdf/png |输出坐标cluester图，若是-InSTOgem                             |


示例图见上面应用场景给的图。示例图和格式当一看即明，相关图可以见example 1 和2


###  4 Example
------------

</br>See more detailed usage in the&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <b>[Chinese Documentation](https://github.com/hewm2008/MingPCACluster/blob/main/Ming2PCACluster使用手册_manual_chinese.pdf)</b>
</br>See more detailed usage in the&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <b>[English Documentation](https://github.com/hewm2008/MingPCACluster/blob/main/Ming2PCACluster使用手册_manual_chinese.pdf)</b>
</br>See the example directory and  Manual.pdf for more detail.
</br>具体见这儿  Manual.pdf for more detail 里面的示意数据和脚本，后期将在某些网址释放一些教程
</br></br> 
../../bin/MingPCACluster      -InVCF        in.vcf.gz     -OutPut outPrefix
</br>  目录  Example/example*/　里面有输入和输出和脚本用法。


* Example 1)千人VCF重测序SNP基因型
</br> 共从K 人数据chr22  dbsnp里面随机挑出了3492个位点，挑 CEU（49） ， CHB（46） ， JPT（56）和 YRI （52）共203 个样品来分析。
</br>聚类走势，best K
</br>![K_SSE.png](https://github.com/hewm2008/MingPCACluster/blob/main/xxample/Example1/K_SSE.png)
</br>PCA结果
</br>![PCA.png](https://github.com/hewm2008/MingPCACluster/blob/main/xxample/Example1/PCA.png)


* Example 2) cellbin时空细胞表达量pca和聚类  

</br>时空分析我初了解主要是：seurat ，我很浅淡的了解，这个包用到的n*m (n是样品，m是位点)的稀疏矩阵，好像周边的做时空的人总说内存很大，我这没有对时空数据敏感，对表达量进行了取log10. 也用了稀疏矩阵 和 n*n, 由于时空n是样品量很大，怕难下降。
</br>初以 我这用了文件大于(File.gem.gz : 177M )， 范围： XXmin:        4975    XXmax:  23374   YYmin:  2525    YYmax:  20724 )。取bin 50， n达到的88507，即主要88507*88507的矩阵double上，占用60.742G (稀疏矩阵5G   矩阵:55G) 。

</br> PCA K Fig
</br>![out1.png](https://github.com/hewm2008/MingPCACluster/blob/main/example/Example2/OUT1.png)
</br> PCA plot Fig
</br>![out2.png](https://github.com/hewm2008/MingPCACluster/blob/main/example/Example2/OUT2.png)
</br> STOmics Cluster plot Fig
</br>![out3.png](https://github.com/hewm2008/MingPCACluster/blob/main/example/Example2/OUT3.png)


###  5 Advantages

</br>速度快，少内存    fast speed, low memory
</br>简明易用    Simple and easy to use
</br>免安装   Free installation


###  6 Discussing
------------
- [:email:](https://github.com/hewm2008/MingPCACluster) hewm2008@gmail.com / hewm2008@qq.com
- join the<b><i> QQ Group : 125293663</b></i>

######################swimming in the sky and flying in the sea #############################

