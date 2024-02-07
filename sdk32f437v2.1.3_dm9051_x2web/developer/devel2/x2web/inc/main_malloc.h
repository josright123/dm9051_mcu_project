#ifndef __MMAINALLOC_H
#define __MMAINALLOC_H

#define MMALLOC_MAX_LEN		15 //255 //125 //125
#define MMALLOC_MAX_LEN1	260 //125 //1024

#define MMALLOC_MAX_LEN2	250

//
// Note: when init, malloc init(MMALLOC_MAX_LEN	15) + tx(MMALLOC_MAX_LEN2	250) = 265 can succeed.
// Note: when loop middle, malloc looptest(MMALLOC_MAX_LEN1	260) can be OK.
// Note: when tx, malloc tx(MMALLOC_MAX_LEN2	250) can be OK.
//

#endif //__MMAINALLOC_H
