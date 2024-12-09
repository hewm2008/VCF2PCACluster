#ifndef ReadDataIn_H_

#define ReadDataIn_H_


#include "include/json_parser/subbuffer.h"
#include "include/json_parser/subparser.h"


#define Block  32


using namespace std;
using namespace Eigen;

struct BaseType
{
	unsigned short int GenotypeValue:2 ;
};

int VCF2Kinship(In3str1v * paraFA04, Para_18 * para_18 ,  double **  CountPair ,  double ** Kinship  ,int & Flag_for_pro ,vector <int> & SampleSiteVV )
{

	igzstream VCFIN ((paraFA04->InStr1).c_str(),ifstream::in);

	if (VCFIN.fail())
	{
		cerr << "open VCF File IN File error: "<<(paraFA04->InStr1)<<endl;
		return  0;
	}

	vector<string> inf ;

	while(!VCFIN.eof())
	{
		string  line ;
		getline(VCFIN,line);
		if (line.length()<=0 )  { continue  ; }
		else if ( line[0] == '#' && line[1] == '#' )  { continue  ; }
		else if ( line[0] == '#' && line[1] != '#' )
		{
			inf.clear();
			if (!line.empty() && line[line.size() - 1] == '\r')  { line.erase(line.size() - 1);}
			split(line,inf," \t");
			if ( inf[0]  != "#CHROM")
			{
				continue ;
			}
			break ;
		}
		else if ( line[0] != '#' && line[1] != '#' )
		{
			cerr<<"wrong Line : "<<line<<endl;
			cerr<<"VCF Header something wrong, can't find sample info before site info"<<endl;
			cerr<<"VCF Header sample info Flag : [  #CHROM  ] "<<endl;
			return  0;
			break;
		}
	}


	int BadSite=0 ;
	int BadIndelSite=0;
	int BadHWE=0;
	int BadMiss=0;
	int BadHete=0;
	int BadMAF=0;
	int Asample=inf.size();	
	std::vector<subbuffer> spline;
	spline.reserve(Asample);
	int SampleNum=SampleSiteVV.size();
	int NumberSubGroup=SampleNum;


	int *SampleSite ;
	SampleSite = new int [Asample];
	for (int ii=0 ; ii<Asample ; ii++)
	{
		SampleSite[ii]=SampleSiteVV[ii];
	}


	BaseType *	BaseALL[Block];
	int BlockReadCount=0;
	for(int i = 0; i < Block ; i++)
	{
		BaseALL[i]= new BaseType [Asample];
	}



	int BadChrName=0;
	vector<string> ChrSexT;
	split(para_18->FChrName,ChrSexT,",");
	map <string,int> ChrSex;
	for (int ii=0; ii<ChrSexT.size(); ii++)
	{
		ChrSex[ChrSexT[ii]]=ii;
	}

	vector<string> Alt ;

	int  Base_len ;
	llong Site ;
	int Het_count=0;
	int Miss_count=0;
	int BaseConut=0;
	BaseType best_base; best_base.GenotypeValue=0;
	BaseType Miss_base;	Miss_base.GenotypeValue=3;
	BaseType sed_base;  sed_base.GenotypeValue=2;
	BaseType hete_base; hete_base.GenotypeValue=1;


	int Max=0;
	int SeD=0;
	int aa_count=0;
	int bb_count=0;
	map <char,int>  :: iterator it ;
	int ERA=0;
	bool MaxSed=true ;
	double TmpBB= 0 ;
	int SampleNumMiss=int((para_18->Miss)*SampleNum);
	int SampleNumHete=int((para_18->Het)*SampleNum);
	double NowMAF;
	string ALTstring =""; 

	if (paraFA04->TF2)
	{

		if ((paraFA04->Method)==1) //BaldingNicolsKinship //Normalized_IBS//Yang
		{

			double mean=0.0;
			double scale=0.0;

			double AA_PP=0.0;
			double BB_PP=0.0;
			double AB_PP=0.0; 



			double  TowArry[Block][4][4]={0};
			for (int cc=0; cc<Block ; cc++)
			{
				TowArry[cc][0][3] = TowArry[cc][1][3] = TowArry[cc][2][3] =TowArry[cc][3][0] =TowArry[cc][3][2] =TowArry[cc][3][1] = 0;
				TowArry[cc][3][3]=0;
			}


			int ComCout[4][4]={1};
			ComCout[0][0]=ComCout[1][1]=ComCout[2][2]=1;
			ComCout[3][0]=ComCout[3][1]=ComCout[3][2]=ComCout[0][3]=ComCout[1][3]=ComCout[2][3]=ComCout[3][3]=0;
			ComCout[0][2]=ComCout[2][0]=1;
			ComCout[0][1]=ComCout[1][0]=ComCout[1][2]=ComCout[2][1]=1;

			/*
			   struct timeval t1,t2;
			   double timeuse;
			   gettimeofday(&t1,NULL);
			   clock_t start,end;
			   start = clock();
			   */
			while(!VCFIN.eof())
			{
				string  line ;
				getline(VCFIN,line);
				if (line.length()<=0)  { continue  ; }

				spline.clear();
				subparser flds(line, '\t', subparser::SKIP_EMPTY);
				flds.split(spline);

				inf[0].assign(spline[0].begin(),spline[0].length());
				if (ChrSex.find(inf[0]) != ChrSex.end())
				{
					BadChrName++;
					continue ;
				}
				Base_len=spline[3].length();

				ALTstring.assign(spline[4].begin(),spline[4].length());
				Alt.clear();

				split(ALTstring,Alt,",");

				for (int ii=0 ; ii<Alt.size() ;ii++)
				{
					if (Alt[ii].length()>Base_len)
					{
						Base_len=Alt[ii].length();
					}
				}

				if (Base_len>1)
				{
					BadIndelSite++;
					continue ;
				}

				map <char,bool> CountBase;


				Het_count=0;
				Miss_count=0;
				aa_count=0;
				bb_count=0;


				for (int kk=0 ; kk< NumberSubGroup ; kk++)
				{
					if (spline[SampleSite[kk]][0] == '.')
					{
						BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=Miss_base.GenotypeValue;
						Miss_count++;
					}
					else
					{
						if (spline[SampleSite[kk]][0] != spline[SampleSite[kk]][2])
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=hete_base.GenotypeValue;
							Het_count++;
						}
						else if (spline[SampleSite[kk]][0] =='0')
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=best_base.GenotypeValue;
							aa_count++;
						}
						else
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=sed_base.GenotypeValue;
							bb_count++;
						}
						CountBase[spline[SampleSite[kk]][0]]=true;
						CountBase[spline[SampleSite[kk]][2]]=true;
					}
				}

				BaseConut=CountBase.size();

				if ( BaseConut!=2)
				{
					BadSite++;
					continue;
				}

				if  ( Miss_count>SampleNumMiss )
				{
					BadMiss++;
					continue;
				}

				if ( Het_count > SampleNumHete )
				{
					BadHete++;
					continue;
				}

				Max=(2*aa_count)+Het_count;			
				SeD=(2*bb_count)+Het_count;

				if (SeD<=Max)
				{
					NowMAF=(SeD*1.0/(SeD+Max));
					MaxSed=true ;
				}
				else
				{					
					NowMAF=(Max*1.0/(SeD+Max));
					MaxSed=false;
				}

				if ( NowMAF  < (para_18->MAF) )  
				{
					BadMAF++;
					continue;
				}

				double  p=SNPHWE(Het_count,aa_count,bb_count);
				if  (p< (para_18->HWE)) {BadHWE++ ;continue ;}
				Flag_for_pro++;



				mean= NowMAF*2.0;
				scale = 1.0/(mean*(1.0 - NowMAF));

				if (MaxSed)
				{
					AA_PP=(0- mean);
					AB_PP=(1- mean);
					BB_PP=(2- mean);
				}
				else
				{
					AA_PP=(2- mean);
					AB_PP=(1- mean);
					BB_PP=(0- mean);
				}

				TowArry[BlockReadCount][0][0]= AA_PP*AA_PP*scale;
				TowArry[BlockReadCount][0][1]= TowArry[BlockReadCount][1][0]=AA_PP*AB_PP*scale;
				TowArry[BlockReadCount][0][2]= TowArry[BlockReadCount][2][0]=AA_PP*BB_PP*scale;
				TowArry[BlockReadCount][1][2]= TowArry[BlockReadCount][2][1]=AB_PP*BB_PP*scale;
				TowArry[BlockReadCount][2][2]= BB_PP*BB_PP*scale;
				TowArry[BlockReadCount][1][1]= AB_PP*AB_PP*scale;


				BlockReadCount++;
				if (BlockReadCount!=Block  &&  (!VCFIN.eof()))
				{
					continue;
				}

				//#pragma omp parallel for schedule (static)
				//#pragma omp parallel for schedule (guided)
#pragma omp parallel for schedule (dynamic)  
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							CountPair[jj][kk]+=ComCout[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
							Kinship[jj][kk]+=TowArry[cc][BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}
				BlockReadCount=0;

			}

			if (BlockReadCount>0)
			{
#pragma omp parallel for schedule (dynamic)  
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							CountPair[jj][kk]+=ComCout[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
							Kinship[jj][kk]+=TowArry[cc][BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}
				BlockReadCount=0;


			}


			/*
			   end = clock();
			   cout<<"F2运行时间"<<(double)(end-start)/CLOCKS_PER_SEC<<endl;

			   gettimeofday(&t2,NULL);
			   timeuse = (t2.tv_sec - t1.tv_sec) + (double)(t2.tv_usec - t1.tv_usec)/1000000.0;

			   cout<<"time = "<<timeuse<<endl;  //输出时间（单位：ｓ）
			   exit(1);
			   */
		}
		if ((paraFA04->Method)==2) //Centered_IBS//VanRaden
		{

			double mean=0.0;
			double scale=0.0;

			double AA_PP=0.0;
			double BB_PP=0.0;
			double AB_PP=0.0; 

			double  TowArry[Block][4][4]={0};
			double  TowArryComCount[Block][4][4]={0};
			for (int cc=0; cc<Block ; cc++)
			{
				TowArry[cc][0][3] = TowArry[cc][1][3] = TowArry[cc][2][3] =TowArry[cc][3][0] =TowArry[cc][3][2] =TowArry[cc][3][1] = 0;
				TowArry[cc][3][3]=0;
				TowArryComCount[cc][3][3]=0;
				TowArryComCount[cc][0][3] =TowArryComCount[cc][1][3] = TowArryComCount[cc][2][3] =TowArryComCount[cc][3][0] =TowArryComCount[cc][3][2] =TowArryComCount[cc][3][1] = 0;
			}



			while(!VCFIN.eof())
			{
				string  line ;
				getline(VCFIN,line);
				if (line.length()<=0)  { continue  ; }

				spline.clear();
				subparser flds(line, '\t', subparser::SKIP_EMPTY);
				flds.split(spline);

				inf[0].assign(spline[0].begin(),spline[0].length());
				if (ChrSex.find(inf[0]) != ChrSex.end())
				{
					BadChrName++;
					continue ;
				}
				Base_len=spline[3].length();

				ALTstring.assign(spline[4].begin(),spline[4].length());
				Alt.clear();

				split(ALTstring,Alt,",");

				for (int ii=0 ; ii<Alt.size() ;ii++)
				{
					if (Alt[ii].length()>Base_len)
					{
						Base_len=Alt[ii].length();
					}
				}

				if (Base_len>1)
				{
					BadIndelSite++;
					continue ;
				}

				map <char,bool> CountBase;


				Het_count=0;
				Miss_count=0;
				aa_count=0;
				bb_count=0;


				for (int kk=0 ; kk< NumberSubGroup ; kk++)
				{
					if (spline[SampleSite[kk]][0] == '.')
					{
						BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=Miss_base.GenotypeValue;
						Miss_count++;
					}
					else
					{
						if (spline[SampleSite[kk]][0] != spline[SampleSite[kk]][2])
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=hete_base.GenotypeValue;
							Het_count++;
						}
						else if (spline[SampleSite[kk]][0] =='0')
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=best_base.GenotypeValue;
							aa_count++;
						}
						else
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=sed_base.GenotypeValue;
							bb_count++;
						}
						CountBase[spline[SampleSite[kk]][0]]=true;
						CountBase[spline[SampleSite[kk]][2]]=true;
					}
				}

				BaseConut=CountBase.size();

				if ( BaseConut!=2)
				{
					BadSite++;
					continue;
				}

				if  ( Miss_count>SampleNumMiss )
				{
					BadMiss++;
					continue;
				}

				if ( Het_count > SampleNumHete )
				{
					BadHete++;
					continue;
				}

				Max=(2*aa_count)+Het_count;			
				SeD=(2*bb_count)+Het_count;

				if (SeD<=Max)
				{
					NowMAF=(SeD*1.0/(SeD+Max));
					MaxSed=true ;
				}
				else
				{					
					NowMAF=(Max*1.0/(SeD+Max));
					MaxSed=false;
				}

				if ( NowMAF  < (para_18->MAF) )  
				{
					BadMAF++;
					continue;
				}

				double  p=SNPHWE(Het_count,aa_count,bb_count);
				if  (p< (para_18->HWE)) {BadHWE++ ;continue ;}
				Flag_for_pro++;



				mean= NowMAF*2.0;
				scale = NowMAF*(1.0 - NowMAF);

				if (MaxSed)
				{
					AA_PP=(0- mean);
					AB_PP=(1- mean);
					BB_PP=(2- mean);
				}
				else
				{
					AA_PP=(2- mean);
					AB_PP=(1- mean);
					BB_PP=(0- mean);
				}

				TowArry[BlockReadCount][0][0]= AA_PP*AA_PP;
				TowArry[BlockReadCount][0][1]= TowArry[BlockReadCount][1][0]=AA_PP*AB_PP;
				TowArry[BlockReadCount][0][2]= TowArry[BlockReadCount][2][0]=AA_PP*BB_PP;
				TowArry[BlockReadCount][1][2]= TowArry[BlockReadCount][2][1]=AB_PP*BB_PP;
				TowArry[BlockReadCount][2][2]= BB_PP*BB_PP;
				TowArry[BlockReadCount][1][1]= AB_PP*AB_PP;
				TowArryComCount[BlockReadCount][0][0]=TowArryComCount[BlockReadCount][0][1]=TowArryComCount[BlockReadCount][0][2]=scale;
				TowArryComCount[BlockReadCount][1][0]=TowArryComCount[BlockReadCount][1][1]=TowArryComCount[BlockReadCount][1][2]=scale;
				TowArryComCount[BlockReadCount][2][0]=TowArryComCount[BlockReadCount][2][1]=TowArryComCount[BlockReadCount][2][2]=scale;

				BlockReadCount++;
				if (BlockReadCount!=Block  &&  (!VCFIN.eof()))
				{
					continue;
				}


