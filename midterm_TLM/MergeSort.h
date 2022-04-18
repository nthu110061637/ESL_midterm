#ifndef MERGE_SORT_H_
#define MERGE_SORT_H_
#include <systemc>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "defines.h"

class MergeSort : public sc_module {
public:
  tlm_utils::simple_target_socket<MergeSort> t_skt;

  sc_fifo<unsigned char> unsort;
  sc_fifo<unsigned char> sorted;

  SC_HAS_PROCESS(MergeSort);
  MergeSort(sc_module_name n);
  ~MergeSort() = default;

private:
  void mergesort();
  unsigned char sortdata[arraysize];
	unsigned char sorting[arraysize];
  unsigned int base_offset;
  void blocking_transport(tlm::tlm_generic_payload &payload,
                          sc_core::sc_time &delay);
};
#endif
