#ifndef DBSCAN_H
#define DBSCAN_H

#include <vector>
#include <cmath>
#include "DataClass.h"

#define UNCLASSIFIED -1
#define CORE_POINT 1
#define BORDER_POINT 2
#define NOISE -2
#define SUCCESS 0
#define FAILURE -3
//   code study and modify from this github: https://github.com/james-yoo/DBSCAN
using namespace std;


class DBSCAN {
public:    
    DBSCAN(unsigned int minPts, float eps, vector<Point> points){
        m_minPoints = minPts;
        m_epsilon = eps;
        m_points = points;
        m_pointSize = points.size();
    }
    ~DBSCAN(){}

    int run();
    vector<int> calculateCluster(Point point);
    int expandCluster(Point point, int clusterID);
    inline double calculateDistance(const Point& pointCore, const Point& pointTarget);
	int ReInitialization (unsigned int minPts, float eps)
	{
		m_minPoints = minPts;
		m_epsilon = eps;
		for (int ii=0 ;ii<m_pointSize ; ii++)
		{
			m_points[ii].clusterID =-1;
		}
	}

    int getTotalPointSize() {return m_pointSize;}
    int getMinimumClusterSize() {return m_minPoints;}
    int getEpsilonSize() {return m_epsilon;}
    
public:
    vector<Point> m_points;
    
private:    
    unsigned int m_pointSize;
    unsigned int m_minPoints;
    float m_epsilon;
};

int DBSCAN::run()
{
    int clusterID = 1;
    vector<Point>::iterator iter;
    for(iter = m_points.begin(); iter != m_points.end(); ++iter)
    {
        if ( iter->clusterID == UNCLASSIFIED )
        {
            if ( expandCluster(*iter, clusterID) != FAILURE )
            {
                clusterID += 1;
            }
        }
    }

    return (clusterID-1);
}

int DBSCAN::expandCluster(Point point, int clusterID)
{    
    vector<int> clusterSeeds = calculateCluster(point);

    if ( clusterSeeds.size() < m_minPoints )
    {
        point.clusterID = NOISE;
        return FAILURE;
    }
    else
    {
        int index = 0, indexCorePoint = 0;
        vector<int>::iterator iterSeeds;
        for( iterSeeds = clusterSeeds.begin(); iterSeeds != clusterSeeds.end(); ++iterSeeds)
        {
            m_points.at(*iterSeeds).clusterID = clusterID;
            if (m_points.at(*iterSeeds).x == point.x && m_points.at(*iterSeeds).y == point.y && m_points.at(*iterSeeds).z == point.z )
            {
                indexCorePoint = index;
            }
            ++index;
        }
        clusterSeeds.erase(clusterSeeds.begin()+indexCorePoint);

        for( vector<int>::size_type i = 0, n = clusterSeeds.size(); i < n; ++i )
        {
            vector<int> clusterNeighors = calculateCluster(m_points.at(clusterSeeds[i]));

            if ( clusterNeighors.size() >= m_minPoints )
            {
                vector<int>::iterator iterNeighors;
                for ( iterNeighors = clusterNeighors.begin(); iterNeighors != clusterNeighors.end(); ++iterNeighors )
                {
                    if ( m_points.at(*iterNeighors).clusterID == UNCLASSIFIED || m_points.at(*iterNeighors).clusterID == NOISE )
                    {
                        if ( m_points.at(*iterNeighors).clusterID == UNCLASSIFIED )
                        {
                            clusterSeeds.push_back(*iterNeighors);
                            n = clusterSeeds.size();
                        }
                        m_points.at(*iterNeighors).clusterID = clusterID;
                    }
                }
            }
        }

        return SUCCESS;
    }
}

vector<int> DBSCAN::calculateCluster(Point point)
{
    int index = 0;
    vector<Point>::iterator iter;
    vector<int> clusterIndex;
    for( iter = m_points.begin(); iter != m_points.end(); ++iter)
    {
		//cerr<<calculateDistance(point, *iter)<<endl;
        if ( calculateDistance(point, *iter) <= m_epsilon )
        {
            clusterIndex.push_back(index);
        }
        index++;
    }
    return clusterIndex;
}

inline double DBSCAN::calculateDistance(const Point& pointCore, const Point& pointTarget )
{
    return pow(pointCore.x - pointTarget.x,2)+pow(pointCore.y - pointTarget.y,2)+pow(pointCore.z - pointTarget.z,2);
}


