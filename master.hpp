#ifndef MASTER_INCLUDE
#define MASTER_INCLUDE
#include<string>
#include <atomic>

 static  pthread_mutex_t mapper_lock;
 static  pthread_mutex_t reducer_lock;
 static  pthread_mutex_t file_lock; 
 static  pthread_mutex_t counter_lock;
 static  std::atomic<int> mapper_counter(0);
static   std::atomic<int> reducer_counter(0); 
void init_system();
void master_thread(std::string);
void init_master_connection(int*);
void cleanmaster();
void init_file_partition(int,std::string);

#endif