#pragma omp parallel for schedule (dynamic)  
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							CountPair[jj][kk]+=TowArryComCount[cc][BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
							Kinship[jj][kk]+=TowArry[cc][BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}
				BlockReadCount=0;

			}

			if (BlockReadCount>0)
			{
#pragma omp parallel for schedule (dynamic)  
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							CountPair[jj][kk]+=TowArryComCount[cc][BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
							Kinship[jj][kk]+=TowArry[cc][BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}
				BlockReadCount=0;

			}


		}
		else if ((paraFA04->Method)==3) // IBSKinshipImpute 
		{

			BlockReadCount=0;


			double table[Block][4][4];
			for (int cc=0; cc<Block ; cc++)
			{
				table[cc][0][0] = table[cc][1][1] = table[cc][2][2] = 2;
				table[cc][0][1] = table[cc][1][0] = table[cc][1][2] = table[cc][2][1] = table[cc][1][3] =table[cc][3][1] = 1;
				table[cc][0][2] = table[cc][2][0] = 0;
			}



			while(!VCFIN.eof())
			{
				string  line ;
				getline(VCFIN,line);
				if (line.length()<=0)  { continue  ; }

				spline.clear();
				subparser flds(line, '\t', subparser::SKIP_EMPTY);
				flds.split(spline);

				inf[0].assign(spline[0].begin(),spline[0].length());
				if (ChrSex.find(inf[0]) != ChrSex.end())
				{
					BadChrName++;
					continue ;
				}
				Base_len=spline[3].length();

				ALTstring.assign(spline[4].begin(),spline[4].length());
				Alt.clear();

				split(ALTstring,Alt,",");

				for (int ii=0 ; ii<Alt.size() ;ii++)
				{
					if (Alt[ii].length()>Base_len)
					{
						Base_len=Alt[ii].length();
					}
				}

				if (Base_len>1)
				{
					BadIndelSite++;
					continue ;
				}

				map <char,bool> CountBase;


				Het_count=0;
				Miss_count=0;
				aa_count=0;
				bb_count=0;


				for (int kk=0 ; kk< NumberSubGroup ; kk++)
				{
					if (spline[SampleSite[kk]][0] == '.')
					{
						BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=Miss_base.GenotypeValue;
						Miss_count++;
					}
					else
					{
						if (spline[SampleSite[kk]][0] != spline[SampleSite[kk]][2])
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=hete_base.GenotypeValue;
							Het_count++;
						}
						else if (spline[SampleSite[kk]][0] =='0')
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=best_base.GenotypeValue;
							aa_count++;
						}
						else
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=sed_base.GenotypeValue;
							bb_count++;
						}
						CountBase[spline[SampleSite[kk]][0]]=true;
						CountBase[spline[SampleSite[kk]][2]]=true;
					}
				}

				BaseConut=CountBase.size();

				if ( BaseConut!=2)
				{
					BadSite++;
					continue;
				}

				if  ( Miss_count>SampleNumMiss )
				{
					BadMiss++;
					continue;
				}

				if ( Het_count > SampleNumHete )
				{
					BadHete++;
					continue;
				}

				Max=(2*aa_count)+Het_count;			
				SeD=(2*bb_count)+Het_count;

				if (SeD<=Max)
				{
					NowMAF=(SeD*1.0/(SeD+Max));
					MaxSed=true ;
				}
				else
				{					
					NowMAF=(Max*1.0/(SeD+Max));
					MaxSed=false;
				}

				if ( NowMAF  < (para_18->MAF) )  
				{
					BadMAF++;
					continue;
				}

				double  p=SNPHWE(Het_count,aa_count,bb_count);
				if  (p< (para_18->HWE)) {BadHWE++ ;continue ;}
				Flag_for_pro++;


				if (MaxSed)
				{
					table[BlockReadCount][0][3] = table[BlockReadCount][3][0] = 2.0 * (1.0 - NowMAF);
					table[BlockReadCount][2][3] = table[BlockReadCount][3][2] = 2.0 * NowMAF;
					table[BlockReadCount][3][3] = 2.0 - 4.0 * NowMAF * (1 - NowMAF);
				}
				else
				{
					table[BlockReadCount][0][3] = table[BlockReadCount][3][0] = 2.0 * NowMAF;
					table[BlockReadCount][2][3] = table[BlockReadCount][3][2] = 2.0 * (1.0 - NowMAF);
					table[BlockReadCount][3][3] = 2.0 - 4.0 * NowMAF * (1 - NowMAF);
				}




				BlockReadCount++;
				if (BlockReadCount!=Block  &&  (!VCFIN.eof()))
				{
					continue;
				}



#pragma omp parallel for schedule(dynamic)
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj+1 ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							Kinship[jj][kk]+=table[cc][BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}


				BlockReadCount=0;

			}


			if  (BlockReadCount>0)
			{
#pragma omp parallel for schedule(dynamic)
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj+1 ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							Kinship[jj][kk]+=table[cc][BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}
				BlockReadCount=0;
			}



		}
		else if ((paraFA04->Method)==4) // IBSKinship
		{


			int ComCout[4][4]={1};
			ComCout[0][0]=ComCout[1][1]=ComCout[2][2]=1;
			ComCout[0][2]=ComCout[2][0]=1;
			ComCout[0][1]=ComCout[1][0]=ComCout[1][2]=ComCout[2][1]=1;
			ComCout[3][0]=ComCout[3][1]=ComCout[3][2]=ComCout[0][3]=ComCout[1][3]=ComCout[2][3]=ComCout[3][3]=0;


			double table[4][4];
			table[0][0] = table[1][1] = table[2][2] = 2;
			table[0][1] = table[1][0] = table[1][2] = table[2][1] = table[1][3] =table[3][1] = 1;
			table[0][2] = table[2][0] = 0;
			table[0][3] = table[3][0] =0;
			table[2][3] = table[3][2] =0;
			table[3][3] = 0;



			while(!VCFIN.eof())
			{
				string  line ;
				getline(VCFIN,line);
				if (line.length()<=0)  { continue  ; }

				spline.clear();
				subparser flds(line, '\t', subparser::SKIP_EMPTY);
				flds.split(spline);

				inf[0].assign(spline[0].begin(),spline[0].length());
				if (ChrSex.find(inf[0]) != ChrSex.end())
				{
					BadChrName++;
					continue ;
				}
				Base_len=spline[3].length();

				ALTstring.assign(spline[4].begin(),spline[4].length());
				Alt.clear();

				split(ALTstring,Alt,",");

				for (int ii=0 ; ii<Alt.size() ;ii++)
				{
					if (Alt[ii].length()>Base_len)
					{
						Base_len=Alt[ii].length();
					}
				}

				if (Base_len>1)
				{
					BadIndelSite++;
					continue ;
				}

				map <char,bool> CountBase;


				Het_count=0;
				Miss_count=0;
				aa_count=0;
				bb_count=0;


				for (int kk=0 ; kk< NumberSubGroup ; kk++)
				{
					if (spline[SampleSite[kk]][0] == '.')
					{
						BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=Miss_base.GenotypeValue;
						Miss_count++;
					}
					else
					{
						if (spline[SampleSite[kk]][0] != spline[SampleSite[kk]][2])
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=hete_base.GenotypeValue;
							Het_count++;
						}
						else if (spline[SampleSite[kk]][0] =='0')
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=best_base.GenotypeValue;
							aa_count++;
						}
						else
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=sed_base.GenotypeValue;
							bb_count++;
						}
						CountBase[spline[SampleSite[kk]][0]]=true;
						CountBase[spline[SampleSite[kk]][2]]=true;
					}
				}

				BaseConut=CountBase.size();

				if ( BaseConut!=2)
				{
					BadSite++;
					continue;
				}

				if  ( Miss_count>SampleNumMiss )
				{
					BadMiss++;
					continue;
				}

				if ( Het_count > SampleNumHete )
				{
					BadHete++;
					continue;
				}

				Max=(2*aa_count)+Het_count;			
				SeD=(2*bb_count)+Het_count;

				if (SeD<=Max)
				{
					NowMAF=(SeD*1.0/(SeD+Max));
					MaxSed=true ;
				}
				else
				{					
					NowMAF=(Max*1.0/(SeD+Max));
					MaxSed=false;
				}

				if ( NowMAF  < (para_18->MAF) )  
				{
					BadMAF++;
					continue;
				}

				double  p=SNPHWE(Het_count,aa_count,bb_count);
				if  (p< (para_18->HWE)) {BadHWE++ ;continue ;}
				Flag_for_pro++;




				BlockReadCount++;
				if (BlockReadCount!=Block  &&  (!VCFIN.eof()))
				{
					continue;
				}


