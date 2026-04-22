#ifndef ERR_H
#define ERR_H

#define SUCCESS     0

#define ERR         1
#define CONINITERR  2 // Connection initialization error 
#define NETINITERR  3 // Network initialization error
#define PARINITERR  4 // Parser initialization error
#define PARPARERR   5 // Parser parser error (idk.. sounded funny, okay?) PAR PAR ERR 
#define PKGNOTFND   6 // Package not found                    
#define USAGE       7 // Usage screen displayed instead of cmd exec.

/**
 *  Base errors.
 *  Substract these values to determine library errors.
 */
#define CONREQERR   100

#endif
