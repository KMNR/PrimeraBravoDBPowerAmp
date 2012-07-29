#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <cstdio>

using namespace std;

const string STATUS_FILE = "C:\\ptburnjobs\\Status\\PTStatus.txt";
const string REQUEST_FOLDER = "C:\\ptburnjobs\\";

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
        cout<<"Couldn't open status file"<<std::endl;
        return -9999;
    }
    while(!f.eof() && f.good())
    {
        getline(f,tmp);
        string needle = "SysErrorString=";
        size_t pos = tmp.find(needle);
        if(pos != string::npos)
        {
            cout<<"Match (ERROR): "<<tmp<<std::endl;
            stringstream ss(tmp.substr(pos+needle.length()));
            getline(ss,error,'\r');
            cout<<"Found Error: "<<error<<std::endl;
        }
        needle = "LoadDiscState0=";
        pos = tmp.find(needle);
        if(pos != string::npos)
        {
            cout<<"Match (STATE): "<<tmp<<std::endl;
            stringstream ss(tmp.substr(pos+needle.length()));
            ss>>status;
            cout<<"Found load status: "<<status<<std::endl;
        }
    }
    f.close();
	if(error != "No Errors")
		return 4;
    return status;
}

int main()
{
    cout<<"Issuing Eject"<<std::endl;
    update_job("REJECT_DISC");
    cout<<"Waiting..."<<std::endl;
    int status = get_job_status();
    int c = 0;
    while(status == 1 || status == 4)
    {
        cout<<"Current Status: "<<status<<std::endl;
        _sleep(2000);
        c++;
        if(c > 30 && status == 1)
        {
            cout<<"Reissuing Eject"<<std::endl;
            update_job("REJECT_DISC");
            c = 0;
        }
        status = get_job_status();
    }
    cout<<"Disc Ejected. (Status ="<<status<<")"<<std::endl;
    _sleep(30000);
    cout<<"Watching for Job Complete"<<std::endl;
    while(1)
    {
        string file = REQUEST_FOLDER+"extract.ERR";
        ifstream f(file.c_str());
        if(f.good())
        {
            cout<<"Found err file (that's good!)"<<endl;
            f.close();
            remove(file.c_str());
            break;
        }
        else
        {
            f.close();
        }
        _sleep(5000);
    }
    cout<<"Finished."<<endl;
    return 0;
}