#pragma omp parallel for schedule(dynamic)
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj+1 ; kk< NumberSubGroup ; kk++)
					{

						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							CountPair[jj][kk]+=ComCout[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
							Kinship[jj][kk]+=table[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}

				BlockReadCount=0;


			}


			if  (BlockReadCount>0)
			{
#pragma omp parallel for schedule(dynamic)
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj+1 ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							CountPair[jj][kk]+=ComCout[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
							Kinship[jj][kk]+=table[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}

				BlockReadCount=0;


			}



		}
		else if ((paraFA04->Method)==5) // p_dis.mat
		{


			int PP[4][4];
			PP[0][0]=PP[1][1]=PP[2][2]=PP[3][3]=0;
			PP[3][0]=PP[3][1]=PP[3][2]=PP[0][3]=PP[1][3]=PP[2][3]=0;
			PP[0][2]=PP[2][0]=2;
			PP[0][1]=PP[1][0]=PP[1][2]=PP[2][1]=1;

			int ComCout[4][4]={2};
			ComCout[0][0]=ComCout[1][1]=ComCout[2][2]=2;
			ComCout[3][0]=ComCout[3][1]=ComCout[3][2]=ComCout[0][3]=ComCout[1][3]=ComCout[2][3]=ComCout[3][3]=0;
			ComCout[0][2]=ComCout[2][0]=2;
			ComCout[0][1]=ComCout[1][0]=ComCout[1][2]=ComCout[2][1]=2;

			while(!VCFIN.eof())
			{
				string  line ;
				getline(VCFIN,line);
				if (line.length()<=0)  { continue  ; }

				spline.clear();
				subparser flds(line, '\t', subparser::SKIP_EMPTY);
				flds.split(spline);

				inf[0].assign(spline[0].begin(),spline[0].length());
				if (ChrSex.find(inf[0]) != ChrSex.end())
				{
					BadChrName++;
					continue ;
				}
				Base_len=spline[3].length();

				ALTstring.assign(spline[4].begin(),spline[4].length());
				Alt.clear();

				split(ALTstring,Alt,",");

				for (int ii=0 ; ii<Alt.size() ;ii++)
				{
					if (Alt[ii].length()>Base_len)
					{
						Base_len=Alt[ii].length();
					}
				}

				if (Base_len>1)
				{
					BadIndelSite++;
					continue ;
				}

				map <char,bool> CountBase;


				Het_count=0;
				Miss_count=0;
				aa_count=0;
				bb_count=0;


				for (int kk=0 ; kk< NumberSubGroup ; kk++)
				{
					if (spline[SampleSite[kk]][0] == '.')
					{
						BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=Miss_base.GenotypeValue;
						Miss_count++;
					}
					else
					{
						if (spline[SampleSite[kk]][0] != spline[SampleSite[kk]][2])
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=hete_base.GenotypeValue;
							Het_count++;
						}
						else if (spline[SampleSite[kk]][0] =='0')
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=best_base.GenotypeValue;
							aa_count++;
						}
						else
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=sed_base.GenotypeValue;
							bb_count++;
						}
						CountBase[spline[SampleSite[kk]][0]]=true;
						CountBase[spline[SampleSite[kk]][2]]=true;
					}
				}

				BaseConut=CountBase.size();

				if ( BaseConut!=2)
				{
					BadSite++;
					continue;
				}

				if  ( Miss_count>SampleNumMiss )
				{
					BadMiss++;
					continue;
				}

				if ( Het_count > SampleNumHete )
				{
					BadHete++;
					continue;
				}

				Max=(2*aa_count)+Het_count;			
				SeD=(2*bb_count)+Het_count;

				if (SeD<=Max)
				{
					NowMAF=(SeD*1.0/(SeD+Max));
					MaxSed=true ;
				}
				else
				{					
					NowMAF=(Max*1.0/(SeD+Max));
					MaxSed=false;
				}

				if ( NowMAF  < (para_18->MAF) )  
				{
					BadMAF++;
					continue;
				}

				double  p=SNPHWE(Het_count,aa_count,bb_count);
				if  (p< (para_18->HWE)) {BadHWE++ ;continue ;}
				Flag_for_pro++;




				BlockReadCount++;
				if (BlockReadCount!=Block  &&  (!VCFIN.eof()))
				{
					continue;
				}


#pragma omp parallel for schedule(dynamic)
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj+1 ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							Kinship[jj][kk]+=PP[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
							CountPair[jj][kk]+=ComCout[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}


				BlockReadCount=0;

			}

			if  (BlockReadCount>0)
			{
#pragma omp parallel for schedule(dynamic)
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj+1 ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							Kinship[jj][kk]+=PP[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
							CountPair[jj][kk]+=ComCout[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}
			}


		}


	}

	else
	{

		string OUT_VCFTMP=(paraFA04->InStr2)+".filter.vcf.gz";
		ogzstream OUTVCFFF ((OUT_VCFTMP).c_str());
		OUTVCFFF<<inf[0];
		for (int iik=1 ; iik<Asample ; iik++)
		{
			OUTVCFFF<<"\t"<<inf[iik];
		}
		OUTVCFFF<<endl;

		// /*//


		if ((paraFA04->Method)==1) //BaldingNicolsKinship //Normalized_IBS//Yang
		{

			double mean=0.0;
			double scale=0.0;

			double AA_PP=0.0;
			double BB_PP=0.0;
			double AB_PP=0.0; 



			double  TowArry[Block][4][4]={0};
			for (int cc=0; cc<Block ; cc++)
			{
				TowArry[cc][0][3] = TowArry[cc][1][3] = TowArry[cc][2][3] =TowArry[cc][3][0] =TowArry[cc][3][2] =TowArry[cc][3][1] = 0;
				TowArry[cc][3][3]=0;
			}


			int ComCout[4][4]={1};
			ComCout[0][0]=ComCout[1][1]=ComCout[2][2]=1;
			ComCout[3][0]=ComCout[3][1]=ComCout[3][2]=ComCout[0][3]=ComCout[1][3]=ComCout[2][3]=ComCout[3][3]=0;
			ComCout[0][2]=ComCout[2][0]=1;
			ComCout[0][1]=ComCout[1][0]=ComCout[1][2]=ComCout[2][1]=1;

			/*
			   struct timeval t1,t2;
			   double timeuse;
			   gettimeofday(&t1,NULL);
			   clock_t start,end;
			   start = clock();
			   */
			while(!VCFIN.eof())
			{
				string  line ;
				getline(VCFIN,line);
				if (line.length()<=0)  { continue  ; }

				spline.clear();
				subparser flds(line, '\t', subparser::SKIP_EMPTY);
				flds.split(spline);

				inf[0].assign(spline[0].begin(),spline[0].length());
				if (ChrSex.find(inf[0]) != ChrSex.end())
				{
					BadChrName++;
					continue ;
				}
				Base_len=spline[3].length();

				ALTstring.assign(spline[4].begin(),spline[4].length());
				Alt.clear();

				split(ALTstring,Alt,",");

				for (int ii=0 ; ii<Alt.size() ;ii++)
				{
					if (Alt[ii].length()>Base_len)
					{
						Base_len=Alt[ii].length();
					}
				}

				if (Base_len>1)
				{
					BadIndelSite++;
					continue ;
				}

				map <char,bool> CountBase;


				Het_count=0;
				Miss_count=0;
				aa_count=0;
				bb_count=0;


				for (int kk=0 ; kk< NumberSubGroup ; kk++)
				{
					if (spline[SampleSite[kk]][0] == '.')
					{
						BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=Miss_base.GenotypeValue;
						Miss_count++;
					}
					else
					{
						if (spline[SampleSite[kk]][0] != spline[SampleSite[kk]][2])
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=hete_base.GenotypeValue;
							Het_count++;
						}
						else if (spline[SampleSite[kk]][0] =='0')
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=best_base.GenotypeValue;
							aa_count++;
						}
						else
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=sed_base.GenotypeValue;
							bb_count++;
						}
						CountBase[spline[SampleSite[kk]][0]]=true;
						CountBase[spline[SampleSite[kk]][2]]=true;
					}
				}

				BaseConut=CountBase.size();

				if ( BaseConut!=2)
				{
					BadSite++;
					continue;
				}

				if  ( Miss_count>SampleNumMiss )
				{
					BadMiss++;
					continue;
				}

				if ( Het_count > SampleNumHete )
				{
					BadHete++;
					continue;
				}

				Max=(2*aa_count)+Het_count;			
				SeD=(2*bb_count)+Het_count;

				if (SeD<=Max)
				{
					NowMAF=(SeD*1.0/(SeD+Max));
					MaxSed=true ;
				}
				else
				{					
					NowMAF=(Max*1.0/(SeD+Max));
					MaxSed=false;
				}

				if ( NowMAF  < (para_18->MAF) )  
				{
					BadMAF++;
					continue;
				}

				double  p=SNPHWE(Het_count,aa_count,bb_count);
				if  (p< (para_18->HWE)) {BadHWE++ ;continue ;}
				Flag_for_pro++;
				OUTVCFFF<<line<<"\n";



				mean= NowMAF*2.0;
				scale = 1.0/(mean*(1.0 - NowMAF));

				if (MaxSed)
				{
					AA_PP=(0- mean);
					AB_PP=(1- mean);
					BB_PP=(2- mean);
				}
				else
				{
					AA_PP=(2- mean);
					AB_PP=(1- mean);
					BB_PP=(0- mean);
				}

				TowArry[BlockReadCount][0][0]= AA_PP*AA_PP*scale;
				TowArry[BlockReadCount][0][1]= TowArry[BlockReadCount][1][0]=AA_PP*AB_PP*scale;
				TowArry[BlockReadCount][0][2]= TowArry[BlockReadCount][2][0]=AA_PP*BB_PP*scale;
				TowArry[BlockReadCount][1][2]= TowArry[BlockReadCount][2][1]=AB_PP*BB_PP*scale;
				TowArry[BlockReadCount][2][2]= BB_PP*BB_PP*scale;
				TowArry[BlockReadCount][1][1]= AB_PP*AB_PP*scale;


				BlockReadCount++;
				if (BlockReadCount!=Block  &&  (!VCFIN.eof()))
				{
					continue;
				}

				//#pragma omp parallel for schedule (static)
				//#pragma omp parallel for schedule (guided)
