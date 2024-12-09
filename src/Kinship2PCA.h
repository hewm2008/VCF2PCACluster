
#ifndef Kinship2PCA_H_
#define Kinship2PCA_H_

#include<iostream>
#include<algorithm>
#include<cstdlib>
#include<fstream>
#include <vector>
#include "./include/Eigen/Dense"
#include "K-means_cluster.h"
#include "EMGaussian_cluster.h"
#include "dbscan_cluster.h"

using namespace std;
using namespace Eigen;

//#define MINIMUM_POINTS 4     // minimum number of cluster
//#define EPSILON (0.75*0.75)  // distance for clustering, metre^2


int Kinship2PCA (In3str1v * paraFA04, Para_18 * para_18 ,  MatrixXd & Xmat ,vector <int> & SampleSite, vector <string>  & SampleName )
{
	int SampleNum = SampleSite.size();
	SelfAdjointEigenSolver<MatrixXd> eigensolver(Xmat);
	MatrixXd evec = (eigensolver.eigenvectors());
	VectorXd eval = eigensolver.eigenvalues();

	string eval_file = (paraFA04->InStr2) + ".eigenval";
	ofstream o_eval(eval_file.c_str());
	if (!o_eval)
	{
		cerr<<"cannot open the file [ "<< eval_file << " ] to read."<<endl;
		return 0;
	}
	double sumEval=0;
	for (int i = SampleNum - 1; i >= 0; i--)
	{
		sumEval+= eval(i);
	}
	o_eval<<"#eval\teval%\n";
	//	int CluseterMutiN=0;
	for (int i = SampleNum - 1; i >= 0; i--)
	{
		double  pp= eval(i)*100/sumEval;
		o_eval<<setiosflags(ios::fixed)<<setprecision(5) << eval(i) <<"\t"<<pp<<endl;
		//		if (pp>10){CluseterMutiN++;}
	}
	o_eval.close();

	/*
	   if (CluseterMutiN==0)
	   {
	   CluseterMutiN=1;
	   }
	   */

	cout << "Eigenvalues of " << SampleNum << " individuals have been saved in [ " << eval_file << " ]." << endl;
	string evec_file = (paraFA04->InStr2) + ".eigenvec";
	ofstream o_evec(evec_file.c_str());
	if (!o_evec) 
	{
		cout<<"cannot open the file [ " << evec_file << " ] to read."<<endl;
		return 0;
	}
	int OUTPCANum=paraFA04->PCANum ;
	if (OUTPCANum > SampleNum) {  OUTPCANum = SampleNum;}

	map <string,string> Group;
	int SampleNameSize=SampleName.size();
	for ( int ii=0 ; ii<SampleNameSize; ii++)
	{
		Group[SampleName[ii]]="UnGroup";
	}

	if (!(paraFA04->SampleGroup).empty())
	{
		igzstream ING ((paraFA04->SampleGroup).c_str(),ifstream::in); // ifstream  + gz
		if(!ING.good())
		{
			cerr << "open IN Sample Group File error: "<<(paraFA04->SampleGroup)<<endl;
			exit(1);
		}
		vector<string>  tmp;
		while(!ING.eof())
		{
			string  line ;
			getline(ING,line);
			if (!line.empty() && line[line.size() - 1] == '\r')  { line.erase(line.size() - 1);}
			if (line.length()<=0 )  { continue  ; }
			tmp.clear();
			split(line,tmp," \t");
			Group[tmp[0]]=tmp[1];
		}
		ING.close();
	}



	o_evec<<"SampleName\tGroup\tCluster";
	int PCACount=1;
	for (int j = SampleNum - 1; j >= (SampleNum - OUTPCANum); j--)
	{
		o_evec<<"\tPC"<<PCACount;
		PCACount++;
	}
	o_evec << endl;






	vector<Point> data3D ;
	vector<string> ClusterRepeat ;
	vector<double> SSENum;
	vector<double> DBiNum;
	SSENum.push_back(100);
	SSENum.push_back(100);
	DBiNum.push_back(10.0);
	DBiNum.push_back(10.0);
	double XXmax=-1000;
	double YYmax=-1000;
	double ZZmax=-1000;

	double XXmin=1000;
	double YYmin=1000;
	double ZZmin=1000;

	float error_limit = 0.000001;


	for (int i = 0; i < SampleNum; i++) 
	{
		Point a = {0,0,0,-1};
		int PCA1=SampleNum - 1;
		int PCA2=SampleNum - 2;
		int PCA3=SampleNum - 3;
		a.x = evec(i, PCA1);
		a.y = evec(i, PCA2);
		a.z = evec(i, PCA3);
		data3D.push_back(a);
		if (XXmax<a.x) { XXmax=a.x;}
		if (XXmin>a.x) { XXmin=a.x;}
		if (YYmax<a.y) { YYmax=a.y;}
		if (YYmin>a.y) { YYmin=a.y;}
		if (ZZmax<a.z) { ZZmax=a.z;}
		if (ZZmin>a.z) { ZZmin=a.z;}
		string NameThis=SampleName[SampleSite[i]];
		NameThis=NameThis+"\t"+Group[NameThis];
		ClusterRepeat.push_back(NameThis);
	}

	float XXMid=(XXmax+XXmin)*0.5;
	float YYMid=(YYmax+YYmin)*0.5;
	float ZZMid=(ZZmax+ZZmin)*0.5;

	//	standardize(data3D);


	if ( ( paraFA04->ClusterMethod)==1)
	{

		string cluster_file = (paraFA04->InStr2) + ".cluster";
		ofstream o_cluster(cluster_file.c_str());
		if (!o_cluster)
		{
			cout<<"cannot open the file [ " << cluster_file << " ] to write."<<endl;
			return 0;
		}
		o_cluster<<"#Sample\tGroup";



		if ((paraFA04->BestKManually) > ( paraFA04->ClusterNum) )  
		{
			(paraFA04->ClusterNum)=(paraFA04->BestKManually)+2;
		}

		if  (( paraFA04->ClusterNum)<3 )
		{
			(paraFA04->ClusterNum)=3;
		}
		else if (( paraFA04->ClusterNum) > SampleNum )
		{
			(paraFA04->ClusterNum)= SampleNum;
		}
		//	( paraFA04->ClusterNum)=3;

		for (int KK=2 ; KK<=( paraFA04->ClusterNum); KK++)
		{
			for (int i=0; i<data3D.size(); i++ )
			{
				data3D[i].clusterID=-1;
			}
			vector<Point> previous_cluster(KK,{XXMid,YYMid,ZZMid,-1});
			vector<Point> cluster(KK,{XXMid,YYMid,ZZMid,-1});
			if (paraFA04->RandomCenter)
			{
				initialize_clustera_V2(KK,cluster,data3D,XXmax,XXmin,YYmax,YYmin);
			}
			else
			{
				initialize_clustera(KK,cluster,data3D);
			}
			while(is_different(cluster,previous_cluster,error_limit))
			{
				assign_clusters(cluster,data3D);
				rep(i,0,KK)
				{
					previous_cluster[i] = cluster[i];
				}
				recalculate_clusters(cluster,data3D,KK);
			}
			assign_clusters(cluster,data3D);
			recalculate_clusters(cluster,data3D,KK);
			double AA=SSE(data3D,cluster);
			double DBi=Davies_Bouldin_index(data3D,cluster, KK);
			SSENum.push_back(AA);
			DBiNum.push_back(DBi);
			o_cluster<<"\tK="<<KK<<"(SSE="<<AA<<")";
			for (int i = 0; i < SampleNum; i++)
			{
				string NameThis=ClusterRepeat[i];
				if (data3D[i].clusterID==-1)
				{
					NameThis=NameThis+"\tUnCluster";
				}
				else
				{
					NameThis=NameThis+"\tCluster"+Int2Str(data3D[i].clusterID);
				}
				ClusterRepeat[i]=NameThis;
			}
		}


		o_cluster<<endl;

		for (int i = 0; i < SampleNum; i++)
		{
			o_cluster<<ClusterRepeat[i]<<endl;
		}
		o_cluster.close();




		double MaxSSE=SSENum[2];
		double MinSSE=SSENum[2];
		int BestkK=2;
		int BestKKDBi=2;
		int CountKK=3;

		string SSE_file = (paraFA04->InStr2) + ".evaluation";
		ofstream OUTSSE (SSE_file.c_str());
		if (!OUTSSE)
		{
			cout<<"cannot open the file [ " << SSE_file << " ] to write."<<endl;
			return 0;
		}
		OUTSSE<<"K\tSSE\tDBi\n2\t"<<SSENum[2]<<"\t"<<DBiNum[2]<<endl;


		//  find the best K //
		for (CountKK=3;CountKK<=(paraFA04->ClusterNum) ; CountKK++)
		{
			OUTSSE<<CountKK<<"\t"<<SSENum[CountKK]<<"\t"<<DBiNum[CountKK]<<"\n";
			if (MinSSE<SSENum[CountKK])
			{
				BestkK=CountKK-1;
			}
			else
			{
				MinSSE=SSENum[CountKK];
			}

			if (MaxSSE<SSENum[CountKK])
			{
				BestkK=CountKK-1;
				break;
			}
			if ( DBiNum[2] > DBiNum[CountKK] )
			{
				DBiNum[2]=DBiNum[CountKK];
				BestKKDBi=CountKK;
			}
		}
		OUTSSE.close();


		double binSSE=(MaxSSE-MinSSE)/(CountKK-2);
		binSSE=binSSE*(paraFA04->BestKRatio);
		for (int ii=3;ii<CountKK ; ii++)
		{
			double detaSSE=SSENum[ii-1]-SSENum[ii];
			if  (detaSSE<binSSE)
			{
				BestkK=ii-1;
				break;
			}
		}

		if ((paraFA04->BestKManually)!=-1)
		{
			BestkK=(paraFA04->BestKManually);
			cout<<"\t Kmean init K by input parameter, cluster K = " <<BestkK<<endl;
		}
		else
		{
			cout<<"\t[ SSE Method ] find the Best cluster K = "<<BestkK<<endl;
			cout<<"\t[ DBi Method ] find the Best cluster K = "<<BestKKDBi<<endl;
			if (BestkK!=BestKKDBi)
			{
				if (BestkK > BestKKDBi)
				{
					BestkK=BestKKDBi;
				}
				if (BestkK<2) {BestkK=1;}
				cout<<"\t Final we chose the minor, the Best cluster K = "<<BestkK<<endl;
			}
		}


		for (int i = 0; i < SampleNum; i++) 
		{
			string NameThis=SampleName[SampleSite[i]];
			vector <string> ClusArry;
			split(ClusterRepeat[i],ClusArry," \t");
			o_evec << NameThis << "\t" << Group[NameThis]<<"\t"<<ClusArry[BestkK];
			for (int j = SampleNum - 1; j >= (SampleNum - OUTPCANum); j--)
			{
				o_evec << "\t" << evec(i, j);
			}
			o_evec << endl;
		}

	}
	else if ( (paraFA04->ClusterMethod)==2)
	{

		int BestkK=(paraFA04->BestKManually);
		if (BestkK==-1)
		{
			if  (( paraFA04->ClusterNum)<3)
			{
				(paraFA04->ClusterNum)=3;
			}
			else if (( paraFA04->ClusterNum) > SampleNum )
			{
				(paraFA04->ClusterNum)= SampleNum;
			}
			for (int KK=2 ; KK<=( paraFA04->ClusterNum); KK++)
			{
				for (int i=0; i<data3D.size(); i++ )
				{
					data3D[i].clusterID=-1;
				}
				vector<Point> previous_cluster(KK,{XXMid,YYMid,ZZMid,-1});
				vector<Point> cluster(KK,{XXMid,YYMid,ZZMid,-1});
				if (paraFA04->RandomCenter)
				{
					initialize_clustera_V2(KK,cluster,data3D,XXmax,XXmin,YYmax,YYmin);
				}
				else
				{
					initialize_clustera(KK,cluster,data3D);
				}
				while(is_different(cluster,previous_cluster,error_limit))
				{
					assign_clusters(cluster,data3D);
					rep(i,0,KK)
					{
						previous_cluster[i] = cluster[i];
					}
					recalculate_clusters(cluster,data3D,KK);
				}
				assign_clusters(cluster,data3D);
				recalculate_clusters(cluster,data3D,KK);
				double AA=SSE(data3D,cluster);
				double DBi=Davies_Bouldin_index(data3D,cluster, KK);
				SSENum.push_back(AA);
				DBiNum.push_back(DBi);
			}


			double MaxSSE=SSENum[2];
			double MinSSE=SSENum[2];
			int BestKKDBi=2;
			int CountKK=3;

			//  find the best K //
			for (CountKK=3;CountKK<=(paraFA04->ClusterNum) ; CountKK++)
			{
				if (MinSSE<SSENum[CountKK])
				{
					BestkK=CountKK-1;
				}
				else
				{
					MinSSE=SSENum[CountKK];
				}
				if (MaxSSE<SSENum[CountKK])
				{
					BestkK=CountKK-1;
					break;
				}
				if ( DBiNum[2] > DBiNum[CountKK] )
				{
					DBiNum[2]=DBiNum[CountKK];
					BestKKDBi=CountKK;
				}
			}

			double binSSE=(MaxSSE-MinSSE)/(CountKK-2);
			binSSE=binSSE*(paraFA04->BestKRatio);
			for (int ii=3;ii<CountKK ; ii++)
			{
				double detaSSE=SSENum[ii-1]-SSENum[ii];
				if  (detaSSE<binSSE)
				{
					BestkK=ii-1;
					break;
				}
			}

			if (BestkK!=BestKKDBi)
			{
				if (BestkK > BestKKDBi)
				{
					BestkK=BestKKDBi;
				}
			}

			if (BestkK<2) {BestkK=1;}
			cout<<"\t DBSCAN init K by auto of K-mean, Best cluster K = "<<BestkK<<endl;
		}
		else
		{
			cout<<"\t DBSCAN init K by input parameter, cluster K = " <<BestkK<<endl;
		}

		double  binDeta=0;
		if ((paraFA04->Epsilon)==0)
		{
			(paraFA04->Epsilon) =pow((XXmax - XXmin)*0.5,2)+pow((YYmax - YYmin)*0.5,2)+pow((ZZmax - ZZmin)*0.5,2);
			(paraFA04->Epsilon) =FindBestEpsilon(paraFA04->MinPointNum,binDeta ,data3D );
		}

		cout<<"DBSCAN cluster with MinPointNum : " <<paraFA04->MinPointNum<<", and initial Epsilon :"<<(paraFA04->Epsilon)<<endl;
		double Epsilon_square=(paraFA04->Epsilon)*(paraFA04->Epsilon);
		DBSCAN ds(paraFA04->MinPointNum, Epsilon_square , data3D);
		int NumK=ds.run();
		if (NumK!=BestkK)
		{
			int TY_V1=0,TY_V2=0;
			if (binDeta==0) {binDeta=((paraFA04->Epsilon))/100;}
			else
			{
				binDeta=binDeta*0.1;
			}

			if (BestkK > NumK )
			{
				binDeta=0-binDeta ;
				TY_V1=1 ;
			}

			for (int ii=1 ;ii <100 ; ii++)
			{
				double ThisEpsilon=(paraFA04->Epsilon)+(binDeta*ii);
				Epsilon_square=ThisEpsilon*ThisEpsilon;
				ds.ReInitialization(paraFA04->MinPointNum,Epsilon_square);
				NumK=ds.run();
				if (BestkK == NumK)
				{
					break;
				}
				else if (BestkK>NumK)
				{
					TY_V2=1;
				}
				else
				{
					TY_V2=0;
				}
				if (TY_V2!=TY_V1)  
				{
					cout<<"DBSCAN reach the its' best K at K= "<<NumK<<endl;
					break ;
				}
			}
		}


		//Cluster"+Int2Str(data3D[i].clusterID);
		for (int i = 0; i < SampleNum; i++) 
		{
			string NameThis=SampleName[SampleSite[i]];
			vector <string> ClusArry;
			split(ClusterRepeat[i],ClusArry," \t");
			int clusterID=(ds.m_points[i]).clusterID;
			if (clusterID>0)
			{
				o_evec << NameThis << "\t" << Group[NameThis]<<"\tCluster"<<clusterID;
			}
			else
			{
				o_evec << NameThis << "\t" << Group[NameThis]<<"\tClusterNA";
			}

			for (int j = SampleNum - 1; j >= (SampleNum - OUTPCANum); j--)
			{
				o_evec << "\t" << evec(i, j);
			}
			o_evec << endl;
		}

	}
	else if ( ( paraFA04->ClusterMethod)==3 )  // EM Cluter
	{

		int BestkK=(paraFA04->BestKManually);
		if (BestkK==-1)
		{
			if  (( paraFA04->ClusterNum)<3 )
			{
				(paraFA04->ClusterNum)=3;
			}
			else if (( paraFA04->ClusterNum) > SampleNum )
			{
				(paraFA04->ClusterNum)= SampleNum;
			}
			for (int KK=2 ; KK<=( paraFA04->ClusterNum); KK++)
			{
				for (int i=0; i<data3D.size(); i++ )
				{
					data3D[i].clusterID=-1;
				}
				vector<Point> previous_cluster(KK,{XXMid,YYMid,ZZMid,-1});
				vector<Point> cluster(KK,{XXMid,YYMid,ZZMid,-1});
				if (paraFA04->RandomCenter)
				{
					initialize_clustera_V2(KK,cluster,data3D,XXmax,XXmin,YYmax,YYmin);
				}
				else
				{
					initialize_clustera(KK,cluster,data3D);
				}
				while(is_different(cluster,previous_cluster,error_limit))
				{
					assign_clusters(cluster,data3D);
					rep(i,0,KK)
					{
						previous_cluster[i] = cluster[i];
					}
					recalculate_clusters(cluster,data3D,KK);
				}
				assign_clusters(cluster,data3D);
				recalculate_clusters(cluster,data3D,KK);
				double AA=SSE(data3D,cluster);
				double DBi=Davies_Bouldin_index(data3D,cluster, KK);
				SSENum.push_back(AA);
				DBiNum.push_back(DBi);
			}



			double MaxSSE=SSENum[2];
			double MinSSE=SSENum[2];
			int BestKKDBi=2;
			int CountKK=3;

			//  find the best K //
			for (CountKK=3;CountKK<=(paraFA04->ClusterNum) ; CountKK++)
			{
				if (MinSSE<SSENum[CountKK])
				{
					BestkK=CountKK-1;
				}
				else
				{
					MinSSE=SSENum[CountKK];
				}
				if (MaxSSE<SSENum[CountKK])
				{
					BestkK=CountKK-1;
					break;
				}
				if ( DBiNum[2] > DBiNum[CountKK] )
				{
					DBiNum[2]=DBiNum[CountKK];
					BestKKDBi=CountKK;
				}
			}

			double binSSE=(MaxSSE-MinSSE)/(CountKK-2);
			binSSE=binSSE*(paraFA04->BestKRatio);
			for (int ii=3;ii<CountKK ; ii++)
			{
				double detaSSE=SSENum[ii-1]-SSENum[ii];
				if  (detaSSE<binSSE)
				{
					BestkK=ii-1;
					break;
				}
			}

			if (BestkK!=BestKKDBi)
			{
				if (BestkK > BestKKDBi)
				{
					BestkK=BestKKDBi;
				}
			}
			if (BestkK<2) {BestkK=1;}
			cout<<"\t EM init K by auto of K-mean, Best cluster K = "<<BestkK<<endl;
		}
		else
		{
			cout<<"\t EM init K by input parameter, cluster K = " <<BestkK<<endl;
		}

		int KK=BestkK;	
		vector<Point> cluster(BestkK,{XXMid,YYMid,ZZMid,-1});
		for (int i=0; i<data3D.size(); i++)
		{
			data3D[i].clusterID=-1;
		}
		vector<Point> previous_cluster(KK,{XXMid,YYMid,ZZMid,-1});
		if (paraFA04->RandomCenter)
		{
			initialize_clustera_V2(KK,cluster,data3D,XXmax,XXmin,YYmax,YYmin);
		}
		else
		{
			initialize_clustera(KK,cluster,data3D);
		}
		while(is_different(cluster,previous_cluster,error_limit))
		{
			assign_clusters(cluster,data3D);
			rep(i,0,KK)
			{
				previous_cluster[i] = cluster[i];
			}
			recalculate_clusters(cluster,data3D,KK);
		}
		assign_clusters(cluster,data3D);
		recalculate_clusters(cluster,data3D,KK);



		MatrixXf  CData(SampleNum,3) ;
		for (int i=0; i<data3D.size(); i++ )
		{
			CData(i,0)=data3D[i].x;
			CData(i,1)=data3D[i].y;
			CData(i,2)=data3D[i].z;
		}


		if ((paraFA04->Epsilon)>1e-10)
		{
			(paraFA04->Epsilon)=1e-10;
		}
			
		Cluster  EM (CData,BestkK,(paraFA04->Epsilon)) ;
		EM.assignCentres(cluster,data3D);
		EM.EMcluster(paraFA04->Iterations);



		for (int i = 0; i < SampleNum; i++) 
		{
			string NameThis=SampleName[SampleSite[i]];
			int AA=EM.assignment(i);
			if (AA!=-1)
			{
				o_evec << NameThis << "\t" << Group[NameThis]<<"\tCluster"<<EM.assignment(i);
			}
			else
			{
				o_evec << NameThis << "\t" << Group[NameThis]<<"\tClusterNA";
			}
			for (int j = SampleNum - 1; j >= (SampleNum - OUTPCANum); j--)
			{
				o_evec << "\t" << evec(i, j);
			}
			o_evec << endl;
		}


	}
	else if (( paraFA04->ClusterMethod)==8)
	{
		for (int i = 0; i < SampleNum; i++)
		{
			string NameThis=SampleName[SampleSite[i]];
			o_evec << NameThis << "\t" << Group[NameThis]<<"\tClusterNA";
			for (int j = SampleNum - 1; j >= (SampleNum - OUTPCANum); j--)
			{
				o_evec << "\t" << evec(i, j);
			}
			o_evec << endl;
		}
	}




	o_evec.close();
	cout << "The first " << OUTPCANum << " eigenvectors of " << SampleNum << " individuals have been saved in [ " << evec_file <<" ]." << endl;

	return 1;
}



