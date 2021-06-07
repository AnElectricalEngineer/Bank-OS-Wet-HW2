#include "bank.h"

#include <cmath>
#include <unistd.h>
#include <iostream>

#define BANK_FEE_INTERVAL 3 // Time in seconds between when bank takes fees
// Time in MICROSECONDS between when the status is printed
#define STATUS_PRINT_INTERVAL 500000

using namespace std;

extern accounts Accounts;
extern BankMoney bankMoney;
extern Logfile logfile;

accounts::accounts():_readerCnt(0)
{
    // TODO need to check if succeeded like syscall - if failed - print to std
    // error and exit(-1)
    // TODO check if need to destroy in destructor
    pthread_mutex_init(&_readLock, nullptr);
    pthread_mutex_init(&_writeLock, nullptr);
}

//TODO check that necessary (solves one valgrind problem) and correct. Check
// when destructor of global var is called. fix possibly lost errors from
// valgrind (maybe pthread exit somehow from bank threads).
accounts::~accounts()
{
    for (auto & account : Accounts._accounts)
    {
        delete account.second;
    }
}

void accounts::enterReader()
{
    pthread_mutex_lock(&_readLock); // TODO check if sys call
    _readerCnt++;
    if(_readerCnt == 1)
    {
        pthread_mutex_lock(&_writeLock); // TODO check if sys call
    }
    pthread_mutex_unlock(&_readLock); // TODO check if sys call
}

void accounts::exitReader()
{
    pthread_mutex_lock(&_readLock); // TODO check if sys call
    _readerCnt--;
    if(_readerCnt == 0)
    {
        pthread_mutex_unlock(&_writeLock); // TODO check if sys call
    }
    pthread_mutex_unlock(&_readLock); // TODO check if sys call
}

void accounts::enterWriter()
{
    pthread_mutex_lock(&_writeLock); // TODO check if sys call
}

void accounts::exitWriter()
{
    pthread_mutex_unlock(&_writeLock); // TODO check if sys call

}

Logfile::Logfile()
{
    pthread_mutex_init(&_writeLock, nullptr);
}

void Logfile::enterWriter()
{
    pthread_mutex_lock(&_writeLock); // TODO check if sys call
}

void Logfile::exitWriter()
{
    pthread_mutex_unlock(&_writeLock); // TODO check if sys call

}

BankMoney::BankMoney():_bankBalance(0)
{
    // TODO check if need to check if succeeded like syscall
    // TODO check if need to destroy in destructor
    pthread_mutex_init(&_lock, nullptr);
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
    pthread_mutex_lock(&_lock); // TODO check if sys call
}

void BankMoney::unlockBankBalance()
{
    pthread_mutex_unlock(&_lock); // TODO check if sys call
}

void* takeFee(void* arg)
{
    while(true)
    {
        sleep(BANK_FEE_INTERVAL);

        auto bankThreadsLock = (pthread_mutex_t*)arg;
        if(pthread_mutex_trylock(bankThreadsLock)) pthread_exit(nullptr);

        Accounts.enterReader();
        double feePercent = ((double)(rand() % 2) + 2)/100.0;
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
        auto bankThreadsLock = (pthread_mutex_t*)arg;
        if(pthread_mutex_trylock(bankThreadsLock)) pthread_exit(nullptr);
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
