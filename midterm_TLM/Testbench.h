#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <string>
using namespace std;

#include <systemc>
using namespace sc_core;

#include "Initiator.h"
#include "MemoryMap.h"
#include "defines.h"

class Testbench : public sc_module {
public:
  Initiator initiator;

  SC_HAS_PROCESS(Testbench);
  Testbench(sc_module_name n);
  ~Testbench();
  void read_data(string infile_name);
  void write_data(string outfile_name);

private:
  void do_sort();
  
	sc_time start_time[arraysize];	// sc_time array to store times that input is sent to DUT
  sc_time total_start_time;
  sc_time total_latency;
	FILE *infp;			// File pointer for stimulus file
	FILE *outfp;
  unsigned char mask[4] = {0};
};
#endif
