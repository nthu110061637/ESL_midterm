#include <cmath>
#include <iomanip>

#include "MergeSort.h"

MergeSort::MergeSort(sc_module_name n)
    : sc_module(n), t_skt("t_skt"), base_offset(0) {
  SC_THREAD(mergesort);

  t_skt.register_b_transport(this, &MergeSort::blocking_transport);
}

void MergeSort::mergesort() {
  unsigned int n;
  while (true) {
    for( n = 0 ; n < arraysize ; n++){
			unsigned char value = unsort.read();
			sortdata[n] = value;	
		}
		if(n==arraysize) 
		{
			int rght, rend;
			int i,j,m;
      /* merge from size of subarray from 1 to 2 to 4 to 8 */
			for (int k=1; k < arraysize; k *= 2 ) {
        /* pick the strating point of different subarrays */      
				for (int left=0; left+k < arraysize; left += k*2 ) { 
					rght = left + k; //ending point of Leftsub        
					rend = rght + k; //ending point of Rightsub
					if (rend > arraysize){
					  rend = arraysize; 
          }
					m = left; 
					i = left; 
					j = rght;
          /* merge left and right array */ 
          for(;(i < rght) && (j < rend); m ++){
							if (sortdata[i] <= sortdata[j]) {         
								sorting[m] = sortdata[i]; 
								i++;
							} 
							else {
								sorting[m] = sortdata[j]; 
								j++;
							}
						}
          /* copy the rest of array */
						for(;i < rght;i++){
							sorting[m]=sortdata[i]; 
							m++;
						}
						for(;j < rend;j++){
							sorting[m]=sortdata[j]; 
							m++;
						}
						
						for (m=left; m < rend; m++) { 
							//HLS_PIPELINE_LOOP(SOFT_STALL,1,"COMP");
							sortdata[m] = sorting[m]; 
						}
				}
			}
			for(i = 0 ; i < arraysize ; i++){
				unsigned char sortvalue = sorting[i];
				sorted.write(sortvalue); 
			}
		}
  }
}

void MergeSort::blocking_transport(tlm::tlm_generic_payload &payload,
                                     sc_core::sc_time &delay) {
  sc_dt::uint64 addr = payload.get_address();
  addr -= base_offset;
  unsigned char *mask_ptr = payload.get_byte_enable_ptr();
  unsigned char *data_ptr = payload.get_data_ptr();
  word buffer;
  switch (payload.get_command()) {
  case tlm::TLM_READ_COMMAND:
    switch (addr) {
    case MERGE_RESULT_ADDR:
      buffer.uc[0] = sorted.read();
      break;
    case MERGE_CHECK_ADDR:
      buffer.uc[0] = sorted.num_available();
    break;
    default:
      std::cerr << "Error! MergeSort::blocking_transport: address 0x"
                << std::setfill('0') << std::setw(8) << std::hex << addr
                << std::dec << " is not valid" << std::endl;
    }
    data_ptr[0] = buffer.uc[0];
    data_ptr[1] = buffer.uc[1];
    data_ptr[2] = buffer.uc[2];
    data_ptr[3] = buffer.uc[3];
    break;
  case tlm::TLM_WRITE_COMMAND:
    switch (addr) {
    case MERGE_R_ADDR:
      unsort.write(data_ptr[0]);
      break;
    default:
      std::cerr << "Error! MergeSort::blocking_transport: address 0x"
                << std::setfill('0') << std::setw(8) << std::hex << addr
                << std::dec << " is not valid" << std::endl;
    }
    break;
  case tlm::TLM_IGNORE_COMMAND:
    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    return;
  default:
    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    return;
  }
  payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
}