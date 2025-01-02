#ifndef EM_H_
#define EM_H_


#include "DataClass.h"


class Cluster
{
	public:

		//input
		MatrixXf P; //the data  N x d (this way for easier interface with vector< vector >
		int d; //dimension of the data
		int N; //number of data points
		int K; //number of clusters
		float eps;	//target accuracy for EM
		float dc; //cutoff for density cluster

		//output
		MatrixXf centres;
		VectorXi assignment;
		VectorXi sizes;

		//em stuff
		VectorXf alpha; 
		vector< MatrixXf > covariance; 
		MatrixXf prob; 
		MatrixXf weight;

		//empty constructor
		Cluster(){
			d = 0;
			N = 0;
			K = 0;
			eps = 1e-8;
		}
		//no data constructor
		Cluster(int K_, float eps_=1e-8)
		{
			K = K_;
			eps = eps_;
		}
		//init
		void initP(const Ref<MatrixXf> P_)
		{
			P = P_;
			d = P.cols();
			N = P.rows();
		}
		//init
		void initK(int K_)
		{
			K = K_;

			centres.resize(K, d);
			assignment = VectorXi::Zero(N);
			sizes = VectorXi::Zero(K); 

			alpha.resize(K);
			for(int k=0; k<K; ++k){ covariance.push_back( MatrixXf::Zero(d,d) ); }
			weight = MatrixXf::Zero(K,N);
			prob.resize(K,N);


		}
		//full constructor
		Cluster(const Ref<MatrixXf> P_, int K_, float eps_=1e-8)
		{
			eps = eps_;
			initP(P_);
			initK(K_);
		}

	
		void assignCentres(const Ref<MatrixXf> centres_){ centres = centres_; }
		void clusterAssign();
		void assignCentres (vector<Point> &cluster, vector<Point> & data)
		{
			for(int i=0 ; i< K ; i++)
			{
				centres(i,0)=cluster[i].x;
				centres(i,1)=cluster[i].y;
				centres(i,2)=cluster[i].z;
			}
		}

		//em init functions
		float EMweights();
		void EMassign();

		void EMcluster(int max_its);

};



// calculate weights for gaussian mixture clusters
float Cluster::EMweights() 
{

	//calc weights
	for (int k = 0; k < K; ++k) 
	{
		MatrixXf MS = P.transpose();
		for (int n = 0; n < N; ++n) { MS.col(n) -= centres.row(k); }    //MS.col(i) = x_i - mu_k
		MatrixXf SMS = covariance[k].householderQr().solve(MS);    //SMS.col(i) = Sigma^-1 ( x_i - mu_k )
		float norm = 1.0 / (pow(2 * 3.1415926535897, d * 0.5) * sqrt(abs(covariance[k].determinant()))); //technically correct
		for (int n = 0; n < N; ++n) 
		{
		prob(k, n) = norm * exp(-0.5 * MS.col(n).dot(SMS.col(n)));
		}
	
	}

	float newP = 0;

	for (int n = 0; n < N; ++n)
	{
		for (int k = 0; k < K; ++k) 
		{ 
			weight(k, n) = alpha(k) * prob(k, n);
			if (isnan(weight(k, n)))
			{
				weight(k, n)=0;
			}
			else if (isinf(weight(k, n)))
			{
				weight(k, n)=0;
			}
		}
	
		float norm = weight.col(n).sum();
		if  (norm<1) {norm=1;}
		newP += log(norm);
		weight.col(n) /= norm;

	}

	return newP;
}


//assign data to clusters based on gaussian probability weights
void Cluster::EMassign()
{

	for (int k = 0; k < K; ++k)
	{
		sizes(k) = 0; 
	}
	for (int n = 0; n < N; ++n)
	{
		int mx;
		weight.col(n).maxCoeff(&mx);
		assignment[n] = mx;
		++sizes[mx];
	}
}


void Cluster::clusterAssign()
{
	sizes = VectorXi::Zero(K);

	//assign to cluster
	for (int i = 0; i < N; ++i)
	{
		float min_dist = numeric_limits<float>::infinity();
		int c = -1;
		for (int j = 0; j < K; ++j)
		{
			float dist = (P.row(i) - centres.row(j)).squaredNorm();
			if (dist < min_dist)
			{
				min_dist = dist;
				c = j;
			}
		}
		assignment(i) = c;
		++sizes(c);
	}
}


//EM Gaussian clustering algorithm
void Cluster::EMcluster( int max_its ) 
{

	clusterAssign();

	//init
	weight = MatrixXf::Zero(K, N);
	for (int n = 0; n < N; ++n) { weight(assignment(n), n) = 1.0; }

	float oldP = 0;
	for (int n = 0; n < N; ++n)
	{
		float norm = weight.col(n).sum();
		oldP += log(norm);
	}

	for (int it = 0; it < max_its; ++it) 
	{
		//calc params
		centres = MatrixXf::Zero(K, d);
		for (int k = 0; k < K; ++k) 
		{
			alpha(k) = 0;
			for (int n = 0; n < N; ++n) 
			{
				alpha(k) += weight(k, n);
				centres.row(k) += weight(k, n) * P.row(n).transpose();
			}
			centres.row(k) *= (1.0 / alpha(k));
			covariance[k] = MatrixXf::Zero(d, d);
			for (int n = 0; n < N; ++n) 
			{
				covariance[k].noalias() +=
				weight(k, n) * (P.row(n) - centres.row(k)).transpose() * (P.row(n) - centres.row(k));
			}
			covariance[k] *= (1.0 / (float) alpha(k));
			alpha(k) *= (1.0 / N);
		}
		float newP = EMweights();
		float detaEps=abs(newP - oldP) / abs(oldP);
		if (detaEps < eps)
		{
			cout << "EM Clustering completed after " << it << " iterations, Eps= "<<detaEps << endl;
			break;
		}
		
		if (it + 1 == max_its) { cout << "EM Clustering Max its reached: Eps= " << detaEps  << endl; }
		oldP = newP;
	}

	EMassign();
}


#endif


