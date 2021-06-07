#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <pthread.h>
#include <string>

using namespace std;

class account{
private:
    const string _password;
    int _balance;
    pthread_mutex_t _readLock, _writeLock;
    int _readerCnt;

public:
    account(string password, int balance);
    void deposit(int amount);
    void withdraw(int amount);
    int getBalance() const;
    bool checkPassword(string password) const;
    string getPassword() const;
    void enterReader();
    void exitReader();
    void enterWriter();
    void exitWriter();
};


#endif //ACCOUNT_H