double FindBestEpsilon(int & minPts, double & binDeta ,vector<Point> & Point )
{
	int PointNum=Point.size();
	double dis=0;

	map <double,int> DisMap;
	map <double,int> :: iterator  DisMapIT;
	double MaxSSE=-1;
	double MinSSE=10000;
	for (int ii=0 ; ii<PointNum ; ii++)
	{
		map <double,int> MinKK;
		map <double,int> :: iterator  KKit ;
		for (int jj=0 ; jj<PointNum ;  jj++)
		{
			if (ii==jj)
			{
				continue ;
			}
			dis=distance(Point[ii], Point[jj]);
			KKit=MinKK.find(dis);
			if (KKit==MinKK.end())
			{
				MinKK.insert(map <double ,int> ::value_type(dis,1));
			}
			else
			{
				(KKit->second)++;
			}
		}
		int Count=0;
		double meanDisKK =0;
		for (KKit=MinKK.begin(); KKit!=MinKK.end() && Count< minPts ; KKit++)
		{
			meanDisKK+=((KKit->first)*(KKit->second));
			Count+=(KKit->second);
		}

		meanDisKK=meanDisKK/Count;

		if (MaxSSE<meanDisKK) {MaxSSE=meanDisKK;}
		if (MinSSE>meanDisKK) {MinSSE=meanDisKK;}

		DisMapIT=DisMap.find(meanDisKK);
		if (DisMapIT==DisMap.end())
		{
			DisMap.insert(map <double ,int>::value_type(meanDisKK,1));
		}
		else
		{
			(DisMapIT->second)++;
		}
	}


	binDeta=(MaxSSE-MinSSE)/10;
	
	//cerr<<MaxSSE<<"\t"<<MinSSE<<"\t"<<binDeta<<endl;

	map <int,int> MapCount;
	map <int,int> :: iterator MapCountIT;
	for (DisMapIT=DisMap.begin(); DisMapIT!=DisMap.end() ; DisMapIT++)
	{
		int AA=int(((DisMapIT->first)-MinSSE)/binDeta);
		MapCountIT=MapCount.find(AA);
		if (MapCountIT==MapCount.end())
		{
			MapCount.insert(map <int ,int>::value_type(AA,DisMapIT->second));
		}
		else
		{
			(MapCountIT->second)+=(DisMapIT->second);
		}
	}

	MapCountIT=MapCount.begin();
	double DetaCount=PointNum*1.0/10;
	double ratio ;
	
	for (MapCountIT=MapCount.end() ; MapCountIT!=MapCount.begin(); )
	{
		MapCountIT--;
		ratio=((MapCountIT->second)*1.0)/(DetaCount);
		if  (ratio>0.1)
		{			
			break;
		}
	}

	if (MapCountIT!=MapCount.begin())
	{
		MaxSSE=MinSSE+binDeta*(MapCountIT->first+1);
	}
	else
	{
		MaxSSE=MinSSE+binDeta;
	}




    binDeta=(MaxSSE-MinSSE)/10;
//	cerr<<MaxSSE<<"\t"<<MinSSE<<"\t"<<binDeta<<endl;

	MapCount.clear();
	int Outliter=0;
	for (DisMapIT=DisMap.begin(); DisMapIT!=DisMap.end() ; DisMapIT++)
	{
		if  ((DisMapIT->first)>=MaxSSE) {Outliter++;continue;}

		int AA=int(((DisMapIT->first)-MinSSE)/binDeta);
		MapCountIT=MapCount.find(AA);
		if (MapCountIT==MapCount.end())
		{
			MapCount.insert(map <int ,int>::value_type(AA,DisMapIT->second));
		}
		else
		{
			(MapCountIT->second)+=(DisMapIT->second);
		}
	}

	MapCountIT=MapCount.begin();
	DetaCount=(PointNum-Outliter)*1.0/10;

	/*
	cerr<<"bbb\t"<<PointNum<<"\t"<<DetaCount<<"\teeebbsum\t"<<Outliter<<endl;
		for (MapCountIT=MapCount.begin() ; MapCountIT!=MapCount.end(); MapCountIT++)
	{
		ratio=((MapCountIT->second)*1.0)/(DetaCount);
		cerr<<(MapCountIT->first)<<"\t"<<(MapCountIT->second)<<"\t"<<ratio<<endl;
	}
*/

	
	for (MapCountIT=MapCount.end() ; MapCountIT!=MapCount.begin(); )
	{
		MapCountIT--;
		ratio=((MapCountIT->second)*1.0)/(DetaCount);
		if  (ratio>0.1)
		{
			break;
		}
	}


	double EPS=MinSSE+binDeta ;
	if (MapCountIT!=MapCount.begin())  
	{
		EPS=MinSSE+binDeta*(MapCountIT->first+1);
//		cerr<<MinSSE<<"\t"<<binDeta<<"\t"<<(MapCountIT->first)<<"\t"<<EPS<<endl;
	}

	return EPS ;
}

#endif // DBSCAN_H