#pragma omp parallel for schedule (dynamic)  
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							CountPair[jj][kk]+=ComCout[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
							Kinship[jj][kk]+=TowArry[cc][BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}
				BlockReadCount=0;

			}

			if (BlockReadCount>0)
			{
#pragma omp parallel for schedule (dynamic)  
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							CountPair[jj][kk]+=ComCout[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
							Kinship[jj][kk]+=TowArry[cc][BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}
				BlockReadCount=0;


			}


			/*
			   end = clock();
			   cout<<"F2运行时间"<<(double)(end-start)/CLOCKS_PER_SEC<<endl;

			   gettimeofday(&t2,NULL);
			   timeuse = (t2.tv_sec - t1.tv_sec) + (double)(t2.tv_usec - t1.tv_usec)/1000000.0;

			   cout<<"time = "<<timeuse<<endl;  //输出时间（单位：ｓ）
			   exit(1);
			   */
		}
		if ((paraFA04->Method)==2) //Centered_IBS//VanRaden
		{

			double mean=0.0;
			double scale=0.0;

			double AA_PP=0.0;
			double BB_PP=0.0;
			double AB_PP=0.0; 

			double  TowArry[Block][4][4]={0};
			double  TowArryComCount[Block][4][4]={0};
			for (int cc=0; cc<Block ; cc++)
			{
				TowArry[cc][0][3] = TowArry[cc][1][3] = TowArry[cc][2][3] =TowArry[cc][3][0] =TowArry[cc][3][2] =TowArry[cc][3][1] = 0;
				TowArry[cc][3][3]=0;
				TowArryComCount[cc][3][3]=0;
				TowArryComCount[cc][0][3] =TowArryComCount[cc][1][3] = TowArryComCount[cc][2][3] =TowArryComCount[cc][3][0] =TowArryComCount[cc][3][2] =TowArryComCount[cc][3][1] = 0;
			}



			while(!VCFIN.eof())
			{
				string  line ;
				getline(VCFIN,line);
				if (line.length()<=0)  { continue  ; }

				spline.clear();
				subparser flds(line, '\t', subparser::SKIP_EMPTY);
				flds.split(spline);

				inf[0].assign(spline[0].begin(),spline[0].length());
				if (ChrSex.find(inf[0]) != ChrSex.end())
				{
					BadChrName++;
					continue ;
				}
				Base_len=spline[3].length();

				ALTstring.assign(spline[4].begin(),spline[4].length());
				Alt.clear();

				split(ALTstring,Alt,",");

				for (int ii=0 ; ii<Alt.size() ;ii++)
				{
					if (Alt[ii].length()>Base_len)
					{
						Base_len=Alt[ii].length();
					}
				}

				if (Base_len>1)
				{
					BadIndelSite++;
					continue ;
				}

				map <char,bool> CountBase;


				Het_count=0;
				Miss_count=0;
				aa_count=0;
				bb_count=0;


				for (int kk=0 ; kk< NumberSubGroup ; kk++)
				{
					if (spline[SampleSite[kk]][0] == '.')
					{
						BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=Miss_base.GenotypeValue;
						Miss_count++;
					}
					else
					{
						if (spline[SampleSite[kk]][0] != spline[SampleSite[kk]][2])
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=hete_base.GenotypeValue;
							Het_count++;
						}
						else if (spline[SampleSite[kk]][0] =='0')
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=best_base.GenotypeValue;
							aa_count++;
						}
						else
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=sed_base.GenotypeValue;
							bb_count++;
						}
						CountBase[spline[SampleSite[kk]][0]]=true;
						CountBase[spline[SampleSite[kk]][2]]=true;
					}
				}

				BaseConut=CountBase.size();

				if ( BaseConut!=2)
				{
					BadSite++;
					continue;
				}

				if  ( Miss_count>SampleNumMiss )
				{
					BadMiss++;
					continue;
				}

				if ( Het_count > SampleNumHete )
				{
					BadHete++;
					continue;
				}

				Max=(2*aa_count)+Het_count;			
				SeD=(2*bb_count)+Het_count;

				if (SeD<=Max)
				{
					NowMAF=(SeD*1.0/(SeD+Max));
					MaxSed=true ;
				}
				else
				{					
					NowMAF=(Max*1.0/(SeD+Max));
					MaxSed=false;
				}

				if ( NowMAF  < (para_18->MAF) )  
				{
					BadMAF++;
					continue;
				}

				double  p=SNPHWE(Het_count,aa_count,bb_count);
				if  (p< (para_18->HWE)) {BadHWE++ ;continue ;}
				Flag_for_pro++;
				OUTVCFFF<<line<<"\n";



				mean= NowMAF*2.0;
				scale = NowMAF*(1.0 - NowMAF);

				if (MaxSed)
				{
					AA_PP=(0- mean);
					AB_PP=(1- mean);
					BB_PP=(2- mean);
				}
				else
				{
					AA_PP=(2- mean);
					AB_PP=(1- mean);
					BB_PP=(0- mean);
				}

				TowArry[BlockReadCount][0][0]= AA_PP*AA_PP;
				TowArry[BlockReadCount][0][1]= TowArry[BlockReadCount][1][0]=AA_PP*AB_PP;
				TowArry[BlockReadCount][0][2]= TowArry[BlockReadCount][2][0]=AA_PP*BB_PP;
				TowArry[BlockReadCount][1][2]= TowArry[BlockReadCount][2][1]=AB_PP*BB_PP;
				TowArry[BlockReadCount][2][2]= BB_PP*BB_PP;
				TowArry[BlockReadCount][1][1]= AB_PP*AB_PP;
				TowArryComCount[BlockReadCount][0][0]=TowArryComCount[BlockReadCount][0][1]=TowArryComCount[BlockReadCount][0][2]=scale;
				TowArryComCount[BlockReadCount][1][0]=TowArryComCount[BlockReadCount][1][1]=TowArryComCount[BlockReadCount][1][2]=scale;
				TowArryComCount[BlockReadCount][2][0]=TowArryComCount[BlockReadCount][2][1]=TowArryComCount[BlockReadCount][2][2]=scale;

				BlockReadCount++;
				if (BlockReadCount!=Block  &&  (!VCFIN.eof()))
				{
					continue;
				}


#pragma omp parallel for schedule (dynamic)  
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							CountPair[jj][kk]+=TowArryComCount[cc][BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
							Kinship[jj][kk]+=TowArry[cc][BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}
				BlockReadCount=0;

			}

			if (BlockReadCount>0)
			{
#pragma omp parallel for schedule (dynamic)  
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							CountPair[jj][kk]+=TowArryComCount[cc][BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
							Kinship[jj][kk]+=TowArry[cc][BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}
				BlockReadCount=0;

			}


		}
		else if ((paraFA04->Method)==3) // IBSKinshipImpute 
		{

			BlockReadCount=0;


			double table[Block][4][4];
			for (int cc=0; cc<Block ; cc++)
			{
				table[cc][0][0] = table[cc][1][1] = table[cc][2][2] = 2;
				table[cc][0][1] = table[cc][1][0] = table[cc][1][2] = table[cc][2][1] = table[cc][1][3] =table[cc][3][1] = 1;
				table[cc][0][2] = table[cc][2][0] = 0;
			}



			while(!VCFIN.eof())
			{
				string  line ;
				getline(VCFIN,line);
				if (line.length()<=0)  { continue  ; }

				spline.clear();
				subparser flds(line, '\t', subparser::SKIP_EMPTY);
				flds.split(spline);

				inf[0].assign(spline[0].begin(),spline[0].length());
				if (ChrSex.find(inf[0]) != ChrSex.end())
				{
					BadChrName++;
					continue ;
				}
				Base_len=spline[3].length();

				ALTstring.assign(spline[4].begin(),spline[4].length());
				Alt.clear();

				split(ALTstring,Alt,",");

				for (int ii=0 ; ii<Alt.size() ;ii++)
				{
					if (Alt[ii].length()>Base_len)
					{
						Base_len=Alt[ii].length();
					}
				}

				if (Base_len>1)
				{
					BadIndelSite++;
					continue ;
				}

				map <char,bool> CountBase;


				Het_count=0;
				Miss_count=0;
				aa_count=0;
				bb_count=0;


				for (int kk=0 ; kk< NumberSubGroup ; kk++)
				{
					if (spline[SampleSite[kk]][0] == '.')
					{
						BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=Miss_base.GenotypeValue;
						Miss_count++;
					}
					else
					{
						if (spline[SampleSite[kk]][0] != spline[SampleSite[kk]][2])
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=hete_base.GenotypeValue;
							Het_count++;
						}
						else if (spline[SampleSite[kk]][0] =='0')
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=best_base.GenotypeValue;
							aa_count++;
						}
						else
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=sed_base.GenotypeValue;
							bb_count++;
						}
						CountBase[spline[SampleSite[kk]][0]]=true;
						CountBase[spline[SampleSite[kk]][2]]=true;
					}
				}

				BaseConut=CountBase.size();

				if ( BaseConut!=2)
				{
					BadSite++;
					continue;
				}

				if  ( Miss_count>SampleNumMiss )
				{
					BadMiss++;
					continue;
				}

				if ( Het_count > SampleNumHete )
				{
					BadHete++;
					continue;
				}

				Max=(2*aa_count)+Het_count;			
				SeD=(2*bb_count)+Het_count;

				if (SeD<=Max)
				{
					NowMAF=(SeD*1.0/(SeD+Max));
					MaxSed=true ;
				}
				else
				{					
					NowMAF=(Max*1.0/(SeD+Max));
					MaxSed=false;
				}

				if ( NowMAF  < (para_18->MAF) )  
				{
					BadMAF++;
					continue;
				}

				double  p=SNPHWE(Het_count,aa_count,bb_count);
				if  (p< (para_18->HWE)) {BadHWE++ ;continue ;}
				Flag_for_pro++;
				OUTVCFFF<<line<<"\n";


				if (MaxSed)
				{
					table[BlockReadCount][0][3] = table[BlockReadCount][3][0] = 2.0 * (1.0 - NowMAF);
					table[BlockReadCount][2][3] = table[BlockReadCount][3][2] = 2.0 * NowMAF;
					table[BlockReadCount][3][3] = 2.0 - 4.0 * NowMAF * (1 - NowMAF);
				}
				else
				{
					table[BlockReadCount][0][3] = table[BlockReadCount][3][0] = 2.0 * NowMAF;
					table[BlockReadCount][2][3] = table[BlockReadCount][3][2] = 2.0 * (1.0 - NowMAF);
					table[BlockReadCount][3][3] = 2.0 - 4.0 * NowMAF * (1 - NowMAF);
				}




				BlockReadCount++;
				if (BlockReadCount!=Block  &&  (!VCFIN.eof()))
				{
					continue;
				}



