#include "account.h"

account::account(string password, int balance):_password(password),
_balance(balance), _readerCnt(0)
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

void account::deposit(int amount)
{
    _balance += amount;
}

void account::withdraw(int amount)
{
    _balance -= amount;
}

int account::getBalance() const
{
    return _balance;
}

bool account::checkPassword(string password) const
{
    return (password == _password);
}

string account::getPassword() const
{
    return _password;
}

void account::enterReader()
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

void account::exitReader()
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

void account::enterWriter()
{
    if(pthread_mutex_lock(&_writeLock) != 0)
    {
        perror("Error");
        exit(-1);
    }
}

void account::exitWriter()
{
    if(pthread_mutex_unlock(&_writeLock) != 0)
    {
        perror("Error");
        exit(-1);
    }
}