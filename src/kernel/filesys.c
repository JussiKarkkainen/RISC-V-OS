#include "filesys.h"


// LOGGING
// The logging layer provides security in case of a crash by keeping a log of disk writes.
// 


struct log_header {
    // Used to indicate if there are transactions in the log, if yes holds how many blocks it contains
    int count;             
    // Array of sector numbers, one for each logged block
    int block[LOGSIZE];
}



