#ifndef GetFig_H_
#define GetFig_H_


using namespace std;

int GetFig_Fun(In3str1v * paraFA04, Para_18 * para_18 )
{
	string binPath=(paraFA04->BinDir)+"/Plot2Deig";
	string command=binPath+"\t-InFile\t"+(paraFA04->InStr2)+".eigenvec\t-OutPut\t"+(paraFA04->InStr2);
	if  (!(paraFA04->PlotPara).empty())
	{
		command=command+"\t"+(paraFA04->PlotPara);
	}

	if ( access(binPath.c_str(), 0) == 0 )
	{
		std::system(command.c_str());
		command=command+" -columns  4:6 ";
		std::system(command.c_str());
		command=(paraFA04->BinDir)+"/Plot3Deig  -InFile "+(paraFA04->InStr2)+".eigenvec\t-OutPut "+(paraFA04->InStr2);
		std::system(command.c_str());
	}
	else
	{
		char   buf[2048]={'\0'};
		string cc="which  Plot2Deig  2> /dev/null ";
		memset( buf, '\0', sizeof(buf) );
		FILE   *stream ;
		stream=popen(cc.c_str(),"r");
		fread( buf, sizeof(char), sizeof(buf), stream);
		binPath=buf;
		binPath=binPath.substr(0,binPath.length()-1);

		if (binPath == "")
		{
			cout <<"\twarning: can't find the [Plot2Deig] in your $PATH ; no pdf Figure Out"<<endl;
			cout <<"\t\tPlot2Deig\t"<<command<<endl;
		}
		else
		{
			std::system(command.c_str());
		}
	}

	return 1;
}


#endif

