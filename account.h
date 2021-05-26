#ifndef ACCOUNT_H
#define ACCOUNT_H


class account{
private:
    const int _accountNumber;
    const int _password;
    int _balance;

public:
    account(int accountNumber, int password, int balance);
    void deposit(int amount);
    void withdraw(int amount);
    int getBalance() const;
};


#endif //ACCOUNT_H