#pragma omp parallel for schedule(dynamic)
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj+1 ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							Kinship[jj][kk]+=table[cc][BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}


				BlockReadCount=0;

			}


			if  (BlockReadCount>0)
			{
#pragma omp parallel for schedule(dynamic)
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj+1 ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							Kinship[jj][kk]+=table[cc][BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}
				BlockReadCount=0;
			}



		}
		else if ((paraFA04->Method)==4) // IBSKinship
		{


			int ComCout[4][4]={1};
			ComCout[0][0]=ComCout[1][1]=ComCout[2][2]=1;
			ComCout[0][2]=ComCout[2][0]=1;
			ComCout[0][1]=ComCout[1][0]=ComCout[1][2]=ComCout[2][1]=1;
			ComCout[3][0]=ComCout[3][1]=ComCout[3][2]=ComCout[0][3]=ComCout[1][3]=ComCout[2][3]=ComCout[3][3]=0;


			double table[4][4];
			table[0][0] = table[1][1] = table[2][2] = 2;
			table[0][1] = table[1][0] = table[1][2] = table[2][1] = table[1][3] =table[3][1] = 1;
			table[0][2] = table[2][0] = 0;
			table[0][3] = table[3][0] =0;
			table[2][3] = table[3][2] =0;
			table[3][3] = 0;



			while(!VCFIN.eof())
			{
				string  line ;
				getline(VCFIN,line);
				if (line.length()<=0)  { continue  ; }

				spline.clear();
				subparser flds(line, '\t', subparser::SKIP_EMPTY);
				flds.split(spline);

				inf[0].assign(spline[0].begin(),spline[0].length());
				if (ChrSex.find(inf[0]) != ChrSex.end())
				{
					BadChrName++;
					continue ;
				}
				Base_len=spline[3].length();

				ALTstring.assign(spline[4].begin(),spline[4].length());
				Alt.clear();

				split(ALTstring,Alt,",");

				for (int ii=0 ; ii<Alt.size() ;ii++)
				{
					if (Alt[ii].length()>Base_len)
					{
						Base_len=Alt[ii].length();
					}
				}

				if (Base_len>1)
				{
					BadIndelSite++;
					continue ;
				}

				map <char,bool> CountBase;


				Het_count=0;
				Miss_count=0;
				aa_count=0;
				bb_count=0;


				for (int kk=0 ; kk< NumberSubGroup ; kk++)
				{
					if (spline[SampleSite[kk]][0] == '.')
					{
						BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=Miss_base.GenotypeValue;
						Miss_count++;
					}
					else
					{
						if (spline[SampleSite[kk]][0] != spline[SampleSite[kk]][2])
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=hete_base.GenotypeValue;
							Het_count++;
						}
						else if (spline[SampleSite[kk]][0] =='0')
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=best_base.GenotypeValue;
							aa_count++;
						}
						else
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=sed_base.GenotypeValue;
							bb_count++;
						}
						CountBase[spline[SampleSite[kk]][0]]=true;
						CountBase[spline[SampleSite[kk]][2]]=true;
					}
				}

				BaseConut=CountBase.size();

				if ( BaseConut!=2)
				{
					BadSite++;
					continue;
				}

				if  ( Miss_count>SampleNumMiss )
				{
					BadMiss++;
					continue;
				}

				if ( Het_count > SampleNumHete )
				{
					BadHete++;
					continue;
				}

				Max=(2*aa_count)+Het_count;			
				SeD=(2*bb_count)+Het_count;

				if (SeD<=Max)
				{
					NowMAF=(SeD*1.0/(SeD+Max));
					MaxSed=true ;
				}
				else
				{					
					NowMAF=(Max*1.0/(SeD+Max));
					MaxSed=false;
				}

				if ( NowMAF  < (para_18->MAF) )  
				{
					BadMAF++;
					continue;
				}

				double  p=SNPHWE(Het_count,aa_count,bb_count);
				if  (p< (para_18->HWE)) {BadHWE++ ;continue ;}
				Flag_for_pro++;
				OUTVCFFF<<line<<"\n";




				BlockReadCount++;
				if (BlockReadCount!=Block  &&  (!VCFIN.eof()))
				{
					continue;
				}


#pragma omp parallel for schedule(dynamic)
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj+1 ; kk< NumberSubGroup ; kk++)
					{

						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							CountPair[jj][kk]+=ComCout[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
							Kinship[jj][kk]+=table[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}

				BlockReadCount=0;


			}


			if  (BlockReadCount>0)
			{
#pragma omp parallel for schedule(dynamic)
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj+1 ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							CountPair[jj][kk]+=ComCout[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
							Kinship[jj][kk]+=table[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}

				BlockReadCount=0;


			}



		}
		else if ((paraFA04->Method)==5) // p_dis.mat
		{


			int PP[4][4];
			PP[0][0]=PP[1][1]=PP[2][2]=PP[3][3]=0;
			PP[3][0]=PP[3][1]=PP[3][2]=PP[0][3]=PP[1][3]=PP[2][3]=0;
			PP[0][2]=PP[2][0]=2;
			PP[0][1]=PP[1][0]=PP[1][2]=PP[2][1]=1;

			int ComCout[4][4]={2};
			ComCout[0][0]=ComCout[1][1]=ComCout[2][2]=2;
			ComCout[3][0]=ComCout[3][1]=ComCout[3][2]=ComCout[0][3]=ComCout[1][3]=ComCout[2][3]=ComCout[3][3]=0;
			ComCout[0][2]=ComCout[2][0]=2;
			ComCout[0][1]=ComCout[1][0]=ComCout[1][2]=ComCout[2][1]=2;

			while(!VCFIN.eof())
			{
				string  line ;
				getline(VCFIN,line);
				if (line.length()<=0)  { continue  ; }

				spline.clear();
				subparser flds(line, '\t', subparser::SKIP_EMPTY);
				flds.split(spline);

				inf[0].assign(spline[0].begin(),spline[0].length());
				if (ChrSex.find(inf[0]) != ChrSex.end())
				{
					BadChrName++;
					continue ;
				}
				Base_len=spline[3].length();

				ALTstring.assign(spline[4].begin(),spline[4].length());
				Alt.clear();

				split(ALTstring,Alt,",");

				for (int ii=0 ; ii<Alt.size() ;ii++)
				{
					if (Alt[ii].length()>Base_len)
					{
						Base_len=Alt[ii].length();
					}
				}

				if (Base_len>1)
				{
					BadIndelSite++;
					continue ;
				}

				map <char,bool> CountBase;


				Het_count=0;
				Miss_count=0;
				aa_count=0;
				bb_count=0;


				for (int kk=0 ; kk< NumberSubGroup ; kk++)
				{
					if (spline[SampleSite[kk]][0] == '.')
					{
						BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=Miss_base.GenotypeValue;
						Miss_count++;
					}
					else
					{
						if (spline[SampleSite[kk]][0] != spline[SampleSite[kk]][2])
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=hete_base.GenotypeValue;
							Het_count++;
						}
						else if (spline[SampleSite[kk]][0] =='0')
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=best_base.GenotypeValue;
							aa_count++;
						}
						else
						{
							BaseALL[BlockReadCount][SampleSite[kk]].GenotypeValue=sed_base.GenotypeValue;
							bb_count++;
						}
						CountBase[spline[SampleSite[kk]][0]]=true;
						CountBase[spline[SampleSite[kk]][2]]=true;
					}
				}

				BaseConut=CountBase.size();

				if ( BaseConut!=2)
				{
					BadSite++;
					continue;
				}

				if  ( Miss_count>SampleNumMiss )
				{
					BadMiss++;
					continue;
				}

				if ( Het_count > SampleNumHete )
				{
					BadHete++;
					continue;
				}

				Max=(2*aa_count)+Het_count;			
				SeD=(2*bb_count)+Het_count;

				if (SeD<=Max)
				{
					NowMAF=(SeD*1.0/(SeD+Max));
					MaxSed=true ;
				}
				else
				{					
					NowMAF=(Max*1.0/(SeD+Max));
					MaxSed=false;
				}

				if ( NowMAF  < (para_18->MAF) )  
				{
					BadMAF++;
					continue;
				}

				double  p=SNPHWE(Het_count,aa_count,bb_count);
				if  (p< (para_18->HWE)) {BadHWE++ ;continue ;}
				Flag_for_pro++;
				OUTVCFFF<<line<<"\n";




				BlockReadCount++;
				if (BlockReadCount!=Block  &&  (!VCFIN.eof()))
				{
					continue;
				}


#pragma omp parallel for schedule(dynamic)
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj+1 ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							Kinship[jj][kk]+=PP[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
							CountPair[jj][kk]+=ComCout[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}


				BlockReadCount=0;

			}

			if  (BlockReadCount>0)
			{
#pragma omp parallel for schedule(dynamic)
				for (int jj=0 ; jj< NumberSubGroup ; jj++)
				{
					for (int kk=jj+1 ; kk< NumberSubGroup ; kk++)
					{
						for (int cc=0; cc<BlockReadCount ; cc++)
						{
							Kinship[jj][kk]+=PP[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
							CountPair[jj][kk]+=ComCout[BaseALL[cc][SampleSite[jj]].GenotypeValue][BaseALL[cc][SampleSite[kk]].GenotypeValue];
						}
					}
				}
			}



		}

		// /*//


		OUTVCFFF.close();


	}




	VCFIN.close();


	for(int i = 0; i < Block ; i++)
	{
		delete [] BaseALL[i];
	}


	delete [] SampleSite;

	if(BadIndelSite!=0)
	{
		cout<<"#warning skip Indel site, there are total skip Indel sites number is : "<<BadIndelSite<<endl;
	}
	if (BadSite!=0)
	{
		cout<<"#Warning skip non bi-allelic(Singleton/ThreeMulti allelic) site, and total skip allelic sites number is :"<<BadSite<<endl;
	}
	if (BadMiss!=0)
	{
		cout<<"#Warning skip high missing site, and total skip allelic sites number is :"<<BadMiss<<endl;
	}
	if (BadHete!=0)
	{
		cout<<"#Warning skip high Heterozygosity site, and total skip allelic sites number is :"<<BadHete<<endl;
	}
	if (BadMAF!=0)
	{
		cout<<"#Warning skip low Minor Allele Frequency site, and total skip allelic sites number is :"<<BadMAF<<endl;
	}
	if (BadChrName!=0)
	{
		cout<<"#Warning skip Sex chr Site, and total skip allelic sites number is :"<<BadChrName<<endl;
	}
	if (BadHWE!=0)
	{
		cout<<"#Warning skip low Pvalue of Hardy-Weinberg Equilibrium site , and total skip allelic sites number is :"<<BadHWE<<endl;
	}
	return 1;
}



