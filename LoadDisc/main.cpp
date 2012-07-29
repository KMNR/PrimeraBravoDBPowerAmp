#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>

using namespace std;

const string STATUS_FILE = "C:\\ptburnjobs\\Status\\PTStatus.txt";
const string REQUEST_FOLDER = "C:\\ptburnjobs\\";
const string LOG_FILE = "C:\\ptburnjobs\\Status\\DBPoweramp.log";

fstream LOGSTREAM;

void logsave()
{
    LOGSTREAM.close();
    LOGSTREAM.open(LOG_FILE.c_str(), fstream::out | fstream::app);
}

void create_job()
{
    string file = REQUEST_FOLDER+string("extract.jrq");
	fstream f(file.c_str(), fstream::out | fstream::trunc);
	f<<"ClientID=KMNR Ripper"<<endl;
	f<<"JobID=RIP DISC"<<endl;
	f<<"Importance=2"<<endl;
	f<<"Copies=1"<<endl;
	f<<"NotifyClient=Message"<<endl;
	f<<"CreateSubFolders=YES"<<endl;
	f<<"LoadUnloadOverride=YES"<<endl;
	f.close();
}

void update_job(string msg)
{
    string file = REQUEST_FOLDER+"extract.ptm";
	fstream f(file.c_str(), fstream::out | fstream::trunc);
	f<<"Message="<<msg<<endl;
	f<<"DiscID=0"<<endl;
	f.close();
}

int get_job_status()
{
	int status = -1;
	string error = "No Errors";
    string tmp;
    fstream f(STATUS_FILE.c_str(), fstream::in);
    if(!f.good())
    {
        f.close();
        LOGSTREAM<<"Couldn't open status file"<<std::endl;
        logsave();
        return -9999;
    }
    while(!f.eof() && f.good())
    {
        getline(f,tmp);
        string needle = "SysErrorString=";
        size_t pos = tmp.find(needle);
        if(pos != string::npos)
        {
            LOGSTREAM<<"Match (ERROR): "<<tmp<<std::endl;
            logsave();
            stringstream ss(tmp.substr(pos+needle.length()));
            getline(ss,error,'\r');
            LOGSTREAM<<"Found Error: "<<error<<std::endl;
            logsave();
        }
        needle = "LoadDiscState0=";
        pos = tmp.find(needle);
        if(pos != string::npos)
        {
            LOGSTREAM<<"Match (STATE): "<<tmp<<std::endl;
            logsave();
            stringstream ss(tmp.substr(pos+needle.length()));
            ss>>status;
            LOGSTREAM<<"Found load status: "<<status<<std::endl;
            logsave();
        }
    }
    f.close();
	if(error != "No Errors")
		return 4;
    return status;
}

int main()
{
    LOGSTREAM.open(LOG_FILE.c_str(), fstream::out | fstream::trunc);
    LOGSTREAM<<"Requesting Job"<<std::endl;
    logsave();
    create_job();
    LOGSTREAM<<"Watching for job state 1"<<std::endl;
    logsave();
    while(get_job_status() != 1)
    {
        _sleep(2000);
    }
    LOGSTREAM<<"Job Registered. Exiting."<<std::endl;
    logsave();
    return 0;
}
