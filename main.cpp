#include <iostream>
#include <thread>
#include <unistd.h>
#include "master.hpp"
int main(int argc, char const *argv[])
{
    std::thread master(master_thread,"input.txt");
    sleep(1);
    init_system();
    master.join();
    return 0;
}