int ReadPreDealGenotype_IN( In3str1v * paraFA04, Para_18 * para_18 , vector <string> & SampleName , vector <int> & SampleSite )
{
	map <string ,int >  SubVetor;
	map <string ,int >  :: iterator it;
	if  (!(paraFA04->SubPop).empty())
	{
		igzstream SampleList ((paraFA04->SubPop).c_str(),ifstream::in);
		if (SampleList.fail())
		{
			cerr << "open Sub Group IN File error: "<<(paraFA04->SubPop)<<endl;
			return  0;
		}
		while(!SampleList.eof())
		{
			string  line ;
			getline(SampleList,line);
			if (line.length()<=0 || line[0] == '#' )  { continue  ; }
			if (!line.empty() && line[line.size() - 1] == '\r')  { line.erase(line.size() - 1);}
			if (line.length()<=0)  { continue  ; }
			vector <string> inf ;
			split(line,inf," \t");
			int A=inf.size();
			for(int ii=0 ; ii<A ; ii++)
			{
				it=SubVetor.find(inf[ii]);
				if (it==SubVetor.end())
				{
					SubVetor.insert(map <string ,int> ::value_type(inf[ii],0));
				}
			}
		}
		SampleList.close();
	}


	igzstream GenotypeIN ((paraFA04->InStr3).c_str(),ifstream::in);
	if (GenotypeIN.fail())
	{
		cerr << "open Genotype File IN File error: "<<(paraFA04->InStr3)<<endl;
		return  0;
	}


	while(!GenotypeIN.eof())
	{
		string  line ;
		getline(GenotypeIN,line);
		if (line.length()<=0 )  { continue  ; }
		else if (line[0] == '#' && line[1] == '#' )  { continue  ; }
		else if( line[0] == '#' && line[1] != '#')
		{
			SampleName.clear();
			if (!line.empty() && line[line.size() - 1] == '\r')  { line.erase(line.size() - 1);}
			split(line,SampleName," \t");
			if  ( SampleName[0]  != "#CHROM")
			{
				continue  ;
			}
			int A=SampleName.size();
			if  ((paraFA04->SubPop).empty())
			{
				for(int ii=2 ; ii< A ; ii++)
				{
					SampleSite.push_back(ii);
					it=SubVetor.find(SampleName[ii]);
					if (it==SubVetor.end())
					{
						SubVetor.insert(map <string ,int> ::value_type(SampleName[ii],1));
					}
					else
					{
						(it->second)++;
					}
				}
			}
			else
			{
				for(int ii=2 ; ii< A ; ii++)
				{
					it=SubVetor.find(SampleName[ii]);
					if (it!=SubVetor.end())
					{
						SampleSite.push_back(ii);
						(it->second)++;
					}
				}
			}
			break ;
		}
		else if ( line[0] != '#' && line[1] != '#' )
		{
			cerr<<"wrong Line : "<<line<<endl;
			cerr<<"Genotype Header something wrong, can't find sample info before site info"<<endl;
			cerr<<"iTools   Formtools  VCF2Genotype  -InPut  in.vcf  -OutPut  out.genotype  -WithHeader   -NoRef"<<endl;
			cerr<<"Genotype Header sample info Flag : [  #CHROM  ] "<<endl;
			return  0;
			break;
		}
	}

	int NumberSubGroup=SampleSite.size();
	if  (!(paraFA04->SubPop).empty())
	{
		cout<<"the Number of SubPop samples[found in Genotype] is "<<NumberSubGroup<<endl;
	}
	if (NumberSubGroup<3)
	{
		cerr<<"sub Group Population szie is too small, SubGroup sample size: "<<NumberSubGroup<<endl;
		return  0;
	}


	for(it=SubVetor.begin(); it!=SubVetor.end() ;  it++)
	{
		if ((it->second)==0)
		{
			cerr<<"warning : Sample [ "<<(it->first)<<" ] can't be found in the Genotype Header\n";
		}
		else if ((it->second)>1)
		{
			cerr<<"warning : Sample [ "<<(it->first)<<" ] can be found [Repeat] in the Genotype Header\n";
		}
	}


	return 1;
}


int OutPutMat(In3str1v * paraFA04, Para_18 * para_18 , MatrixXd  & Kinship ,int & Flag_for_pro, vector <int> & SampleSite, vector <string> &  SampleName ,   double  **  CountPairArry , double ** KinshipArry)
{
	int sampleNumber=SampleSite.size();

	if ((paraFA04->Method)==1)  // BaldingNicolsKinship
	{
		string OutMatFile=(paraFA04->InStr2)+".Normalized_IBS.Kinship";
		ofstream  OUT (OutMatFile.c_str());
		if((!OUT.good()))
		{
			cerr << "open OUT File error: "<<OutMatFile<<endl;
			return  0;
		}	
		cout<<"Start To Create Normalized_IBS/Yang/BaldingNicolsKinship ..."<<endl;
		OUT<<setw(5)<<sampleNumber<<endl;
		for (int jj=0 ; jj< sampleNumber ;jj++ )
		{
			OUT<<setiosflags(ios::left)<<setw(12)<<SampleName[SampleSite[jj]];
			for (int kk=0 ; kk< sampleNumber ;kk++)
			{
				if  (jj>kk)
				{
					double p_dis=Kinship(kk,jj);
					OUT<<setiosflags(ios::fixed)<<setprecision(6)<<"\t"<<p_dis;
					Kinship(jj,kk)=p_dis;
				}
				else
				{
					double Diff_here=KinshipArry[jj][kk];
					int Count=CountPairArry[jj][kk];
					double p_dis=Diff_here*1.0/Count;
					OUT<<setiosflags(ios::fixed)<<setprecision(6)<<"\t"<<p_dis;
					Kinship(jj,kk)=p_dis;
				}
			}
			OUT<<endl;
		}
		OUT.close();
	}
	else if ((paraFA04->Method)==2)  //Centered_IBS//VanRaden
	{
		string OutMatFile=(paraFA04->InStr2)+".Centered_IBS.Kinship";
		ofstream  OUT (OutMatFile.c_str());
		if((!OUT.good()))
		{
			cerr << "open OUT File error: "<<OutMatFile<<endl;
			return  0;
		}	
		cout<<"Start To Create Centered_IBS/VanRaden ..."<<endl;
		OUT<<setw(5)<<sampleNumber<<endl;
		for (int jj=0 ; jj< sampleNumber ;jj++ )
		{
			OUT<<setiosflags(ios::left)<<setw(12)<<SampleName[SampleSite[jj]];
			for (int kk=0 ; kk< sampleNumber ;kk++)
			{
				if  (jj>kk)
				{
					double p_dis=Kinship(kk,jj);
					OUT<<setiosflags(ios::fixed)<<setprecision(6)<<"\t"<<p_dis;
					Kinship(jj,kk)=p_dis;
				}
				else
				{
					double Diff_here=KinshipArry[jj][kk];
					double Count=CountPairArry[jj][kk];
					double p_dis=Diff_here*0.5/Count;
					OUT<<setiosflags(ios::fixed)<<setprecision(6)<<"\t"<<p_dis;
					Kinship(jj,kk)=p_dis;
				}
			}
			OUT<<endl;
		}
		OUT.close();
	}
	else if ((paraFA04->Method)==3)  // IBSKinshipImpute
	{
		string OutMatFile=(paraFA04->InStr2)+".IBSImpute.Kinship";
		ofstream  OUT (OutMatFile.c_str());
		if((!OUT.good()))
		{
			cerr << "open OUT File error: "<<OutMatFile<<endl;
			return  0;
		}	
		cout<<"Start To Create IBSKinshipImpute ..."<<endl;
		OUT<<setw(5)<<sampleNumber<<endl;
		for (int jj=0 ; jj< sampleNumber ;jj++ )
		{
			OUT<<setiosflags(ios::left)<<setw(12)<<SampleName[SampleSite[jj]];
			for (int kk=0 ; kk< sampleNumber ;kk++)
			{
				if  (jj==kk)
				{
					OUT<<setiosflags(ios::fixed)<<setprecision(6)<<"\t"<<1.0;
					Kinship(jj,kk)=1.0;
					continue ;
				}
				else if  (jj>kk)
				{
					double p_dis=Kinship(kk,jj);
					OUT<<setiosflags(ios::fixed)<<setprecision(6)<<"\t"<<p_dis;
					Kinship(jj,kk)=p_dis;
				}
				else
				{
					double Diff_here=KinshipArry[jj][kk];
					double p_dis=Diff_here*0.5/Flag_for_pro;
					OUT<<setiosflags(ios::fixed)<<setprecision(6)<<"\t"<<p_dis;
					Kinship(jj,kk)=p_dis;
				}
			}
			OUT<<endl;
		}
		OUT.close();
	}
	else if ((paraFA04->Method)==4)  // IBSKinship
	{
		string OutMatFile=(paraFA04->InStr2)+".IBS.Kinship";
		ofstream  OUT (OutMatFile.c_str());
		if((!OUT.good()))
		{
			cerr << "open OUT File error: "<<OutMatFile<<endl;
			return  0;
		}	
		cout<<"Start To Create IBSKinship ..."<<endl;
		OUT<<setw(5)<<sampleNumber<<endl;
		for (int jj=0 ; jj< sampleNumber ;jj++ )
		{
			OUT<<setiosflags(ios::left)<<setw(12)<<SampleName[SampleSite[jj]];
			for (int kk=0 ; kk< sampleNumber ;kk++)
			{

				if  (jj==kk)
				{
					OUT<<setiosflags(ios::fixed)<<setprecision(6)<<"\t"<<1.0;
					Kinship(jj,kk)=1.0;
					continue ;
				}
				else if  (jj>kk)
				{
					double p_dis=Kinship(kk,jj);
					OUT<<setiosflags(ios::fixed)<<setprecision(6)<<"\t"<<p_dis;
					Kinship(jj,kk)=p_dis;

				}
				else
				{
					double Diff_here=KinshipArry[jj][kk];
					int Count=CountPairArry[jj][kk];
					double p_dis=Diff_here*0.5/Count;
					OUT<<setiosflags(ios::fixed)<<setprecision(6)<<"\t"<<p_dis;
					Kinship(jj,kk)=p_dis;
				}
			}
			OUT<<endl;
		}
		OUT.close();
	}
	else if ((paraFA04->Method)==5)
	{
		string OutMatFile=(paraFA04->InStr2)+".p_dis";
		ofstream  OUT (OutMatFile.c_str());
		if((!OUT.good()))
		{
			cerr << "open OUT File error: "<<OutMatFile<<endl;
			return  0;
		}	
		cout<<"Start To Create P_distance ..."<<endl;
		OUT<<setw(5)<<sampleNumber<<endl;
		for (int jj=0 ; jj< sampleNumber ;jj++ )
		{
			OUT<<setiosflags(ios::left)<<setw(12)<<SampleName[SampleSite[jj]];
			for (int kk=0 ; kk< sampleNumber ;kk++)
			{
				if  (jj==kk)
				{
					OUT<<setiosflags(ios::fixed)<<setprecision(6)<<"\t"<<0.0;
					Kinship(jj,kk)=0.0;
					continue ;
				}
				else if  (jj>kk)
				{
					double	p_dis=Kinship(kk,jj);
					OUT<<setiosflags(ios::fixed)<<setprecision(6)<<"\t"<<p_dis;
					Kinship(jj,kk)=p_dis;
				}
				else
				{
					int Count=CountPairArry[jj][kk];
					double Diff_here=KinshipArry[jj][kk];
					double p_dis=Diff_here*1.0/Count;
					OUT<<setiosflags(ios::fixed)<<setprecision(6)<<"\t"<<p_dis;
					Kinship(jj,kk)=p_dis;
				}
			}
			OUT<<endl;
		}
		OUT.close();
	}



	return 1;	
}


