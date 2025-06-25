#ifndef comm_H_
#define comm_H_

using namespace std;
typedef int  llong ;

//////////////////////////////// some small funtion /////////////////////////////////////////////

bool GetBinPath ( std::string path  ,std::string& BinPath )
{
	char   buf[2048]={'\0'};
	BinPath.clear() ;
	if ( path.rfind('/') == string::npos)
	{
		string cc="which  "+path+"   2> /dev/null ";
		FILE   *stream ;
		stream=popen(cc.c_str(),"r") ;
		fread( buf, sizeof(char), sizeof(buf), stream);
		BinPath=buf;
		BinPath=BinPath.substr(0,BinPath.rfind('/'));
	}
	else
	{
		if (path[0]  == '/' )
		{
			BinPath=path.substr(0, path.rfind('/')+1);
		}
		else if   (path[0]  == '.' )
		{

			if(!getcwd(buf, 1000))
			{
				return false ;
			}
			BinPath = buf ;

			if (path[1]  == '/')
			{
				string cutOFF =path.substr(1, path.rfind('/'));
				BinPath = BinPath+cutOFF;
			}
			else
			{
				string cutOFF =path.substr(0, path.rfind('/'));
				BinPath = BinPath+"/"+cutOFF;
			}
		}
		else
		{
			string cutOFF =path.substr(0, path.rfind('/')+1);
			if(!getcwd(buf, 1000))
			{
				return false ;
			}
			BinPath = buf ;
			BinPath = BinPath+"/"+cutOFF;
		}
	}
	return true ;
}

string Int2Str (int A )
{
	stringstream   sstrm ;
	sstrm  <<  A ;
	return  sstrm.str();
}

inline void  LogLackArg( string  flag )
{
	cerr << "\t\tLack Argument for [ -"<<flag<<" ]"<<endl;
}


inline string &  replace_all(string &  str,const  string &  old_Avalue,const string &  new_Avalue)
{
	while(true)   {
		string::size_type  pos(0);
		if(   (pos=str.find(old_Avalue))!=string::npos   )
			str.replace(pos,old_Avalue.length(),new_Avalue);
		else   break;
	}
	return   str;
}


inline void split(const string& str,vector<string>& tokens,  const string& delimiters = " ")
{
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type pos     = str.find_first_of(delimiters, lastPos);
	while (string::npos != pos || string::npos != lastPos)
	{
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}



//  https://github.com/tobbez/string-splitting

/*
bool  GzipTrue( string filepath )
{
	FILE   *infile;
	unsigned char buf[3] ={'\0'};
	infile = fopen(filepath.c_str(), "rb");
	int rc = fread(buf,sizeof(unsigned char), 2,infile);
	fclose(infile);
	if  (buf[0] == 31 &&  buf[1] == 139)
	{
		return true ;
	}
	else
	{
		return false ;
	}
}
*/

bool is_digits(string &str)
{
    str=replace_all(str,"chr","");
    return str.find_first_not_of("0123456789") == std::string::npos ;
}

///////////////////swimming in the sky & flying in the sea/////////////////////////////

string getCmdResult(const string &strCmd)
{
	char buf[10240] = {0};
	FILE *pf = NULL;

	if( (pf = popen(strCmd.c_str(), "r")) == NULL )
	{
		return "";
	}

	string strResult;
	while(fgets(buf, sizeof buf, pf))
	{
		strResult += buf;
	}

	pclose(pf);

	unsigned int iSize =  strResult.size();
	if(iSize > 0 && strResult[iSize - 1] == '\n')  // linux
	{
		strResult = strResult.substr(0, iSize - 1);
	}

	return strResult;
}




void Rand6str( string &  strResult ) 
{
	char chr[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 
		'H', 'I', 'J', 'K', 'L', 'M', 'N',
		'O', 'P', 'Q', 'R', 'S', 'T', 
		'U', 'V', 'W', 'X', 'Y', 'Z',
		'a', 'b', 'c', 'd', 'e', 'f', 'g',
		'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 
		'u', 'v', 'w', 'x', 'y', 'z'};

	struct timeval tv; 
	gettimeofday(&tv, NULL);
	srand(tv.tv_sec + tv.tv_usec+((unsigned)time(NULL)));
	srand(time(NULL));
	for (int i=0; i<6; i++)
	{
		int idx = rand()%62;
		char buf=chr[idx];
		strResult=strResult+buf;
	}
}




#endif // comm_H_  ;


///////////////////



////////////////////////swimming in the sea & flying in the sky //////////////////


