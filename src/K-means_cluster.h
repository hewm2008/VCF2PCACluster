#ifndef Kmeans_H_
#define Kmeans_H_


#include<algorithm>
#include<list> 
#include <bits/stdc++.h> 
#include<fstream>
#include <cmath>
#include "DataClass.h"

using namespace std;

//#define PI 3.14159265


#define rep(i,a,b) for(int i=a;i<b;i++)
using namespace std;


float distance(Point a, Point b)
{
	return sqrtf( powf((a.x - b.x),2) + powf((a.y - b.y),2) + powf((a.z - b.z),2) );
}


/*
   float NewSSE(vector<Point> &data,vector<Point> &cluster)
   {
   float sse=0;
   int  NN=data.size();
   float Count=0;
   for (int ii=0; ii<NN,ii++)
   {
   if(data[ii].clusterID!=-1)
   {
   for (int jj=0; jj<NN,jj++)
   {
   if (data[ii].clusterID!=data[jj].clusterID)
   {

   }
   if (ii==jj)
   {
   continue;
   }
   sse+=distance(data[ii],data[jj]);
   Count++;
   }
   }
   }
   if  (Count!=0)
   {
   sse=sse/Count;
   }
   return sse;
   }
   */

///*//
float SSE(vector<Point> &data,vector<Point> &cluster)
{
	float sse=0;
	int Count=0;
	rep(i,0,data.size())
	{
		if(data[i].clusterID!=-1)
		{
			sse += powf(distance(data[i],cluster[data[i].clusterID]),2);
			Count++;
		}
	}
	if  (Count!=0)
	{
		sse=sse/Count;
	}
	//this code was earlier used as i was iterating this process over various K and storing the error to find elbow point

	return sse;
}


/*
   The Davies–Bouldin index (DBI) (introduced by David L. Davies and 
   Donald W. Bouldin in 1979) is a metric for evaluating clustering algorithms. 
   This is an internal evaluation scheme, where the validation of how well the 
   clustering has been done is made using quantities and features inherent to the dataset.
   This is the function of K cluster calculation DBI developed by hewm2008
   Davies-Bouldin Index(戴维森堡丁指数)
   *///
double Davies_Bouldin_index(vector<Point> &data,vector<Point> &cluster,int KK)
{
	double DBi=0;
	int  NN=data.size();
	vector <double> SumDis (KK,0);
	vector <int> Count (KK,0);

	for (int ii=0; ii<NN ; ii++)
	{
		int NumCluster=data[ii].clusterID;
		if(NumCluster!=-1)
		{
			for (int jj=ii+1; jj<NN; jj++)
			{
				if (NumCluster==data[jj].clusterID)
				{
					SumDis[NumCluster]+=distance(data[ii],data[jj]);
					Count[NumCluster]++;
				}
			}
		}
	}

	for (int ii=0 ; ii<KK; ii++)
	{
		if  (Count[ii]!=0)
		{
			SumDis[ii]=SumDis[ii]/Count[ii];
		}
	}


	for (int ii=0 ; ii<KK; ii++)
	{
		double MaxEE=0;
		double index=0;
		for (int jj=ii+1; jj<KK ;jj++)
		{
			index =((SumDis[ii] + SumDis[jj]))/(distance(cluster[ii],cluster[jj]));
			if (MaxEE<index) {MaxEE=index;}
		}
		DBi+=MaxEE;
	}

	DBi=DBi/KK;

	return DBi;
}



//*/


//*///
void initialize_clustera(int K, vector<Point> &cluster, vector<Point> &data)
{
	unordered_map<int,int> index_visited;
	int cnt=0;
	while(cnt != K)
	{
		int z = rand()%(data.size());
		if(index_visited[z]!=1)
		{
			index_visited[z]=1;
			cluster[cnt] = data[z];
			cnt++;
		}
	}
}
///*///

