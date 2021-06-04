#ifndef _BANK_H
#define _BANK_H

#include <map>
#include <fstream>

#include "account.h"

using namespace std;

class accounts
{
private:
    pthread_mutex_t _readLock, _writeLock;
    int _readerCnt;
public:
    map<int, account*> _accounts{};

    accounts();
    ~accounts();
    void enterReader();
    void exitReader();
    void enterWriter();
    void exitWriter();
};

class Logfile
{
private:
    pthread_mutex_t _writeLock;

public:
    ofstream _logfile;

    Logfile();
    void enterWriter();
    void exitWriter();
};

class BankMoney
{
private:
    unsigned int _bankBalance;
    pthread_mutex_t _lock;

public:
    BankMoney();
    unsigned int getBankBalance() const;
    void addAmount(int amount);
    void lockBankBalance();
    void unlockBankBalance();
};

void* takeFee(void* arg);
void* printStatus(void* arg);

#endif //_BANK_H
