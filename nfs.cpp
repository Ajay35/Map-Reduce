#include<bits/stdc++.h>
#include <sys/stat.h>
#include <sys/mount.h> 
#include <errno.h>
#include <stdlib.h>
using namespace std;
int main(int argc, char const *argv[])
{
    ofstream in;
    int retval;
    string  str="nfs_fs";
    const char* targetPath = "nfs_fs";
    const char* srcPath = "10.1.37.216:/home/ajay/share";
    mkdir(str.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(!mount(srcPath, targetPath,"nfs",0, "nolock,addr=10.1.37.216"))
    {
        cout<<"mounted"<<endl;
    }
    else
    {
        cout<<"failed"<<endl;
        cout<<strerror(errno)<<":"<<errno<<endl;
    }

    in.close();
    return 0;
}
