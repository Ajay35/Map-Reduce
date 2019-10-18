#include<bits/stdc++.h>
#include <sys/stat.h>
#include <sys/mount.h> 
#include "k_way.h"

#define _GNU_SOURCE
#include <unistd.h>
#include <string.h>
#include <stdio.h>
 
#ifndef TARGET_UID
#define TARGET_UID 0
#endif
 
#ifndef TARGET_GID
#define TARGET_GID 0
#endif
 
#ifndef UID_MIN
#define UID_MIN 500
#endif
 
#ifndef GID_MIN
#define GID_MIN 500
#endif
int getWordsCount(std::string file_name)
{
    std::string str;
    int count=0;
    std::ifstream in;
    in.open(file_name);
    while(in>>str)
        count++;
    in.close();
    return count;
}
int calhash(std::string str)
{
    long long  hash = 1;
    for (int i = 0; i < str.length(); i++) 
    {
        hash += pow(37,i)*str[i];
        hash%=3;
    }
   
return hash;
}
void sort_file(std::string file_name)
{
    std::fstream fileh;
    std::string word;
    std::vector<std::string> names;
    fileh.open(file_name,std::fstream::in);
    while(getline(fileh, word))
        names.push_back(word);
    fileh.close();
    sort(names.begin(), names.end());
    fileh.open(file_name,std::fstream::out);
    for(int i=0;i<names.size();i++)
    {           
        fileh<<names[i]<<"\n";
    }
    fileh.close();
}
void merge_files(std::ifstream *in,std::string f_name)
{
    min_heap heap;
	// make priority queue of first elements of k files
	for(int i=0;i<5;i++)
	{
		string str;
		in[i]>>str;
		heap.insert_element(node(i,str));
	}

    std::ofstream out;
    std::string tmp;
    int count=0;
    out.open(f_name,std::fstream::out |std::fstream::app);
    while(count!=5)
    {
   		node n=heap.heap_top();
        out<<n.val<<"\n";
       // std::cout<<"file selected:"<<n.file_no<<std::endl;
        if(in[n.file_no]>>tmp)
        {
            n.val=tmp;
        }
		else
		{
			n.val="zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
			count++;
		}
		heap.pop();
		heap.insert_element(n);
    }
    out.close();
}
int mount_fs(char * targetPath)
{
    uid_t ruid, euid, suid; /* Real, Effective, Saved user ID */
    gid_t rgid, egid, sgid; /* Real, Effective, Saved group ID */
    int uerr, gerr, fd;
 
    if (getresuid(&ruid, &euid, &suid) == -1) {
        fprintf(stderr, "Cannot obtain user identity: %m.\n");
        return EXIT_FAILURE;
    }
    if (getresgid(&rgid, &egid, &sgid) == -1) {
        fprintf(stderr, "Cannot obtain group identity: %m.\n");
        return EXIT_FAILURE;
    }
    if (ruid != (uid_t)TARGET_UID && ruid < (uid_t)UID_MIN) {
        fprintf(stderr, "Invalid user.\n");
        return EXIT_FAILURE;
    }
    if (rgid != (gid_t)TARGET_UID && rgid < (gid_t)GID_MIN) {
        fprintf(stderr, "Invalid group.\n");
        return EXIT_FAILURE;
    }
 
    /* Switch to target user. setuid bit handles this, but doing it again does no harm. */
    if (seteuid((uid_t)TARGET_UID) == -1) {
        fprintf(stderr, "Insufficient user privileges.\n");
        return EXIT_FAILURE;
    }
 
    /* Switch to target group. setgid bit handles this, but doing it again does no harm.
     * If TARGET_UID == 0, we need no setgid bit, as root has the privilege. */
    if (setegid((gid_t)TARGET_GID) == -1) {
        fprintf(stderr, "Insufficient group privileges.\n");
        return EXIT_FAILURE;
    }
    const char* srcPath = "10.1.37.216:/home/ajay/share";
    if(!mount(srcPath, targetPath,"nfs",0, "nolock,addr=10.1.37.216"))
    {
        cout<<"mounted"<<endl;
    }
    else
    {
        cout<<"failed"<<endl;
        cout<<strerror(errno)<<":"<<errno<<endl;
    }

    gerr = 0;
    if (setresgid(rgid, rgid, rgid) == -1) {
        gerr = errno;
        if (!gerr)
            gerr = EINVAL;
    }
    uerr = 0;
    if (setresuid(ruid, ruid, ruid) == -1) {
        uerr = errno;
        if (!uerr)
            uerr = EINVAL;
    }
    if (uerr || gerr) {
        if (uerr)
            fprintf(stderr, "Cannot drop user privileges: %s.\n", strerror(uerr));
        if (gerr)
            fprintf(stderr, "Cannot drop group privileges: %s.\n", strerror(gerr));
        return EXIT_FAILURE;
    }

}

