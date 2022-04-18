
#include "dut.h"
#include "stratus_hls.h"
// The thread function for the design
void dut::thread1()
{
	// Reset the interfaces
	// Also initialize any variables that are part of the module class here
	{
		HLS_DEFINE_PROTOCOL( "reset" );
		din.reset();
		dout.reset();
		wait();
	}
	//unsigned int i,j,k,m;
	unsigned int n;
	while( true )
	{
		for( n = 0 ; n < arraysize ; n++){
		
			HLS_DEFINE_PROTOCOL("input");
			input_t value = din.get();
			sortdata[n] = value;
			wait(1);	
		}
		if(n==arraysize) 
		{
			int rght, rend;
			int i,j,m;

			for (int k=1; k < arraysize; k *= 2 ) {
				//HLS_UNROLL_LOOP(ALL,"loop");     
				for (int left=0; left+k < arraysize; left += k*2 ) {
					//HLS_UNROLL_LOOP(ON,"inner"); // not useful
					rght = left + k;        
					rend = rght + k;
					if (rend > arraysize) {
					rend = arraysize; }
			
					m = left; 
					i = left; 
					j = rght;
					for(m = left;(i < rght) && (j < rend); m ++){
						if (sortdata[i] <= sortdata[j]) {         
							sorting[m] = sortdata[i]; 
							i++;
						} 
						else {
							sorting[m] = sortdata[j]; 
							j++;
						}
					}
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
			for(int i = 0 ; i < arraysize ; i++){
				HLS_DEFINE_PROTOCOL("output");
				output_t sortvalue = (output_t)sorting[i];
				wait();
				dout.put(sortvalue); 
			}
		}
	}	
}