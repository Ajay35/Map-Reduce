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
#include <dirent.h>

#include "master.hpp"
#include "reducer.hpp"
#include "utilitiy.hpp"

#define MAPPER_COUNT 5
#define REDUCER_COUNT 3

void killer_thread(std::string ip,int port,std::string thread_name)
{
    pthread_mutex_lock(&reducer_lock); 
    std::cout<<"----------------------------------------------\n";
  
    std::cout<<"mutex held by:"<<thread_name<<std::endl;
    struct sockaddr_in master_addr,self_addr;  
    char buffer[1024];
    pthread_setname_np(pthread_self(), thread_name.c_str());
    int sockfd=socket(AF_INET, SOCK_STREAM, 0);
    self_addr.sin_family = AF_INET; 
    self_addr.sin_addr.s_addr = inet_addr(ip.c_str()); 
    self_addr.sin_port =htons(port); 
    bind(sockfd, (struct sockaddr *)&self_addr,sizeof(self_addr));
    
    if(sockfd<0) 
    {
        std::cout<<"failed to open socket"<<"\n";
        close(sockfd);
        std::exit(0);
    }
    bzero((char *) &master_addr, sizeof(master_addr));
    master_addr.sin_family = AF_INET;
    master_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    master_addr.sin_port = htons(8080);

    if (connect(sockfd,(struct sockaddr *) &master_addr,sizeof(master_addr)) < 0) 
    {
        std::cout<<"Failed to connect to master"<<"\n";
        close(sockfd);
        std::exit(0);
    }
    /* reading input string from client */
    int n;
    
    bzero(buffer,1024);
    strcpy(buffer,thread_name.c_str());  
    n=write(sockfd,buffer,strlen(buffer));
    
    /*if(n>0)
    {
      
        std::cout<<thread_name<<":exit message send to master "<<"\n";
       
    }
    std::cout<<"mutex released by:"<<thread_name<<std::endl;
    pthread_mutex_unlock(&reducer_lock);
}

void cleaner(int sockfd,std::string thread_name,int thread_no)
{
     char buffer[1024];
     int n;
     
     for(int i=0;i<5;i++)
    {
        remove(("reducer/mapper_"+std::to_string(i+1)+"/"+std::to_string(thread_no-1)).c_str());
    }
    remove(("reduced/reduced_"+std::to_string(thread_no)).c_str());
    
    if(reducer_counter+1==REDUCER_COUNT)
    {
         for(int i=0;i<5;i++)
        {
            rmdir(("reducer/mapper_"+std::to_string(i+1)).c_str());
        }
        rmdir(("reducer"));
        rmdir(("reduced"));
        bzero(buffer,1024);
        strcpy(buffer,"done");
        n= write(sockfd,buffer,strlen(buffer));
        if(n>0)
        {
            bzero(buffer,1024);
            std::cout<<thread_name<<":waiting for master reply"<<"\n";
            n=read(sockfd,buffer,1024);
            if(n<0)
                std::cout<<"No message from server"<<"\n";
            else
            {  
                std::cout<<"\n============================================================================\n";
                 std::cout<<"Task ====>"<<buffer<<std::endl;
                 std::cout<<"\n============================================================================\n";
                std::cout<<"Sending message of completion to master\n";
               /* bzero(buffer,1024);
                strcpy(buffer,("over"));  
                n=write(sockfd,buffer,strlen(buffer));
                if(n>0)
                {
                    std::cout<<"success";
                }
                else
                {
                    std::cout<<"failure";
                }*/
                close(sockfd);
                int port=8090;
                std::string ip="127.0.0.10";
                std::thread killer(killer_thread,ip,port,"killer");
                killer.detach();
                 std::cout<<"Task Complete message send====>"<<std::endl;
            }
        }  
    }
    else
    {
        bzero(buffer,1024);
        strcpy(buffer,"done");
        write(sockfd,buffer,strlen(buffer));
        close(sockfd);
    }
    
}

