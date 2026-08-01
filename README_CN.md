
# VCF2PCACluster
A new simple and efficient software to PCA and Cluster For popolation VCF File

###  1 Introduction
<b>VCF2PCACluster</b>  ���ڻ���VCF������PCA�����;���������ͬʱ�沢��Genotype �ȸ�ʽ������
��ֻҪ��Ӧ��һ�������ļ���������PCA����ͼ�����һλ��λ��
</br>
</br> keyword  : VCF2PCA ;&nbsp;&nbsp;&nbsp;&nbsp;    VCF2Kinship ;&nbsp;&nbsp;&nbsp;&nbsp; cluster;&nbsp;&nbsp;&nbsp;&nbsp; k-means ;&nbsp;&nbsp;&nbsp;&nbsp;EMGaussian &nbsp;&nbsp;&nbsp;&nbsp; DBSCAN ;&nbsp;&nbsp;&nbsp;&nbsp;

</br>���㣺
</br>1  The result is the same with [tassel](https://www.maizegenetics.net/tassel),[gapit](https://zzlab.net/GAPIT/) and [gcta](https://yanglab.westlake.edu.cn/software/gcta/#Overview) , just the difference in precision.
</br>2  ������ 1 5��kinship���� 2  PCA��� 3  ������  ��4 ��clusterȾɫ����ͼ��
</br>3  һ��VCF���룬һ����λ�������û�ʹ��.
</br>4  �߶����㣬�ڴ������λ����ٵ�Ӱ�죨ʱ�����ǰ����ܻ����������ٵ�Ӱ�죩���ڴ�ֻ����Ʒ��Ӱ�죬����100k����Ʒ��Ҳ�У�����������Ͽ���ʱ��ϸ��PCA�;���,��Ȼʱ����ѧ����Ҫ����Ʒ�ࡣ��80K 60G�ڴ棩
</br>5  Kmean�����DBSCAN���ࡣ   Keman���������ҳ����Kֵ����Structure��Kֵһ��. ��ͼ�Դ�Ⱦɫ��
</br>6  ������ͼС�ű�������������ű��Ż���ͼ�ȡ�

</br>

</br>�����Ǹ�һЩ�л��������������õģ�����С�׿����������ˡ�
</br>
</br><b>VCF2PCACluster</b> VCF2PCA is a PCA analysis software format developed based on VCF. It also incorporates Genotype, etc., That is, as long as the input is satisfied, the PCA and the cluster group are of the same output.


### Release Notes

#### v1.45 更新日志

Bug 修复：
- 修复 `-InGenotype` 路径中样本位点索引被重复 push 的问题（避免基因型列重复）。
- 修复自动 "Best K" 选择（`findBestKFromSSE`）的初始化问题，EM/DBSCAN 自动 K 不再错误回退。
- 修复 `replace_all()` 死循环风险、为 `split()` 增加空结果保护、修复 `is_digits()` 原地修改 bug。
- 修复 `PSite` 悬空指针（use-after-free），并将所有 `while(!eof())` 读取循环改为 `getline()` 循环。
- DBSCAN 对称距离比较改用 `fabs()`。

性能优化（所有输出与 v1.42 字节级一致，已通过全部测试套件的 md5 校验）：
- 新的分块 VCF 读取器（1 MB 缓冲 + `memchr` 分行）替代 300 字节的 `gzstream` 缓冲，大型纯文本 VCF 文件的 I/O 系统调用减少约 100 倍。
- K-means：`unordered_map` -> `vector<char>`、DBI 预分组、用平方距离替代 `sqrt`。
- DBSCAN：swap-pop 删除与平方距离比较。
- EM 高斯：`MatrixXf::setZero()`。
- Kinship2PCA：去重合并 K-means Best-K 公共代码。
- 基因型存储改为 2-bit 压缩（`BaseType`），并清理 OpenMP 区域。

实测（7.6 GB VCF，1,871 个样本，约 10 万 SNP）：墙钟时间 -19%，CPU 时间 -22%，峰值内存不变。


###  2 Download and Install
------------
The <b>new version</b> will be updated and maintained in <b>[hewm2008/VCF2PCACluster](https://github.com/hewm2008/VCF2PCACluster)</b>, please click below website to download the latest version
</br><p align="center"><b>[hewm2008/VCF2PCACluster](https://github.com/hewm2008/VCF2PCACluster)</b></p>

<b> 2.1. linux/MaxOS&nbsp;&nbsp;&nbsp;   [Download](https://github.com/hewm2008/VCF2PCACluster/archive/v1.45.tar.gz)</b>
  
  </br> <b>2.2 Pre-install</b>
  </br> VCF2PCACluster is for Linux/Unix/macOS only.
  </br>Before installing,please make sure the following pre-requirements are ready to use.
  </br> 1) [OpenMP c/c++](https://www.openmp.org/wp-content/uploads/OpenMP3.1-CCard.pdf) command is recommended to be pre-installed
  </br> 2) g++   : g++ with [--std=c++11](https://gcc.gnu.org/) > 4.8+ is recommended
  </br> 3) zlib  : [zlib](https://zlib.net/) > 1.2.3 is recommended
  </br> 4) R     : [R](https://www.r-project.org/) with [ggplot](http://ggplot.yhathq.com/) is recommended

</br> <b>2.3 Install</b>
</br> Users can install it with the following options:
</br> Option 1��we provide a static version for Linux/Unix 
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
	   #   �÷�һ���������������Ϊ һ�������������� 
	   #    ����˵�����潫��֪������

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

		-help          v1.45      Show more Parameters and help [hewm2008]

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

	Version:1.45         hewm2008@gmail.com

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
|       |out.kinship         |�������Ե���󣬸���Ʒ��������ϵ                             |
|       |out.eigenvec        |������ž����pca���                                        |
|       |out.eigenval        |���pca�������������                                        |
|       |out.PC1_PC2.pdf     |�����clusterȾɫ���pc 1 2ͼ                               |
|       |out.PC1PC2PC3.pdf   |�����clusterȾɫ���pc 1 2 3ͼ                               |


ʾ��ͼ������Ӧ�ó�������ͼ��ʾ��ͼ�͸�ʽ��һ�����������ͼ���Լ�example 1 ��2


###  4 Example
------------

</br>See more detailed usage in the&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <b>[Chinese Documentation](https://github.com/hewm2008/VCF2PCACluster/blob/main/Manual_CN.pdf)</b>
</br>See more detailed usage in the&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <b>[English Documentation](https://github.com/hewm2008/VCF2PCACluster/blob/main/Manual_EN.pdf)</b>
</br>See the example directory and  Manual.pdf for more detail.
</br>��������  Manual.pdf for more detail �����ʾ�����ݺͽű������ڽ���ĳЩ��ַ�ͷ�һЩ�̳�
</br></br> 
../../bin/VCF2PCACluster      -InVCF        in.vcf.gz     -OutPut outPrefix
</br>  Ŀ¼  Example/example*/�����������������ͽű��÷���


* Example 1)ǧ��VCF�ز���SNP������
</br> ����K ������chr22  dbsnp�������������1194��λ�㣬�� CEU��49�� �� CHB��46�� �� JPT��56���� YRI ��52����203 ����Ʒ��������
</br>EM Gaussian Ĭ�ϲ�����PC1 PC2���
</br>![PC12.png](https://github.com/hewm2008/VCF2PCACluster/blob/main/example/Example1/PC12.png)
</br>EM Gaussian Ĭ�ϲ�����PC1 PC3���
</br>![PC13.png](https://github.com/hewm2008/VCF2PCACluster/blob/main/example/Example1/PC13.png)
</br>EM Gaussian Ĭ�ϲ�����PCA 3D ���
</br>![PC3D.png](https://github.com/hewm2008/VCF2PCACluster/blob/main/example/Example1/PC3D.png)



* Example 2) ǧ�����ݲ���  

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

</br>�ٶȿ죬���ڴ�    fast speed, low memory
</br>��������    Simple and easy to use  (-i  -o)
</br>�ⰲװ   Free installation
</br>һ����ͼ    only one step from vcf  to final drawing

###  6 Discussing
------------
- [:email:](https://github.com/hewm2008/VCF2PCACluster) hewm2008@gmail.com / hewm2008@qq.com
- join the<b><i> QQ Group : 125293663</b></i>

######################swimming in the sky and flying in the sea #############################

