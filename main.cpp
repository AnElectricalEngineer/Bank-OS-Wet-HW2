#include "atm.h"
#include "account.h"
#include "bank.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <pthread.h>

using namespace std;

// Globals
accounts Accounts; // Accounts. Holds mapping of account numbers to accounts.
BankMoney bankMoney; // The balance of the bank itself
Logfile logfile; // Logfile


int main(int argc, char* argv[])
{
    int N{}; // Number of ATMS in the bank
    vector<ATMinfo> ATM_infos{}; // Stores serial numbers and file descriptors
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

    // Check that all arguments except first two are actually files
    for(int i = 2; i < argc; i++)
    {
        const char* pathName = argv[i];
        ifstream ATM_file(pathName);

        // If opening file failed
        if(!ATM_file.is_open())
        {
            cerr << "illegal arguments" << endl;
            exit(-1);
        }
        ATM_file.close();

        // Create a new struct for each ATM and add to collection of ATMs
        ATMinfo atmInfo = ATMinfo{i - 1, pathName};
        ATM_infos.push_back(atmInfo);
    }

    // Create logfile - overwrite content
    logfile._logfile.open("log.txt", ofstream::trunc);
    if(!logfile._logfile.is_open())
    {
        cerr << "Error creating logfile" << endl;
        exit(-1);
    }

    // Lock that remains locked while ATMs have tasks
    pthread_mutex_t bankThreadsLock;

    // Returns 0 if successful
    if(pthread_mutex_init(&bankThreadsLock, nullptr) != 0)
    {
        perror("Error");
        exit(-1);
    }
    if(pthread_mutex_lock(&bankThreadsLock) != 0)
    {
        perror("Error");
        exit(-1);
    }

    // Create thread for taking fee from accounts
    pthread_t feeThread{};
    int threadCreateSuccess = pthread_create(&feeThread, nullptr, takeFee,
                                             (void*)&bankThreadsLock);
    // pthread_create returns 0 on success
    if(threadCreateSuccess != 0)
    {
        perror("Error");
        exit(-1);
    }

    // Create thread for printing status
    pthread_t printingStatusThread{};
    threadCreateSuccess = pthread_create(&printingStatusThread, nullptr,
                                         printStatus,(void*)&bankThreadsLock);
    if(threadCreateSuccess != 0)
    {
        perror("Error");
        exit(-1);
    }

    // Create N threads for N ATMs
    auto ATM_threads = new pthread_t[N];
    for(int i = 0; i < N; i++)
    {
        threadCreateSuccess = pthread_create(&ATM_threads[i], nullptr, atmFunc,
                                       (void*)&ATM_infos.at(i));
        if(threadCreateSuccess != 0)
        {
            perror("Error");
            exit(-1);
        }
    }

    for(int i = 0; i < N; i++)
    {
        // pthread_join returns 0 on success
        int threadJoinSuccess = pthread_join(ATM_threads[i], nullptr);
        if(threadJoinSuccess != 0)
        {
            perror("Error");
            exit(-1);
        }
    }

    // Notify bank threads that all ATM tasks are done
    if(pthread_mutex_unlock(&bankThreadsLock) != 0)
    {
        perror("Error");
        exit(-1);
    }
    if(pthread_join(feeThread, nullptr) != 0)
    {
        perror("Error");
        exit(-1);
    }
    if(pthread_join(printingStatusThread, nullptr) != 0)
    {
        perror("Error");
        exit(-1);
    }

    if(pthread_mutex_destroy(&bankThreadsLock) != 0)
    {
        perror("Error");
        exit(-1);
    }
    logfile._logfile.close();
    delete[] ATM_threads;
    return 0;
}