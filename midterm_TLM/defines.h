#ifndef _DEFINES_H_
#define _DEFINES_H_
const int arraysize = 10;
const int num = 10;
//typedef sc_uint < 8 > input_t;
//typedef sc_uint < 8 > output_t;
typedef unsigned char io;
// Merge Sort inner transport addresses
// Used between blocking_transport() & merge()
const int MERGE_R_ADDR = 0x00000000;
const int MERGE_RESULT_ADDR = 0x00000004;
const int MERGE_CHECK_ADDR = 0x00000008;

union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};
#endif // _DEFINES_H_
