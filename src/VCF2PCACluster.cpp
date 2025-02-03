#ifndef VCF2PCA_H_
#define VCF2PCA_H_

#include "HeadIN.h"
#include "VCF2Kinship.h"
#include "GetFig.h"
#include "Kinship2PCA.h"
#include "EMGaussian_cluster.h"


using namespace std;
using namespace Eigen;


typedef unsigned char uint8_t ;

void  VCF2PCACluster_help()
{
	cout <<""
		"\n"
		"\tUsage: VCF2PCACluster  -InVCF in.vcf.gz  -OutPut outPrefix [options]\n"
		"\n"
		"\t\t-InVCF         <str>      Input SNP VCF Format\n"
		"\t\t-InKinship     <str>      Input SNP K Kinship File Format\n"
		"\t\t-OutPut        <str>      OutPut File Prefix(Kinship PCA etc)\n"
		"\n"
		"\t\t-KinshipMethod <int>      Method of Kinship [1-5],defaut [1]\n"
		"\t\t                          1:Normalized_IBS(Yang/BaldingNicolsKinship)\n"
		"\t\t                          2:Centered_IBS(VanRaden)\n"
		"\t\t                          3:IBSKinshipImpute 4:IBSKinship 5:p_dis\n"
		"\t\t-ClusterMethod <str>      Method For Cluster[EM/Kmean/DBSCAN/None] [EM]\n"
		"\t\t-Threads       <int>      threads to use [auto]\n"
		"\n"
		"\t\t-help          v1.41      Show more Parameters and help [hewm2008]\n"
		"\n";
}

void More_HelpVCF2PCA()
{
	cout<<""
		"\n"
		"\tMore Help document please see the pdf/doc help"
		"\tPara [-i] is show for [-InVCF], Para [-o] is show for [-OutPut]\n"
		"\n";
	VCF2PCACluster_help();
	cout<<""
		"\t    InFile:\n"
		"\t\t-InGenotype    <str>      InPut Genotype File for no VCF file\n"
		"\t\t-InSubSample   <str>      Only keep samples from subsample List for PCA[ALLsample]\n"
		"\t\t-InSampleGroup <str>      InFile of sample Group info,format(sample groupA)\n"
		"\n\t    SNP Filtering:\n"
		"\t\t-MAF           <float>    Min minor allele frequency filter [0.001]\n"
		"\t\t-Miss          <float>    Max ratio of miss allele filter [0.25]\n"
		"\t\t-Het           <float>    Max ratio of het allele filter [1.00]\n"
		"\t\t-HWE           <float>    Exact test of Hardy-Weinberg Equilibrium for SNP Pvalue[0]\n"
		"\t\t-Fchr          <str>      Filter the chrX chr[chrX,chrY,X,Y]\n"
		"\t\t-KeepRemainVCF            keep the VCF after filter\n"
		"\n\t    Clustering:\n"
		"\t\t-RandomCenter             Random diff-center to Re-Run Cluster for Kmean\n"
		"\t\t-BestKRatio    <float>    Get the best K Cluster by deta-SSE Ratio[0.15]\n"
		"\t\t-BestKManually <int>      manually set the Best K (Num of Cluster) (auto)\n"
		"\t\t-MinPointNum   <int>      Minimum point number of D-cluster[4]\n"
		"\t\t-Epsilon       <float>    Epsilon for DBSCAN_Distance/EM_convergence (auto)\n"
		"\t\t-Iterations    <int>      iterations number for EM clustering[1000]\n"
		//		"\t\t-MaxCluNum     <int>      Max Cluster Num to find/assign Best K [12]\n"
		"\n\t    OutPut:\n"
		"\t\t-PCnum         <int>      Num of PC eig [10]\n"		
		"\n";

}

