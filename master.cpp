#include<bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/sha.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include<dirent.h>
#include <atomic>
#include "master.hpp"
#include "mapper.hpp"
#include "reducer.hpp"
#include "utilitiy.hpp"
#define MAPPER_COUNT 5
#define REDUCER_COUNT 3


void init_system()
{    
    int port=8080;
    std::string ip="";
     std::cout<<"----------------------------------------------\n";
    std::cout<<"init system"<<std::endl; 
    for(int i=2;i<=6;i++)
    {
        ip="127.0.0."+std::to_string(i);
        std::thread mapper(mapper_thread,ip,i+port,"mapper_"+(std::to_string(i-1)),i-1);
        mapper.detach();
    }
     std::cout<<"----------------------------------------------\n";
}
void init_master_connection(int *sockfd)
{
    
    int new_socket_fd;
    socklen_t clilen;
    char buffer[1024]; 
    struct sockaddr_in self_addr; 
    if((*sockfd=socket(AF_INET, SOCK_STREAM,0))<0)
    { 
        std::cout<<"socket creation failed"<<"\n"; 
        close(*sockfd);
        std::exit(0);
    } 
    memset(&self_addr, 0, sizeof(self_addr)); 
    self_addr.sin_family=AF_INET;
    self_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    self_addr.sin_port = htons(8080); 
    if(bind(*sockfd, (struct sockaddr *)&self_addr,sizeof(self_addr))<0)
    { 
        std::cout<<"TCP socket bind failed"<<"\n";
        close(*sockfd);
        std::exit(0);
    }
}
void cleanmaster()
{
    DIR *dir;
    std::string dir_name="mapper";
    struct dirent *dirp;
    std::string path;
    dir=opendir(dir_name.c_str());
    if(dir==NULL)
    {
        perror("Error opening directory");
        exit(0);
    }

     while ((dirp= readdir(dir))!=NULL)
    {
        std::string file(dirp->d_name);
        path=dir_name+"/"+file;
        remove(path.c_str());
    }
    rmdir(dir_name.c_str());
    closedir(dir);   
}
void init_file_partition(int chunk_size,std::string input_filename)
{
        std::ifstream in;
        std::ofstream out;
        in.open(input_filename);
        std::string filename="mapper";
        mkdir("mapper", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        for(int i=1;i<=MAPPER_COUNT;i++)
        {
            std::string str;
            int count=0;
            out.open(filename+"/mapper_"+std::to_string(i));
            while(count!=chunk_size)
            {
                in>>str;
                out<<str<<"\n";
                count++;
            }
            out.close(); 
        }
          in.close();
        std::cout<<"-------------------------\n";
        std::cout<<"File partitoning done\n";
        std::cout<<"-------------------------\n";
         std::cout<<"----------------------------------------------\n";
}
void master_thread(std::string file_name)
{
        int sockfd=0;
        socklen_t clilen;
        struct sockaddr_in client_addr; 
        char buffer[1024];
        int word_count=getWordsCount(file_name);
         std::cout<<"----------------------------------------------\n";
        std::cout<<"words in file:"<<word_count<<std::endl;
        std::ifstream fin("FinalOutput.txt");
        if(fin) {
          remove("FinalOutput.txt");
          fin.close();
        }

        
        init_master_connection(&sockfd);
        
        ////////// file partitioning//////////////

        
        int chunk_size=word_count/MAPPER_COUNT;
      
         init_file_partition(chunk_size,file_name);
         std::cout<<"\n===========================================================================\n";
         std::cout<<"\n*********************** Mapper *******************************\n";
         std::cout<<"\n===========================================================================\n";
        /////////////////////////////////////////////////
       while(1)
        {
            std::cout<<"\n|||||||||||||||||||||||||||||||||||||||||||||||\n";
            std::cout<<"master listening.............. \n"<<std::endl;
            std::cout<<"|||||||||||||||||||||||||||||||||||||||||||||||\n";
            listen(sockfd,10);
            int new_socket_fd= accept(sockfd,(struct sockaddr *) &client_addr,&clilen);
            bzero(buffer,1024);
            read(new_socket_fd,buffer,1024);
            std::string msg(buffer);
            if(msg[0]=='m')
            {
                std::thread master_to_mapper(master_to_mapper_thread,new_socket_fd,client_addr);
                master_to_mapper.detach();
            }
            else if(msg[0]=='r')
            {
                std::cout<<"message from:"<<msg<<std::endl;
                std::thread master_to_reducer(master_to_reducer_thread,new_socket_fd,client_addr);
                master_to_reducer.detach();
            }
            else if(msg[0]=='k')
            {
                std::cout<<"master:"<<msg<<std::endl;
                cleanmaster();
                std::cout<<"*********************** Task ****************************"<<msg<<"\n";
                break;
            }
              
        }       
       std::cout<<"master exited,task completed.."<<std::endl; 
       std::exit(0);
}
