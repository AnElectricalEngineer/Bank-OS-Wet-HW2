#ifndef _BANK_H
#define _BANK_H

//TODO I think make several bank functions to go along with several bank
// threads, each one does something different, eg one prints, and one takes
// fees...

void* takeFee(void* arg);
void* printStatus(void* arg);

#endif //_BANK_H