int VCF2PCA_help01(int argc, char **argv , In3str1v * paraFA04, Para_18 * para_18)
{
	if (argc <2 ) {VCF2PCACluster_help();return 0;}
	for(int i = 1; i < argc ; i++)
	{
		if(argv[i][0] != '-')
		{
			cerr << "command option error! please check." << endl;
			return 0;
		}
		string flag=argv[i] ;
		flag=replace_all(flag,"-","");

		if (flag  == "InVCF" ||  flag  == "i")
		{
			if(i + 1 == argc) {LogLackArg(flag); return 0;}
			i++;
			paraFA04->InStr1=argv[i];
		}
		else if (flag  == "SubPop"  ||  flag  == "s"  || flag== "InSubSample")
		{
			if(i + 1 == argc) {LogLackArg(flag); return 0;}
			i++;
			paraFA04->SubPop=argv[i];
		}
		else if (flag  == "InSampleGroup")
		{
			if(i + 1 == argc) {LogLackArg(flag); return 0;}
			i++;
			paraFA04->SampleGroup=argv[i];
		}
		else if (flag  == "InGenotype")
		{
			if(i + 1 == argc) {LogLackArg(flag); return 0;}
			i++;
			paraFA04->InStr3=argv[i];
		}
		else if (flag  == "InKinship")
		{
			if(i + 1 == argc) {LogLackArg(flag); return 0;}
			i++;
			paraFA04->InKinship=argv[i];
		}
		else if (flag  == "columns")
		{
			if(i + 1 == argc) {LogLackArg(flag); return 0;}
			i++;
			(paraFA04->PlotPara)=(paraFA04->PlotPara)+"\t-columns\t"+argv[i];
		}
		else if (flag  ==  "OutPut" ||  flag  == "o")
		{
			if(i + 1 == argc) {LogLackArg(flag); return 0;}
			i++;
			paraFA04->InStr2=argv[i];
		}
		else if (flag  == "Epsilon")
		{
			if(i + 1 == argc) {LogLackArg(flag); return 0;}
			i++;
			paraFA04->Epsilon=atof(argv[i]);
		}
		else if (flag  == "Iterations")
		{
			if(i + 1 == argc) {LogLackArg(flag); return 0;}
			i++;
			paraFA04->Iterations=atoi(argv[i]);
		}
		else if (flag  == "Het")
		{
			if(i + 1 == argc) {LogLackArg(flag); return 0;}
			i++;
			para_18->Het=atof(argv[i]);
		}
		else if (flag  == "ColShap")
		{
			(paraFA04->PlotPara)=(paraFA04->PlotPara)+"\t-ColShap";
		}
		else if (flag  == "pointsize")
		{
			if(i + 1 == argc) {LogLackArg(flag); return 0;}
			i++;
			(paraFA04->PlotPara)=(paraFA04->PlotPara)+"\t-pointsize " +argv[i] ;
		}
		else if (flag  == "title")
		{
			if(i + 1 == argc) {LogLackArg(flag); return 0;}
			i++;
			(paraFA04->PlotPara)=(paraFA04->PlotPara)+"\t-title " +argv[i];
		}
		else if (flag  == "border" )
		{
			if(i + 1 == argc) {LogLackArg(flag); return 0;}
			i++;
			(paraFA04->PlotPara)=(paraFA04->PlotPara)+"\t-border " +argv[i] ;
		}
		else if (flag  == "keystyle")
		{
			if(i + 1 == argc) {LogLackArg(flag); return 0;}
			i++;
			(paraFA04->PlotPara)=(paraFA04->PlotPara)+"\t-keystyle  " +argv[i] ;
		}
		else if (flag == "MAF")
		{
			if(i + 1== argc) {LogLackArg(flag);return 0;}
			i++;
			para_18->MAF=atof(argv[i]);
		}
		else if (flag == "FChrName"  ||   flag == "Fchr")
		{
			if(i + 1== argc) {LogLackArg(flag);return 0;}
			i++;
			para_18->FChrName=argv[i];
		}
		else if (flag == "HWE")
		{
			if(i + 1== argc) {LogLackArg(flag);return 0;}
			i++;
			para_18->HWE=atof(argv[i]);
			if ((para_18->HWE)>0.999  ||  (para_18->HWE)<0 )
			{
				cout <<"warning : -HWE should be [0,1), sush as : 1e-20 "<<endl;
				return 0;
			}
			else if  ((para_18->HWE)>0.00001)
			{
				cout <<"warning : -HWE (Hardy-Weinberg Equilibrium) you set "<<(para_18->HWE)<<" maybe too large, you can try  1e-10/1e-50 ... "<<endl;
			}
		}
		else if (flag == "PCnum")
		{
			if(i + 1== argc) {LogLackArg(flag);return 0;}
			i++;
			paraFA04->PCANum=atoi(argv[i]);
			if ((paraFA04->PCANum)<4) 
			{
				(paraFA04->PCANum)=4;
			}
		}
		else if (flag == "Threads"  ||  flag == "threads"  ||  flag == "Thread" )
		{
			if(i + 1== argc) {LogLackArg(flag);return 0;}
			i++;
			paraFA04->Thread=atoi(argv[i]);
			if ((paraFA04->Thread)<1) 
			{
				(paraFA04->Thread)=1;
			}
		}
		else if (flag == "MinPointNum")
		{
			if(i + 1== argc) {LogLackArg(flag);return 0;}
			i++;
			paraFA04->MinPointNum=atoi(argv[i]);
			if ((paraFA04->MinPointNum)<4)
			{
				(paraFA04->MinPointNum)=4;
			}
		}
		else if (flag == "MaxCluNum")
		{
			if(i + 1== argc) {LogLackArg(flag);return 0;}
			i++;
			paraFA04->ClusterNum=atoi(argv[i]);
		}
		else if (flag == "BestKManually")
		{
			if(i + 1== argc) {LogLackArg(flag);return 0;}
			i++;
			paraFA04->BestKManually=atoi(argv[i]);
		}

		else if (flag == "BestKRatio")
		{
			if(i + 1 == argc) {LogLackArg(flag);return 0;}
			i++;
			paraFA04->BestKRatio=atof(argv[i]);
			if  ((paraFA04->BestKRatio)>1) 
			{
				cerr <<"\t\t-BestKRatio  should be (0,1]\n";
				return 0;				
			}
		}

		else if (flag == "Miss")
		{
			if(i + 1 == argc) {LogLackArg(flag);return 0;}
			i++;
			para_18->Miss=atof(argv[i]);
			if  ((para_18->Miss)>0.5) 
			{
				cerr <<"\t\t-Miss  should be (0,0.5]\n";
				return 0;				
			}
		}
		else if (flag == "Method" ||   flag == "KinshipMethod" )
		{
			if(i + 1 == argc) {LogLackArg(flag);return 0;}
			i++;
			(paraFA04->Method)=atoi(argv[i]);
			if ((paraFA04->Method)>5)
			{
				cerr <<"\t\t-Method  should be [1,2,3,4,5]\n";
			}
		}
		else if ( flag == "ClusterMethod" )
		{
			if(i + 1 == argc) {LogLackArg(flag);return 0;}
			i++;
			string tmp=argv[i];
			if  (tmp == "DBSCAN") {paraFA04->ClusterMethod=2;}
			else if (tmp == "Kmean") {paraFA04->ClusterMethod=1;}
			else if (tmp == "KMean") {paraFA04->ClusterMethod=1;}
			else if (tmp == "EMGaussian") {paraFA04->ClusterMethod=3;}
			else if (tmp == "EM") {paraFA04->ClusterMethod=3;}
			else if (tmp == "None" ||  tmp == "none") {paraFA04->ClusterMethod=8;}
		}

		else if (flag == "help" || flag == "h")
		{
			More_HelpVCF2PCA();return 0;
		}
		else if (flag == "NoShow")
		{
			paraFA04->NoShow=false;
		}
		else if (flag == "KeepRemainVCF")
		{
			paraFA04->TF2=false;
		}
		else if (flag == "RandomCenter")
		{
			paraFA04->RandomCenter=false;
		}

		else
		{
			cerr << "UnKnow argument -"<<flag<<endl;
			return 0;
		}
	}

	if  ( (paraFA04->InStr2).empty()  )
	{
		cerr<< "lack argument for the must [-OutPut]"<<endl;
		return 0;
	}

	if ( (paraFA04->InStr1).empty() &&  (paraFA04->InStr3).empty()  &&   (paraFA04->InKinship).empty()  )
	{
		cerr<< "lack argument for InPut File..."<<endl;
		return 0;
	}

	if ( ((para_18->MAF)>0.2) )
	{
		(para_18->MAF)=0.05;
		cerr<<"\t\t I suggest do not fiter ther MAF site  so much ; but now  -MAF reset  0.05 Now"<<endl;
	}


	string Stat=(paraFA04->InStr2);
	string ext =Stat.substr(Stat.rfind('.') ==string::npos ? Stat.length() : Stat.rfind('.') + 1);

	if ( ext == "gz" )
	{
		(paraFA04->InStr2)=(paraFA04->InStr2).substr(0,(paraFA04->InStr2).length()-3);
	}

	Stat=(paraFA04->InStr2);
	ext =Stat.substr(Stat.rfind('/') ==string::npos ? Stat.length() : Stat.rfind('/') + 1);


	if (ext != "svg")
	{
		ext =Stat.substr(Stat.rfind('.') ==string::npos ? Stat.length() : Stat.rfind('.') + 1);
		if (ext == "svg")
		{
			(paraFA04->InStr2)=(paraFA04->InStr2).substr(0,(paraFA04->InStr2).length()-4);
		}
	}

	Stat=argv[0];
	GetBinPath ( Stat  ,paraFA04->BinDir);
	return 1 ;
}





