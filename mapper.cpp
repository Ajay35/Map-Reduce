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

#include "master.hpp"
#include "mapper.hpp"
#include "reducer.hpp"
#include "utilitiy.hpp"

#define MAPPER_COUNT 5
#define REDUCER_COUNT 3

void mapper_thread(std::string ip,int port,std::string thread_name,int thcount)
{
    pthread_mutex_lock(&mapper_lock); 
    std::cout<<"----------------------------------------------\n";
    /*this is where network file system is mounted*/
    mkdir("reducer", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    //mount_fs("reducer");
    std::string red_path="reducer/"+thread_name;
    mkdir(red_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
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


    if(connect(sockfd,(struct sockaddr *) &master_addr,sizeof(master_addr)) < 0) 
    {
        std::cout<<"Failed to connect to master"<<"\n";
        close(sockfd);
        std::exit(0);
    }
    /* reading input string from client */
    bzero(buffer,1024);
    std::strcpy(buffer,thread_name.c_str());  
   
    int n=write(sockfd,buffer,strlen(buffer));
   
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
    pthread_mutex_unlock(&mapper_lock);

    ///////////////////////////
   
    ////////////////////////////


    /*Actual work here*/
   
  
    std::ifstream in;
    in.open("mapper/"+thread_name);
    std::string str;
  
    ///////////////////
    while(in>>str)
    {
        std::vector<std::string> tokens;
		tokens.clear();
		std::stringstream check1(str);
		std::string rawcode;
		while(getline(check1, rawcode, ','))
		{
			tokens.push_back(rawcode);
		}
         int reducer_num=calhash(tokens[0]);
         std::string filename="reducer/"+thread_name+"/"+std::to_string(abs(reducer_num));
         std::ofstream out;
         out.open(filename, std::fstream::out |std::fstream::app);
         out<<str<<"\n";
         out.close();
    }
    in.close();
    ///////////////////////////////////

     
    bzero(buffer,1024);
    std::strcpy(buffer,"done");
    write(sockfd,buffer,strlen(buffer));
    std::cout<<"mutex released by:"<<thread_name<<std::endl;
    close(sockfd);
    std::cout<<thread_name<<" thread closed "<<std::endl;
     std::cout<<"----------------------------------------------\n";
}
void master_to_mapper_thread(int new_socket_fd,struct sockaddr_in client_addr)
{
    //std::cout<<"new connection accepted"<<std::endl;
    //std::cout<<"from IP:"<<inet_ntoa(client_addr.sin_addr)<<std::endl;
    //std::cout<<"from port:"<<ntohs(client_addr.sin_port)<<std::endl;
    char buffer[1024];
    std::cout<<"-------------------------\n";
    std::cout<<buffer<<std::endl;
    std::cout<<"-------------------------\n";
    bzero(buffer,1024);
    std::strcpy(buffer,"hello mapper");
    write(new_socket_fd,buffer,strlen(buffer));
    bzero(buffer,1024);
    read(new_socket_fd,buffer,1024);
    std::string msg(buffer);
    if(msg=="done")
    {
            pthread_mutex_lock(&counter_lock);
            mapper_counter++; 
            std::cout<<"counter:"<<mapper_counter<<std::endl;
            pthread_mutex_unlock(&counter_lock);
    }
    if(mapper_counter==MAPPER_COUNT)
    {
        /* launching reducers */
        int port=8080;
        std::string ip;
         std::cout<<"\n============================================================================\n";
        std::cout<<"\n********************************* Reducers *********************************\n";
         std::cout<<"\n=============================================================================\n";
        for(int i=7;i<=9;i++)
        {
            ip="127.0.0."+std::to_string(i);
            std::thread reducer(reducer_thread,ip,i+port,"reducer_"+(std::to_string(i-6)),i-6);
            reducer.detach();
        }
    }

    std::cout<<"master to mapper thread is finished"<<std::endl;
}
