/*
 * @File:   bit_operation.h 
 * @Author: liuguang 
 * @Date:   2017-09-30 17:05:51 
 * 
 * @LICENSE: 
 * The code owner is liuguang. Other personnel may 
 * not be used or modified.
 * 
 * Change Logs: 
 * Date           Author       Notes 
 * 2017-09-30     liuguang     The first version. 
 */ 
 
#ifndef __BIT_OPERATION_H_ 
#define __BIT_OPERATION_H_ 

#define BITx(n) (1 << ((n)-1))
#define BITS_8  (0xFF)
#define BITS_16 (0xFFFF)
#define BITS_32 (0xFFFFFFFF) 

#define BITS_SET(data, bits)  ((data) |=  (bits)) 
#define BITS_CLR(data, bits)  ((data) &= ~(bits)) 
#define BITS_TGL(data, bits)  ((data) ^=  (bits)) 
#define BITS_READ(data, bits) ((data) &   (bits))

#endif