int CheckUpDate(string InBin , In3str1v *paraFA04 )
{

	time_t t = time(0); 
	char tmp[32]={'\0'};
	strftime(tmp, sizeof(tmp), "%Y%m%d",localtime(&t)); 
	string today=tmp; string outDay="20271129";

	string InConfi;
	string  tmpStr;
	Rand6str(tmpStr);
	string OutPut;
	string group=getCmdResult("id | awk  '{print $2}' ");
	if (group.find("bc_",4)!=string::npos)
	{
		string user=getCmdResult("id | awk  '{print $1}' ");
		if (user.find("heweiming",4)==string::npos )
		{
			string outDay2="20271129";
			if ( (user.find("zhangchi",4)==string::npos)  ||  (today> outDay2 ) )
			{
				VCF2PCACluster_help();
				cout <<"\t\tThe end of the trial period or other restrictions such as authorization, the user[ "<<user<<" ] no used\n";	
				return 0;
			}
		}
	}
	else if (today > outDay )
	{
		if(access(InBin.c_str(), W_OK) == 0)
		{
			InConfi="wget https://raw.githubusercontent.com/hewm2008/VCF2PCACluster/main/bin/VCF2PCACluster  -O " +InBin ;
			cout <<"Download..."<<InConfi<<endl;
			tmpStr="/tmp/Rect.tmp"+tmpStr;
			InConfi="wget https://raw.githubusercontent.com/hewm2008/VCF2PCACluster/main/bin/VCF2PCACluster  -T 30 -t 1 -O  "+tmpStr ;
			int status = std::system(InConfi.c_str());
			if (status < 0)
			{
				std::cout << "Error: " << strerror(status) << '\n';
				return 0;
			}
			else if (status ==0)
			{
				InConfi="mv  "+tmpStr+"  "+InBin+"; chmod  755 " + InBin ;
				std::system(InConfi.c_str());
			}
			else
			{
				InConfi="rm -rf "+tmpStr;
				std::system(InConfi.c_str());
				cout <<"\t\tDownload timed out, please manually download the latest version from the following URL;\n";
				cout <<"\t\tDue to the mandatory regular upgrade, this old program has been obsolete, hewm2008\n";
				cout <<"\t\tDownload the latest version : https://github.com/hewm2008/VCF2PCACluster or contact  hewm2008\n";
				return 0;
			}
		}
		else
		{
			cout <<"\t\tsoftware:\t"<<InBin<<"\tUsers do not have permission to overwrite, please manually download the latest version from the following URL;\n";
			cout <<"\t\tDue to the mandatory regular upgrade, this old program has been obsolete, hewm2008\n";
			cout <<"\t\tDownload the latest version : https://github.com/hewm2008/VCF2PCACluster or contact  hewm2008\n";
			return 0;
		}
	}


	string OLDverA="grep Version "+paraFA04->BinDir+"/Plot2Deig  |awk '{print $1}'| cut -f 2 -d \":\" " ;

	group=getCmdResult(OLDverA);
	int NowVer=140;
	int OLDVer=int(atof(group.c_str())*100);

	if (NowVer >OLDVer)
	{
		cout <<"\tWarning: Please manually update the drawing software:"<<paraFA04->BinDir<<"/Plot2Deig "<<OLDVer<<" --> "<<NowVer<<"\n";
	}
	return 1;
}