int Genotyp2KinShip(In3str1v * paraFA04, Para_18 * para_18 , double **  CountPair , double ** Kinship ,int & Flag_for_pro, vector <int> & SampleSite )
{
	map <string ,int >  SubVetor;
	map <string ,int >  :: iterator it;
	if  (!(paraFA04->SubPop).empty())
	{
		igzstream SampleList ((paraFA04->SubPop).c_str(),ifstream::in);
		if (SampleList.fail())
		{
			cerr << "open Sub Group IN File error: "<<(paraFA04->SubPop)<<endl;
			return  0;
		}
		while(!SampleList.eof())
		{
			string  line ;
			getline(SampleList,line);
			if (line.length()<=0 || line[0] == '#' )  { continue  ; }
			if (!line.empty() && line[line.size() - 1] == '\r')  { line.erase(line.size() - 1);}
			if (line.length()<=0)  { continue  ; }
			vector <string> inf ;
			split(line,inf," \t");
			int A=inf.size();
			for(int ii=0 ; ii<A ; ii++)
			{
				it=SubVetor.find(inf[ii]);
				if (it==SubVetor.end())
				{
					SubVetor.insert(map <string ,int> ::value_type(inf[ii],0));
				}
			}
		}
		SampleList.close();
	}


	igzstream GenotypeIN ((paraFA04->InStr3).c_str(),ifstream::in);
	if (GenotypeIN.fail())
	{
		cerr << "open Genotype File IN File error: "<<(paraFA04->InStr3)<<endl;
		return  0;
	}

	vector <string> inf ;

	while(!GenotypeIN.eof())
	{
		string  line ;
		getline(GenotypeIN,line);
		if (line.length()<=0 )  { continue  ; }
		else if (line[0] == '#' && line[1] == '#' )  { continue  ; }
		else if( line[0] == '#' && line[1] != '#')
		{
			inf.clear();
			if (!line.empty() && line[line.size() - 1] == '\r')  { line.erase(line.size() - 1);}
			split(line,inf," \t");
			if  ( inf[0]  != "#CHROM")
			{
				continue  ;
			}
			int A=inf.size();
			if  ((paraFA04->SubPop).empty())
			{
				for(int ii=2 ; ii< A ; ii++)
				{
					SampleSite.push_back(ii);
					it=SubVetor.find(inf[ii]);
					if (it==SubVetor.end())
					{
						SubVetor.insert(map <string ,int> ::value_type(inf[ii],1));
					}
					else
					{
						(it->second)++;
					}
				}
			}
			else
			{
				for(int ii=2 ; ii< A ; ii++)
				{
					it=SubVetor.find(inf[ii]);
					if (it!=SubVetor.end())
					{
						SampleSite.push_back(ii);
						(it->second)++;
					}
				}
			}
			break ;
		}
		else if ( line[0] != '#' && line[1] != '#' )
		{
			cerr<<"wrong Line : "<<line<<endl;
			cerr<<"Genotype Header something wrong, can't find sample info before site info"<<endl;
			cerr<<"iTools   Formtools  VCF2Genotype  -InPut  in.vcf  -OutPut  out.genotype  -WithHeader   -NoRef"<<endl;
			cerr<<"Genotype Header sample info Flag : [  #CHROM  ] "<<endl;
			return  0;
			break;
		}
	}

	int NumberSubGroup=SampleSite.size();
	if  (!(paraFA04->SubPop).empty())
	{
		cout<<"the Number of SubPop samples[found in Genotype] is "<<NumberSubGroup<<endl;
	}
	if (NumberSubGroup<3)
	{
		cerr<<"sub Group Population szie is too small, SubGroup sample size: "<<NumberSubGroup<<endl;
		return  0;
	}


	for(it=SubVetor.begin(); it!=SubVetor.end() ;  it++)
	{
		if ((it->second)==0)
		{
			cerr<<"warning : Sample [ "<<(it->first)<<" ] can't be found in the Genotype Header\n";
		}
		else if ((it->second)>1)
		{
			cerr<<"warning : Sample [ "<<(it->first)<<" ] can be found [Repeat] in the Genotype Header\n";
		}
	}


	int BadSite=0;
	int BadHWE=0;
	int BadMiss=0;
	int BadHete=0;
	int BadMAF=0;

	double table[4][4];
	table[0][0] = table[1][1] = table[2][2] = 2;
	table[0][1] = table[1][0] = table[1][2] = table[2][1] = table[1][3] =table[3][1] = 1;
	table[0][2] = table[2][0] = 0;
	double *PSite ;
	PSite= new double[NumberSubGroup];
	double mean=0.0;
	double scale=0.0; 
	double AA_PP=0.0;
	double BB_PP=0.0;
	double AB_PP=0.0; 




	int BadChrName=0;
	vector<string> ChrSexT ;
	split(para_18->FChrName,ChrSexT,",");
	map <string,int> ChrSex;
	for (int ii=0; ii<ChrSexT.size(); ii++)
	{
		ChrSex[ChrSexT[ii]]=ii;
	}



	map <string,string > SNP_back_Allele ;
	SNP_back_Allele["M"]="AC";SNP_back_Allele["K"]="GT";SNP_back_Allele["Y"]="CT";
	SNP_back_Allele["R"]="AG";SNP_back_Allele["W"]="AT";SNP_back_Allele["S"]="CG";
	SNP_back_Allele["C"]="CC";SNP_back_Allele["G"]="GG";SNP_back_Allele["T"]="TT";
	SNP_back_Allele["A"]="AA";
	SNP_back_Allele["-"]="NN"; SNP_back_Allele["N"]="NN";



	int Asample=inf.size();
	int BaseConut=0;
	char best_base='N';
	char sed_base='N';
	int Max=0;
	int SeD=0;



	while(!GenotypeIN.eof())
	{
		string  line ;
		getline(GenotypeIN,line);
		if (line.length()<=0  )  { continue  ; }
		llong Site ;
		//	inf.clear();		split(line,inf," \t");
		istringstream isoneLine (line,istringstream::in);
		for (int iik=0 ; iik<Asample ; iik++)
		{
			isoneLine>>inf[iik];
		}

		if (ChrSex.find(inf[0]) != ChrSex.end())			
		{
			BadChrName++;
			continue ;
		}

		map <char,int > Count;
		int Het_count=0;
		int Miss_count=0;


		for (int kk=0 ; kk< NumberSubGroup ; kk++)
		{
			string Genotype=SNP_back_Allele[inf[SampleSite[kk]]];
			char ABase=Genotype[0];
			if (  ABase == 'N' )
			{
				Miss_count++ ;
			}
			else
			{
				char BBase=Genotype[1];
				if  (ABase != BBase )
				{
					Het_count++;
				}
				Count[ABase]++;
				Count[BBase]++;
			}
		}


		if ( ( (Miss_count*1.0/NumberSubGroup)  >(para_18->Miss)  )  )
		{
			BadMiss++;
			continue ;
		}

		if ( ( (Het_count*1.0/NumberSubGroup)  >(para_18->Het) )  )
		{
			BadHete++;
			continue ;
		}



		BaseConut=0;
		best_base='N';
		sed_base='N';
		Max=0;
		SeD=0;
		map <char,int>  :: iterator it=Count.begin();

		for ( ; it!=Count.end(); it++ )
		{
			if ( (it->first ) == 'N' )
			{
				continue ;
			}
			else if ((it->second)  > Max )
			{
				SeD=Max;
				sed_base=best_base;
				Max=(it->second);
				best_base=it->first;
			}
			else if ( (it->second)  >= SeD )
			{
				SeD=(it->second);
				sed_base=it->first;
			}
			BaseConut++;
		}
		if (BaseConut!=2 )
		{
			BadSite++;
			continue ;
		}
		double NowMAF=SeD*1.0/(SeD+Max);
		if ( NowMAF  < (para_18->MAF) )  
		{
			BadMAF++;
			continue;
		}
		int aa_count= (Max-Het_count)/2;
		int bb_count= (SeD-Het_count)/2;
		double  p=SNPHWE(Het_count,aa_count,bb_count);
		if  (p< (para_18->HWE)) { BadHWE++ ;continue ;}





		if ((paraFA04->Method)==1) // BaldingNicolsKinship
		{

			mean= NowMAF*2.0;
			scale = sqrt(1.0/(mean*(1.0 - mean/2.0)));
			AA_PP=(0- mean)*scale;
			BB_PP=(2- mean)*scale;
			AB_PP=(1- mean)*scale;


			for (int jj=0 ; jj< NumberSubGroup ; jj++)
			{
				if  ( SNP_back_Allele[inf[SampleSite[jj]]][0] == 'N' )
				{
					PSite[jj]=0.0;
				}
				else if  ( (SNP_back_Allele[inf[SampleSite[jj]]][0] == best_base )  && (SNP_back_Allele[inf[SampleSite[jj]]][1] ==  best_base))
				{
					PSite[jj]=AA_PP;
				}
				else if  ( (SNP_back_Allele[inf[SampleSite[jj]]][0] == sed_base )  && (SNP_back_Allele[inf[SampleSite[jj]]][1] == sed_base))
				{
					PSite[jj]=BB_PP;
				}
				else if (SNP_back_Allele[inf[SampleSite[jj]]][0]  != SNP_back_Allele[inf[SampleSite[jj]]][1])
				{
					PSite[jj]=AB_PP;
				}

			}


			for (int jj=0 ; jj< NumberSubGroup ; jj++)
			{

				if  ( SNP_back_Allele[inf[SampleSite[jj]]][0] == 'N' )
				{
					continue ;
				}

				for (int kk=jj ; kk< NumberSubGroup ; kk++)
				{
					if  ( SNP_back_Allele[inf[SampleSite[kk]]][0] == 'N' )
					{
						continue ;
					}
					Kinship[jj][kk]+=PSite[jj]*PSite[kk];
					CountPair[jj][kk]++;
				}
			}
		}



		else if ((paraFA04->Method)==2) // IBSKinshipImpute 
		{
			double p =NowMAF;
			table[0][3] = table[3][0] = 2.0 * (1.0 - p);
			table[2][3] = table[3][2] = 2.0 * p;
			table[3][3] = 2.0 - 4.0 * p * (1 - p);

			for (int jj=0 ; jj< NumberSubGroup ; jj++)
			{
				int AAA=3;
				if  ( (SNP_back_Allele[inf[SampleSite[jj]]][0] == best_base )  && (SNP_back_Allele[inf[SampleSite[jj]]][1] ==  best_base))
				{
					AAA=0;
				}
				else if  ( (SNP_back_Allele[inf[SampleSite[jj]]][0] == sed_base )  && (SNP_back_Allele[inf[SampleSite[jj]]][1]== sed_base ))
				{
					AAA=2;
				}
				else if ( SNP_back_Allele[inf[SampleSite[jj]]][0]  != SNP_back_Allele[inf[SampleSite[jj]]][1])
				{
					AAA=1;
				}

				for (int kk=jj+1 ; kk< NumberSubGroup ; kk++)
				{
					int BBB=3;
					if  ( (SNP_back_Allele[inf[SampleSite[kk]]][0] == best_base )  && ( SNP_back_Allele[inf[SampleSite[kk]]][1] ==  best_base))
					{
						BBB=0;
					}
					else if  ( (SNP_back_Allele[inf[SampleSite[kk]]][0] == sed_base )  && (SNP_back_Allele[inf[SampleSite[kk]]][1] == sed_base))
					{
						BBB=2;
					}
					else if ( SNP_back_Allele[inf[SampleSite[kk]]][0] !=  SNP_back_Allele[inf[SampleSite[kk]]][1] )
					{
						BBB=1;
					}
					Kinship[jj][kk]+=table[AAA][BBB];
				}
			}
		}
		else if ((paraFA04->Method)==3) // IBSKinship
		{


			for (int jj=0 ; jj< NumberSubGroup ; jj++)
			{
				if  ( SNP_back_Allele[inf[SampleSite[jj]]][0] == 'N' )
				{
					continue ;
				}
				int AAA=0;
				if  ( (SNP_back_Allele[inf[SampleSite[jj]]][0] == best_base )  && (SNP_back_Allele[inf[SampleSite[jj]]][1] ==  best_base))
				{
					AAA=0;
				}
				else if  ( (SNP_back_Allele[inf[SampleSite[jj]]][0] == sed_base )  && (SNP_back_Allele[inf[SampleSite[jj]]][1] == sed_base))
				{
					AAA=2;
				}
				else if ( SNP_back_Allele[inf[SampleSite[jj]]][0]  != SNP_back_Allele[inf[SampleSite[jj]]][1])
				{
					AAA=1;
				}

				for (int kk=jj+1 ; kk< NumberSubGroup ; kk++)
				{
					if  ( SNP_back_Allele[inf[SampleSite[kk]]][0] == 'N' )
					{
						continue ;
					}
					int BBB=0;
					if  ( (SNP_back_Allele[inf[SampleSite[kk]]][0]== best_base )  && (SNP_back_Allele[inf[SampleSite[kk]]][1] ==  best_base))
					{
						BBB=0;
					}
					else if  ( (SNP_back_Allele[inf[SampleSite[kk]]][0] == sed_base )  && (SNP_back_Allele[inf[SampleSite[kk]]][1] == sed_base))
					{
						BBB=2;
					}
					else if ( SNP_back_Allele[inf[SampleSite[kk]]][0] !=  SNP_back_Allele[inf[SampleSite[kk]]][1] )
					{
						BBB=1;
					}
					Kinship[jj][kk]+=table[AAA][BBB];
					CountPair[jj][kk]++;
				}
			}
		}

		else if ((paraFA04->Method)==4) // p_dis.mat
		{
			for (int jj=0 ; jj< NumberSubGroup ; jj++)
			{
				if  ( SNP_back_Allele[inf[SampleSite[jj]]][0] == 'N' )
				{
					continue ;
				}
				for (int kk=jj+1 ; kk< NumberSubGroup ; kk++)
				{
					if  (SNP_back_Allele[inf[SampleSite[kk]]][0] == 'N')
					{
						continue ;
					}
					if  (SNP_back_Allele[inf[SampleSite[jj]]][0]  != SNP_back_Allele[inf[SampleSite[kk]]][0])
					{
						Kinship[jj][kk]++;
					}
					if  (SNP_back_Allele[inf[SampleSite[jj]]][1]  != SNP_back_Allele[inf[SampleSite[kk]]][1])
					{
						Kinship[jj][kk]++;
					}
					CountPair[jj][kk]+=2;
				}
			}
		}


		Flag_for_pro++;
		delete [] PSite ;
	}


	if (BadSite!=0)
	{
		cout<<"#Warning skip non bi-allelic(Singleton/ThreeMulti allelic) site, and total skip allelic sites number is :"<<BadSite<<endl;
	}
	if (BadMiss!=0)
	{
		cout<<"#Warning skip high missing site, and total skip allelic sites number is :"<<BadMiss<<endl;
	}
	if (BadHete!=0)
	{
		cout<<"#Warning skip high Heterozygosity site, and total skip allelic sites number is :"<<BadHete<<endl;
	}
	if (BadMAF!=0)
	{
		cout<<"#Warning skip low Minor Allele Frequency site, and total skip allelic sites number is :"<<BadMAF<<endl;
	}
	if (BadChrName!=0)
	{
		cout<<"#Warning skip Sex chr Site, and total skip allelic sites number is :"<<BadChrName<<endl;
	}
	if (BadHWE!=0)
	{
		cout<<"#Warning skip low Pvalue of Hardy-Weinberg Equilibrium site , and total skip allelic sites number is :"<<BadHWE<<endl;
	}


	return 1;
}