int ReadKinship2PCA (In3str1v * paraFA04, Para_18 * para_18 , vector <int> & SampleSite , vector <string> &  SampleName )
{
	string input_file=paraFA04->InKinship ;
	igzstream IN (input_file.c_str(),ifstream::in); // ifstream  + gz
	if(!IN.good())
	{
		cerr << "open IN Kinship File error: "<<input_file<<endl;
		exit(1);
	}

	string  line ;
	while(1)
	{
		getline(IN,line);
		if (line[0] != '#')
		{
			break;
		}
		if (IN.eof())
		{
			break;
		}
	}

	vector<string> head;
	split(line,head," \t");

	if (line.length()<=0)
	{
		cerr << "open IN File Format error: "<<input_file<<endl;
		exit(1);
	}


	int m = atoi(head[0].c_str());
	int n = m;

	MatrixXd X(m, n) ;

	// read data
	int FlagCout=0;
	for (int i = 0; i < m; ++i)
	{
		getline(IN,line);
		vector<string> inf;
		split(line,inf," \t");
		for (int j = 1; j <= n; ++j)
		{
			X(i, j-1) = atof(inf[j].c_str());
		}
		SampleName.push_back(inf[0]);
		SampleSite.push_back(FlagCout);
		FlagCout++;
	}
	IN.close();


	Kinship2PCA ( paraFA04, para_18 , X , SampleSite,  SampleName );
	return 1;
}

#endif /* Mat2PCA_H_ */