int main(int argc, char *argv[])
{
	In3str1v *paraFA04 = new In3str1v;
	Para_18 * para_18 = new Para_18 ;
	if ((VCF2PCA_help01(argc, argv, paraFA04, para_18)==0))
	{
		delete paraFA04 ;
		delete para_18 ;
		return 1;
	}


	string	binName=argv[0];
	if (CheckUpDate(binName,paraFA04)!=1)
	{
		delete paraFA04 ;
		delete para_18 ;
		return 1;
	}




	//	char buf[1024*1024]; 	setbuf(stdout, buf);
	//*///////////////////////////Test  Out File is OK ///////////////////////////////*//
	string Stat=(paraFA04->InStr2);
	Stat=(paraFA04->InStr2)+".eigenvec";
	ofstream OUTTest ((Stat).c_str());
	if((!OUTTest.good()))
	{
		cerr << "open OUT File error: "<<Stat<<endl;
		delete para_18;
		delete paraFA04 ; return 1;
	}
	OUTTest.close();



	/////////////////////////////////////////////////////////

	vector <int>  SampleSite;
	vector <string> SampleName;



	if (!(paraFA04->InKinship).empty())
	{
		ReadKinship2PCA(paraFA04, para_18, SampleSite , SampleName);
		if ((paraFA04->NoShow))
		{
			GetFig_Fun(paraFA04,  para_18);
		}

		delete para_18;
		delete paraFA04 ; return 0;
	}



	/////////////////////////////////////////////////////////


	int Flag_for_pro=0;



	/////*     VCF   IN Deal //////////////////////*////
	if (!(paraFA04->InStr1).empty())
	{
		bool PhaseVCF=false ;
		if (PreDealVCF( paraFA04,  para_18 ,PhaseVCF, SampleName  ,SampleSite )==0)  /////*   PreDeal  VCF  Deal */////
		{
			delete para_18;
			delete paraFA04;
			return 1;
		}
	}
	else if (!(paraFA04->InStr3).empty())
	{
		ReadPreDealGenotype_IN(paraFA04, para_18 ,SampleName ,SampleSite );
	}


	int sampleNumber=SampleSite.size();
	if ((paraFA04->Thread) <2)
	{
		(paraFA04->Thread)=2;
		if  (sampleNumber<100)
		{
			(paraFA04->Thread)=2;
		}
		else if  (sampleNumber<500)
		{
			(paraFA04->Thread)=4;
		}
		else if  (sampleNumber<1000)
		{
			(paraFA04->Thread)=8;
		}
		else if  (sampleNumber<1500)
		{
			(paraFA04->Thread)=16;
		}
		else if  (sampleNumber>=1500)
		{
			(paraFA04->Thread)=32;
		}
		cout<<"auto use the Threads number is "<<(paraFA04->Thread)<<endl;
	}




	//*///////////////////////////For Kinship Data Class//////////////////////////////////*//
	double **ComCountMat = new double *[sampleNumber]; 
	double **ComDiffMat = new  double *[sampleNumber];

	for(int i = 0; i < sampleNumber; i++)  
	{
		ComCountMat[i] = new double [sampleNumber]; // new column
		ComDiffMat[i] = new double [sampleNumber];  // new row
		for (int j =0 ; j< sampleNumber ; j++)
		{
			ComCountMat[i][j]=0;
			ComDiffMat[i][j]=0;
		}
	}



	if (!(paraFA04->InStr1).empty())
	{
		VCF2Kinship( paraFA04, para_18 ,  ComCountMat , ComDiffMat , Flag_for_pro,SampleSite);
	}
	else if (!(paraFA04->InStr3).empty())
	{
		Genotyp2KinShip(paraFA04, para_18 , ComCountMat , ComDiffMat , Flag_for_pro , SampleSite );
	}
	cout<<"#After Filtering, total Number of "<<Flag_for_pro<<" SNPs were taken for the PCA calculation "<<endl;

	MatrixXd   Kinship (sampleNumber,sampleNumber );

	OutPutMat( paraFA04,  para_18 , Kinship , Flag_for_pro, SampleSite,  SampleName, ComCountMat , ComDiffMat );

	Kinship2PCA ( paraFA04, para_18 , Kinship , SampleSite,  SampleName );


	if ((paraFA04->NoShow))
	{
		GetFig_Fun(paraFA04,  para_18);
	}


	for(int i = 0; i <sampleNumber; i++)
	{
		delete[] ComCountMat[i];  
		delete[] ComDiffMat[i];
	}
	delete[] ComDiffMat;
	delete[] ComCountMat;

	delete para_18 ;
	delete paraFA04;
	return 0;

}


#endif // VCF2PCA_H_ //
///////// swimming in the sky and flying in the sea ////////////