/////*        PreDeal  VCF  Deal //////////////////////*//

int PreDealVCF(In3str1v * paraFA04, Para_18 * para_18 , bool  &  TTFF ,vector <string> & SampleName , vector <int> &  SampleSite )
{
	map <string ,int >  SubVetor;
	map <string ,int >  :: iterator it; 
	if (!(paraFA04->SubPop).empty())
	{
		igzstream SampleList ((paraFA04->SubPop).c_str(),ifstream::in);
		if (SampleList.fail())
		{
			cerr << "open Sub Group IN File error: "<<(paraFA04->SubPop)<<endl;
			return  0;
		}

		while(!SampleList.eof())
		{
			string  line ;
			getline(SampleList,line);
			if (line.length()<=0 || line[0] == '#' )  { continue  ; }
			if (!line.empty() && line[line.size() - 1] == '\r')  { line.erase(line.size() - 1);}
			if (line.length()<=0)  { continue  ; }
			vector<string> inf;
			split(line,inf," \t");
			int A=inf.size();
			for(int ii=0 ; ii<A; ii++)
			{
				it=SubVetor.find(inf[ii]);
				if (it==SubVetor.end())
				{
					SubVetor.insert(map <string ,int> ::value_type(inf[ii],0));
				}
			}
		}
		SampleList.close();
	}

	igzstream VCFINTT ((paraFA04->InStr1).c_str(),ifstream::in);


	if (VCFINTT.fail())
	{
		cerr << "open VCF File IN File error: "<<(paraFA04->InStr1)<<endl;
		return  0;
	}


	while(!VCFINTT.eof())
	{
		string  line ;
		getline(VCFINTT,line);
		if (line.length()<=0 )  { continue  ; }
		else if ( line[0] == '#' && line[1] == '#' )  { continue  ; }
		else if ( line[0] == '#' && line[1] != '#' )
		{
			SampleName.clear();
			if (!line.empty() && line[line.size() - 1] == '\r')  { line.erase(line.size() - 1);}
			split(line,SampleName," \t");
			if  ( SampleName[0]  != "#CHROM")
			{
				continue  ;
			}
			int A=SampleName.size();
			if ((paraFA04->SubPop).empty())
			{
				for (int ii=9 ; ii< A; ii++)
				{
					it=SubVetor.find(SampleName[ii]);
					if (it!=SubVetor.end())
					{
						(it->second)++;
					}
					else
					{
						SubVetor.insert(map <string ,int> ::value_type(SampleName[ii],1));
					}
					SampleSite.push_back(ii);
				}
			}
			else
			{
				for (int ii=9 ; ii< A; ii++)
				{
					it=SubVetor.find(SampleName[ii]);
					if (it!=SubVetor.end())
					{
						SampleSite.push_back(ii);
						(it->second)++;
					}
				}
			}

			vector<string> inf;
			getline(VCFINTT,line);
			split(line,inf," \t");
			if ((inf[9])[1] == '|') 
			{
				TTFF=true;
				//cout <<"#Detected VCF File is phased file with '|', Read VCF in Phase mode"<<endl;
			}
			break ;
		}
		else if ( line[0] != '#' && line[1] != '#' )
		{
			cerr<<"wrong Line : "<<line<<endl;
			cerr<<"VCF Header something wrong, can't find sample info before site info"<<endl;
			cerr<<"VCF Header sample info Flag : [  #CHROM  ] "<<endl;
			return  0;
		}
	}
	VCFINTT.close();


	int NumberSubGroup=SampleSite.size();

	if (!(paraFA04->SubPop).empty())
	{
		cout<<"the Number of subPop samples[found in VCF] is "<<NumberSubGroup<<endl;
	}

	if (NumberSubGroup<3)
	{
		cerr<<"Group Population szie is too small, Group sample size: "<<NumberSubGroup<<endl;
		return  0;
	}

	for(it=SubVetor.begin(); it!=SubVetor.end() ;  it++)
	{
		if ((it->second)==0)
		{
			cerr<<"warning : Sample [ "<<(it->first)<<" ] can't be found in the VCF Header\n";
		}
		else if ((it->second)>1)
		{
			cerr<<"warning : Sample [ "<<(it->first)<<" ] can be found [Repeat] in the VCF Header\n";
		}
	}



	return 1;
	/////////////////////////////////////////////////////////

}


////////////

#endif // DataIn_H_ //
///////// swimming in the sky and flying in the sea ////////////




