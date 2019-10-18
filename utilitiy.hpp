#ifndef UTILITY_INCLUDE
#define UTILITY_INCLUDE
#include<string>

void sort_file(std::string str);
int getWordsCount(std::string);
void merge_files(std::ifstream *,std::string);
int calhash(std::string);
void mount_fs(char *);
#endif