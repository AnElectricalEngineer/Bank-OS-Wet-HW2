#include "bank.h"

#include <cmath>
#include <unistd.h>
#include <iostream>
#include <random>

#define BANK_FEE_INTERVAL 3 // Time in seconds between when bank takes fees
// Time in MICROSECONDS between when the status is printed
#define STATUS_PRINT_INTERVAL 500000

using namespace std;

extern accounts Accounts;
extern BankMoney bankMoney;
extern Logfile logfile;

accounts::accounts():_readerCnt(0)
{
    if(pthread_mutex_init(&_readLock, nullptr) != 0)
    {
        perror("Error");
        exit(-1);
    }
    if(pthread_mutex_init(&_writeLock, nullptr) != 0)
    {
        perror("Error");
        exit(-1);
    }
}

accounts::~accounts()
{
    for (auto & account : Accounts._accounts)
    {
        delete account.second;
    }
}

void accounts::enterReader()
{
    if(pthread_mutex_lock(&_readLock) != 0)
    {
        perror("Error");
        exit(-1);
    }
    _readerCnt++;
    if(_readerCnt == 1)
    {
        if(pthread_mutex_lock(&_writeLock) != 0)
        {
            perror("Error");
            exit(-1);
        }
    }
    if(pthread_mutex_unlock(&_readLock) != 0)
    {
        perror("Error");
        exit(-1);
    }
}

void accounts::exitReader()
{
    if(pthread_mutex_lock(&_readLock) != 0)
    {
        perror("Error");
        exit(-1);
    }
    _readerCnt--;
    if(_readerCnt == 0)
    {
        if(pthread_mutex_unlock(&_writeLock) != 0)
        {
            perror("Error");
            exit(-1);
        }
    }
    if(pthread_mutex_unlock(&_readLock) != 0)
    {
        perror("Error");
        exit(-1);
    }
}

void accounts::enterWriter()
{
    if(pthread_mutex_lock(&_writeLock) != 0)
    {
        perror("Error");
        exit(-1);
    }
}

void accounts::exitWriter()
{
    if(pthread_mutex_unlock(&_writeLock) != 0)
    {
        perror("Error");
        exit(-1);
    }
}

Logfile::Logfile()
{
    if(pthread_mutex_init(&_writeLock, nullptr) != 0)
    {
        perror("Error");
        exit(-1);
    }
}

void Logfile::enterWriter()
{
    if(pthread_mutex_lock(&_writeLock) != 0)
    {
        perror("Error");
        exit(-1);
    }
}

void Logfile::exitWriter()
{
    if(pthread_mutex_unlock(&_writeLock) != 0)
    {
        perror("Error");
        exit(-1);
    }
}

BankMoney::BankMoney():_bankBalance(0)
{
    if(pthread_mutex_init(&_lock, nullptr) != 0)
    {
        perror("Error");
        exit(-1);
    }
}

unsigned int BankMoney::getBankBalance() const
{
    return _bankBalance;
}

void BankMoney::addAmount(int amount)
{
    _bankBalance += amount;
}

void BankMoney::lockBankBalance()
{
    if(pthread_mutex_lock(&_lock) != 0)
    {
        perror("Error");
        exit(-1);
    }
}

void BankMoney::unlockBankBalance()
{
    if(pthread_mutex_unlock(&_lock) != 0)
    {
        perror("Error");
        exit(-1);
    }
}

void* takeFee(void* arg)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(2, 4);

    while(true)
    {
        sleep(BANK_FEE_INTERVAL);

        // Check if all ATMs have finished their tasks
        auto bankThreadsLock = (pthread_mutex_t*)arg;
        if (!pthread_mutex_trylock(bankThreadsLock))
        {
            if(pthread_mutex_unlock(bankThreadsLock) != 0)
            {
                perror("Error");
                exit(-1);
            }
            pthread_exit(nullptr);
        }
        Accounts.enterReader();

        double feePercent = (double)distrib(gen)/100.0;

        for(auto & account : Accounts._accounts)
        {
            account.second->enterWriter();
            int balance = account.second->getBalance();
            int fee = round(feePercent * balance);
            account.second->withdraw(fee);
            bankMoney.lockBankBalance();
            bankMoney.addAmount(fee);
            bankMoney.unlockBankBalance();
            logfile.enterWriter();
            logfile._logfile << "Bank: commissions of " << (int)(feePercent *
            100) << " % were charged, the bank gained " << fee << " $ from "
            "account " << account.first  << endl;
            logfile.exitWriter();
            account.second->exitWriter();
        }
        Accounts.exitReader();
    }
}

void* printStatus(void* arg)
{
    while(true)
    {
        usleep(STATUS_PRINT_INTERVAL);

        // Check if all ATMs have finished their tasks
        auto bankThreadsLock = (pthread_mutex_t*)arg;
        if(!pthread_mutex_trylock(bankThreadsLock))
        {
            if(pthread_mutex_unlock(bankThreadsLock) != 0)
            {
                perror("Error");
                exit(-1);
            }
            pthread_exit(nullptr);
        }
        printf("\033[2J");
        printf("\033[1;1H");
        cout << "Current Bank Status" << endl;
        Accounts.enterReader();
        for(auto & account : Accounts._accounts)
        {
            account.second->enterReader();
            string accountNumber = account.first;
            int balance = account.second->getBalance();
            string password = account.second->getPassword();
            account.second->exitReader();
            cout << "Account " << accountNumber << ": Balance - " << balance
            << " $ , Account Password - " << password << endl;
        }
        Accounts.exitReader();
        bankMoney.lockBankBalance();
        unsigned int currentBalance = bankMoney.getBankBalance();
        cout << "The Bank has " << currentBalance << " $" << endl;
        bankMoney.unlockBankBalance();
    }
}
