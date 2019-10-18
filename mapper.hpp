#ifndef MAPPER_INCLUDE
#define MAPPER_INCLUDE
#include<string>
#include <atomic>


void mapper_thread(std::string str1,int,std::string str2,int);
void master_to_mapper_thread(int,struct sockaddr_in client_addr);

#endif