void reducer_thread(std::string ip,int port,std::string thread_name,int thread_no)
{
    pthread_mutex_lock(&reducer_lock); 
    std::cout<<"----------------------------------------------\n";
    mkdir("reduced", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    std::cout<<"mutex held by:"<<thread_name<<std::endl;
    struct sockaddr_in master_addr,self_addr;  
    char buffer[1024];
    pthread_setname_np(pthread_self(), thread_name.c_str());
    int sockfd=socket(AF_INET, SOCK_STREAM, 0);
    self_addr.sin_family = AF_INET; 
    self_addr.sin_addr.s_addr = inet_addr(ip.c_str()); 
    self_addr.sin_port =htons(port); 
    bind(sockfd, (struct sockaddr *)&self_addr,sizeof(self_addr));
    
    if(sockfd<0) 
    {
        std::cout<<"failed to open socket"<<"\n";
        close(sockfd);
        std::exit(0);
    }
    bzero((char *) &master_addr, sizeof(master_addr));
    master_addr.sin_family = AF_INET;
    master_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    master_addr.sin_port = htons(8080);

    if (connect(sockfd,(struct sockaddr *) &master_addr,sizeof(master_addr)) < 0) 
    {
        std::cout<<"Failed to connect to master"<<"\n";
        close(sockfd);
        std::exit(0);
    }
    int n;
    
    bzero(buffer,1024);
    strcpy(buffer,thread_name.c_str());  
    n=write(sockfd,buffer,strlen(buffer));
    
    if(n>0)
    {
        bzero(buffer,1024);
        std::cout<<thread_name<<":waiting for master reply"<<"\n";
        n=read(sockfd,buffer,1024);
        if(n<0)
            std::cout<<"No message from server"<<"\n";
        else
            std::cout<<"message from master to :"<<thread_name<<":"<<buffer<<std::endl;
    }
    std::cout<<"mutex released by:"<<thread_name<<std::endl;
    pthread_mutex_unlock(&reducer_lock);

    /* actual work here */
    std::string f_name="reduced/reduced_"+std::to_string(thread_no);

    for(int i=0;i<5;i++)
    {
        sort_file("reducer/mapper_"+std::to_string(i+1)+"/"+std::to_string(thread_no-1));
    }

    std::ifstream in[5];

    for(int i=0;i<5;i++)
    {
        in[i].open("reducer/mapper_"+std::to_string(i+1)+"/"+std::to_string(thread_no-1));
    }
    merge_files(in,f_name);
    
    std::cout<<"reducer finished:"<<thread_name<<std::endl; 
    pthread_mutex_lock(&file_lock);
    reduce(f_name);
    pthread_mutex_unlock(&file_lock);
    
    cleaner(sockfd,thread_name,thread_no);
   
}
void master_to_reducer_thread(int new_socket_fd,struct sockaddr_in client_addr)
{
    char buffer[1024];
    bzero(buffer,1024);
    strcpy(buffer,"hello reducer");
    write(new_socket_fd,buffer,strlen(buffer));
    bzero(buffer,1024);
    read(new_socket_fd,buffer,1024);
    std::string msg(buffer);
    ////////////////////////
    if(msg=="done")
    {
            pthread_mutex_lock(&reducer_lock);
            reducer_counter++; 
            std::cout<<"Reducer counter:"<<reducer_counter<<std::endl;
            pthread_mutex_unlock(&reducer_lock);
    }
    if(reducer_counter==REDUCER_COUNT)
    {
    bzero(buffer,1024);
    strcpy(buffer,"complete");
   // std::cout<<"==========complete\n";
    int n=write(new_socket_fd,buffer,strlen(buffer));

    
    }
   
    ////////////////////////
}
void reduce(std::string file_name)
{
    char filename[ ] = "FinalOutput.txt";
    std::fstream out;
    std::ifstream in;
    std::string str;
    std::unordered_map<std::string,int> words;
    in.open(file_name);
    out.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
    while(in >> str)
    {
        ++words[str];
    }

    for(auto& i:words)
    {
        out<<i.first<<","<<i.second<<"\n";
    }
}