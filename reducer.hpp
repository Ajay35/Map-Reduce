#ifndef REDUCER_INCLUDE
#define REDUCER_INCLUDE
#include<string>
#include <atomic>


void reduce(std::string);
void reducer_thread(std::string,int,std::string,int);
void killer_thread(std::string,int,std::string);
void master_to_reducer_thread(int,struct sockaddr_in client_addr);

#endif
