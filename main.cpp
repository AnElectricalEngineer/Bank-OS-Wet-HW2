#include "atm.h"
#include "account.h"
#include "bank.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

using namespace std;

/*
 * //TODO maybe add description
 */

// TODO look at red note page 2 bottom

// Global - accounts
map<int, account> accounts;

int main(int argc, char* argv[])
{
    int N; // Number of ATMS in the bank
    vector<ATMinfo> ATM_infos; // Stores serial numbers and file descriptors
    // for atm operations

    // If not enough arguments, exit
    if(argc < 3)    //TODO maybe 2 is ok
    {
        cerr << "illegal arguments\n";
        exit(-1);
    }

    try
    {
        N = std::stoi(argv[1]);
    }
    catch(std::invalid_argument&) // If N was not an int, exit
    {
        cerr << "illegal arguments\n";
        exit(-1);
    }

    // If not enough ATMs, exit
    if(N < 1) // TODO check if 0 ATMs is allowed, or if 1 is the minimum
    {
        cerr << "illegal arguments\n";
        exit(-1);
    }

    // If number of ATMs doesn't match number of input files, exit
    if(N != argc - 2)
    {
        cerr << "illegal arguments\n";
        exit(-1);
    }

    //TODO check that all arguments except first two are actually files
    for(int i = 2; i < argc; i++)
    {
        const char* pathName = argv[i];
        int fd = open(pathName, O_RDONLY); //TODO close all files at end!

        // If opening file failed
        if(fd == -1)
        {
            perror("Bad file name"); //TODO check that this is what should be
            // printed
            exit(-1);
        }

        // Create a new struct for each ATM and add to collection of ATMs
        ATMinfo atmInfo = ATMinfo{.serialNumber = -1, .fileDescriptor = fd};
        ATM_infos.push_back(atmInfo);
    }

    // Create thread for bank
    pthread_t bankThread;
    int threadCreateSuccess = pthread_create(&bankThread, nullptr, bankFunc,
                                             nullptr);
    // pthread_create returns 0 on success
    if(threadCreateSuccess)
    {
        perror("Error creating thread"); //TODO check that this is what
        // should be printed
        exit(-1);
    }

    // Create N threads for N ATMs
    pthread_t ATM_threads[N];
    for(int i = 0; i < N; i++)
    {
        threadCreateSuccess = pthread_create(&ATM_threads[i], nullptr, atmFunc,
                                       (void*)&ATM_infos.at(i));
        if(threadCreateSuccess)
        {
            perror("Error creating thread"); //TODO check that this is what
            // should be printed
            exit(-1);
        }
    }

    for(int i = 0; i < N; i++)
    {
        // pthread_join returns 0 on success
        int threadJoinSuccess = pthread_join(ATM_threads[i], nullptr);
        if(threadJoinSuccess)
        {
            perror("Error joining with thread"); //TODO check that this is what
            // should be printed
            exit(-1);
        }
    }

    return 0;
}