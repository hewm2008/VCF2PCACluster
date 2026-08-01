### download the 3K Rice 29M data ###
#wget  -c https://s3.amazonaws.com/3kricegenome/reduced/NB_final_snp.fam.gz  ./
#wget  -c https://s3.amazonaws.com/3kricegenome/reduced/NB_final_snp.bim.gz  ./
#wget  -c https://s3.amazonaws.com/3kricegenome/reduced/NB_final_snp.bed.gz  ./

###  change bhe bed to vcf ###
#gzip -d *.gz
#plink --bfile  NB_final_snp  --recode vcf-iid --out NB_final_snp   # bed back to vcf  259G mem


###  Run  VCF2PCACluster  with 40 cpu  (0.1G 181min)###
time   ../bin/VCF2PCACluster	-InVCF	NB_final_snp.vcf	-OutPut	VCF2PCA  -BestKManually  4  #0.1G 181min 40cpu

###  Run plink2   with 40 cpu  (257G mem     100min)
time   plink2	--vcf	NB_final_snp.vcf	-out	plink	--allow-extra-chr	--pca  ##  257G mem     100min 40cpu

### Run gcta  (Result same with VCF2PCACluster) ###
time   plink2   --vcf    NB_final_snp.vcf      -out  Rice3K  --allow-extra-chr      --make-bed   ##  257G mem  60min 40cpu
time	gcta64	--bfile	Rice3K	--make-grm	--out	out.grm	 ## 3.9G 	223min   1cpu
time	gcta64	--grm	out.grm	--pca	10	--out  gctaPCA   ## 0.1G    1min   1cpu