// new initialize function  delelop by hewm2008  
void initialize_clustera_V2(int K, vector<Point> &cluster, vector<Point> &data, double  & XXmax , double  & XXmin ,double  & YYmax,double  & YYmin )
{

	double XXMid=(XXmax+XXmin)*0.5;
	double YYMid=(YYmax+YYmin)*0.5;
	//	double ZZMid=0;
	double Radius=abs(XXmax-XXMid)*0.70; // 0.7 is the rand num 
	double angle=360.0/K;
	double DetaYY=(YYmax-YYmin)*1.0/(XXmax-XXmin);
	double TmpPI=0.017453295;  // PI/180 
	int NN=data.size();

	double DetaAngle=angle/20 ;
	float min_Sumdist = FLT_MAX ;
	float max_Sundist =0;

	vector<Point> Precluster (K,{0,0,0,-1});

	for (int Time=0; Time<20 ; Time++)
	{

		unordered_map<int,int> index_visited;
		double DDtaAngle=DetaAngle*Time;
		double TmpComAngle=0;
		for (int cnt=0; cnt<K ; cnt++)
		{
			double AA=(angle*cnt+DDtaAngle)*TmpPI;
			double XX=XXMid+Radius*sin(AA);
			double YY=YYMid+Radius*cos(AA)*DetaYY;
			float min_dist = FLT_MAX;
			int ThisNj=K;
			for (int z=0 ; z< NN ; z++)
			{
				if(index_visited[z]!=1)
				{
					double Dis=sqrtf(powf((data[z].x-XX),2)+ powf((data[z].y - YY),2));
					if (min_dist>Dis)
					{
						ThisNj=z;
						min_dist=Dis;
					}
				}
			}
			Precluster[cnt] = data[ThisNj];
			index_visited[ThisNj]=1;
			TmpComAngle+=min_dist;
		}


		if (min_Sumdist>TmpComAngle)
		{
			min_Sumdist=TmpComAngle;
			for (int UU=0; UU<K; UU++)
			{
				cluster[UU] =Precluster[UU];
			}
		}

	}
}




void assign_clusters(vector<Point> &cluster, vector<Point> &data)
{
	rep(i,0,data.size())
	{
		float min_dist = FLT_MAX;

		rep(j,0,cluster.size())
		{
			float dist = distance(data[i], cluster[j]);
			if(dist < min_dist)
			{
				min_dist=dist;
				data[i].clusterID = j;
			}
		}
	}
}

void recalculate_clusters(vector<Point> &cluster, vector<Point> &data,int K)
{
	Point temp={0,0,0,-1};
	temp.x=0;
	temp.y=0;
	temp.z=0;
	vector<Point> cluster_sum(K,temp);
	vector<int> cluster_num(K,0);

	rep(i,0,data.size())
	{
		if(data[i].clusterID != -1)
		{
			cluster_sum[data[i].clusterID].x += data[i].x;
			cluster_sum[data[i].clusterID].y += data[i].y;
			cluster_sum[data[i].clusterID].z += data[i].z;
			cluster_num[data[i].clusterID]++;
		}
	}
	rep(i,0,K)
	{
		if(cluster_num[i]!=0)
		{
			cluster[i].x = cluster_sum[i].x / cluster_num[i];
			cluster[i].y = cluster_sum[i].y / cluster_num[i];
			cluster[i].z = cluster_sum[i].z / cluster_num[i];
		}
	}
}


bool is_different(vector<Point> &cluster,vector<Point> &previous_cluster,float error_limit)
{
	bool not_valid=false;
	rep(i,0,cluster.size())
	{
		if(cluster[i].x - previous_cluster[i].x > error_limit)
		{
			not_valid=true;
		}
		if(cluster[i].y - previous_cluster[i].y > error_limit)
		{
			not_valid=true;
		}
		if(cluster[i].z - previous_cluster[i].z > error_limit)
		{
			not_valid=true;
		}
	}
	return not_valid;
}

#endif 





