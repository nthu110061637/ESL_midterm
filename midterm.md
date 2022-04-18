# Midterm Project

### Information

---

**110061637  林佳瑩**

**Github link** **:** [https://github.com/nthu110061637/ESL_midterm.git](https://github.com/nthu110061637/ESL_midterm.git)

---

### Introduction

I choose Merge Sort algorithm as my midterm project design and port it on to the TLM platform and do the HLS and optimization.

---

### Implementation

### HLS

dut.cc

```cpp
unsigned int n;
	while( true )
	{
		/* read the data to sorted */
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
			/* do the merge sort */
			for (int k=1; k < arraysize; k *= 2 ) {
			/* merge from size of subarray from 1 to 2 to 4 to 8 */   
				for (int left=0; left+k < arraysize; left += k*2 ) {
					/* pick the strating point of different subarrays */
					//HLS_CONSTRAIN_LATENCY(0,9,"LOOP");
					rght = left + k;  //ending point of Leftsub      
					rend = rght + k;  //ending point of Rightsub
					if (rend > arraysize) {
					rend = arraysize; }
			
					m = left; 
					i = left; 
					j = rght;
					/* merge left and right array */
					for(m = left;(i < rght) && (j < rend); m ++){
						//HLS_CONSTRAIN_LATENCY(0,1,"FLOOP1");
						HLS_CONSTRAIN_LATENCY(0,3,"LOOP1");
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
						//HLS_CONSTRAIN_LATENCY(0,1,"FLOOP2");
						HLS_CONSTRAIN_LATENCY(0,2,"LOOP2");
						sorting[m]=sortdata[i]; 
						m++;
					}
					for(;j < rend;j++){
						//HLS_CONSTRAIN_LATENCY(0,1,"FLOOP3");
						HLS_CONSTRAIN_LATENCY(0,2,"LOOP3");
						sorting[m]=sortdata[j]; 
						m++;
					}
					for (m=left; m < rend; m++) { 
						//HLS_CONSTRAIN_LATENCY(0,1,"FLOOP4");
						HLS_CONSTRAIN_LATENCY(0,2,"LOOP4");
						sortdata[m] = sorting[m]; 
					}
					
				}
			}
			/* write the data have been sorted */
			for(int i = 0 ; i < arraysize ; i++){
				HLS_DEFINE_PROTOCOL("output");
				output_t sortvalue = (output_t)sorting[i];
				wait();
				dout.put(sortvalue); 
			}
		}
	}
```

dut.h

```cpp
/* declaration of channel and the data structure use at dut module */
cynw_p2p < input_t >::in	din;	// The input port
cynw_p2p < output_t >::out	dout;	// The output port
sc_dt::sc_uint<8> sortdata[arraysize]; // array to put data to be sorted
sc_dt::sc_uint<8> sorting[arraysize]; // array to put the data have been sorted
```

tb.cc

I read 10 items for 10 times from “stimulus.dat”.

```cpp
void tb::source()
{
	...
	total_start_time = sc_time_stamp();
	// record the time that the first item have been send out 
	// Read stimulus values from file and send to DUT
	for( int r = 0 ; r < 10 ; r++){
		for( int i = 0; i < arraysize; i++ )
		{
			int value;
			fscanf(infp, "%d\n", &value);
			dout.put( (input_t)value );
			wait(2);
		}
	}
	...
}

void tb::sink()
{
	....
	// Read outputs from DUT
	for( int r = 0 ; r < 10 ; r ++){
		for( int i = 0; i < arraysize; i++ )
		{
			output_t inVal = din.get();
			fprintf( outfp, "%d\n", (int)inVal);
			
		// write value to response file
		}
		// record the time that the last item have been retrieve
		total_latency = sc_time_stamp() - total_start_time ;
		cout << "Total latency " << total_latency << "." << endl;
		fprintf( outfp, "\n");
	}
	...
}
```

defines.h

```cpp
#ifndef _DEFINES_H_
#define _DEFINES_H_
const int arraysize = 10; // 10 items
const int num = 10;  // 10 times
typedef sc_uint < 8 > input_t;
typedef sc_uint < 8 > output_t;
#endif // _DEFINES_H_
```

system.h

```cpp
...
SC_MODULE( System )
{
	...

	// cynw_p2p channels
	cynw_p2p< input_t >	chan1;	// For data going into the design
	cynw_p2p< output_t > chan2;	// For data coming out of the design

	// The testbench and DUT modules.
	dut_wrapper	*m_dut;		// use the generated wrapper for the DUT hls module
	/* we have to connect testbench to different level of design (systemc, cycle-accurate systemC model, RTL, netlisst etc.) */
	/* wrapper will used to convert all signal into the connection that tb can connect to */
	tb		*m_tb;

	SC_CTOR( System )
	: clk_sig( "clk_sig", CLOCK_PERIOD, SC_NS )
	, rst_sig( "rst_sig" )
	, chan1( "chan1" )
	, chan2( "chan2" )
	{
		// Connect the design module
		m_dut = new dut_wrapper( "dut_wrapper" );
		m_dut->clk( clk_sig );
		m_dut->rst( rst_sig );
		m_dut->din( chan1 );
		m_dut->dout( chan2 );

		// Connect the testbench
		m_tb = new tb( "tb" );
		m_tb->clk( clk_sig );
		m_tb->rst( rst_sig );
		m_tb->dout( chan1 );
		m_tb->din( chan2 );
	}
...
```

### TLM

The code in TLM is almost the same and is similar to the lab4.

And we read data from stimulus.txt and output the data to response.txt.

Teestbench.cpp : use initiator.write_to_socket() and read_from_socket() to send out the data and take the data back with data.uc[0] to transfer the unsigned char data that we want to do sorting.

```cpp
void Testbench::do_sort() {
	total_start_time = sc_time_stamp();
  for(int j = 0 ; j < num ; j++){
  for( int i = 0; i < arraysize; i++ )
	{
    word data;
    data.uc[0] = (unsigned char)data_to_be_sort[j][i];
    mask[0] = 0xff;
    mask[1] = 0;
    mask[2] = 0;
    mask[3] = 0;
    initiator.write_to_socket(MERGE_MM_BASE + MERGE_R_ADDR, mask
                                    ,data.uc, 4);
		start_time[i] = sc_time_stamp();
    //wait(1 * CLOCK_PERIOD, SC_NS);
	}
  
  word data;
	// Read outputs from DUT
	bool done=false;
  int output_num=0;
  while(!done){
    initiator.read_from_socket(MERGE_MM_BASE + MERGE_CHECK_ADDR, mask, data.uc, 4);
    output_num = data.sint;
    if(output_num>0) done=true;
  }
  wait(247* CLOCK_PERIOD , SC_NS);
	
	for( int i = 0; i < arraysize; i++ )
	{
    initiator.read_from_socket(MERGE_MM_BASE + MERGE_RESULT_ADDR, mask,
                                 data.uc, 4);
		data_to_be_sort[j][i] = (int)data.uc[0];	// write value to response file
	}
  }
	sc_stop();
}
```

---

### Result

### HLS (clock period=5ns)

|  | Original | L | F | FL | FL1234 |
| --- | --- | --- | --- | --- | --- |
| Area | 5234.8 | 6447.5 | 9337.5 | 9763 | 10792.6 |
| Simulation time
(cycles) | 2627 | 2183 | 2237 | 1833 | 1303 |

The directives need to turn on at [dut.cc](http://dut.cc) or project.tcl manually.

Original : Without doing any optimization.

![Original_area.PNG](https://raw.githubusercontent.com/nthu110061637/ESL_midterm/main/midterm_result_pic/Original_area.PNG)

![Original_time.PNG](https://raw.githubusercontent.com/nthu110061637/ESL_midterm/main/midterm_result_pic/Original_time.PNG)

L : HLS_CONSTRAIN_LATENCY(0,9,"LOOP");

![L_area.PNG](https://raw.githubusercontent.com/nthu110061637/ESL_midterm/main/midterm_result_pic/L_area.PNG)

![L_time.PNG](https://raw.githubusercontent.com/nthu110061637/ESL_midterm/main/midterm_result_pic/L_time.PNG)

F : set_attr flatten_arrays	none ;  →  set_attr flatten_arrays	all ; (project.tcl)

![F_area.PNG](https://raw.githubusercontent.com/nthu110061637/ESL_midterm/main/midterm_result_pic/F_area.PNG)

![F_time.PNG](https://raw.githubusercontent.com/nthu110061637/ESL_midterm/main/midterm_result_pic/F_time.PNG)

FL : L + F

![FL_area.PNG](https://raw.githubusercontent.com/nthu110061637/ESL_midterm/main/midterm_result_pic/FL_area.PNG)

![FL_time.PNG](https://raw.githubusercontent.com/nthu110061637/ESL_midterm/main/midterm_result_pic/FL_time.PNG)

FL1234 : F + HLS_CONSTRAIN_LATENCY(0,1,"FLOOP1"); 
+ HLS_CONSTRAIN_LATENCY(0,1,"FLOOP2");
+ HLS_CONSTRAIN_LATENCY(0,1,"FLOOP3");
+ HLS_CONSTRAIN_LATENCY(0,1,"FLOOP4");

![FL1234_area.PNG](https://raw.githubusercontent.com/nthu110061637/ESL_midterm/main/midterm_result_pic/FL1234_area.PNG)

![FL1234_time.PNG](https://raw.githubusercontent.com/nthu110061637/ESL_midterm/main/midterm_result_pic/FL1234_time.PNG)

### TLM

I get the total latency of 1235 ns when doing 10 items sorting for 1 times.

|  | Simulated time (ns) |
| --- | --- |
| HLS | 12365 |
| TLM | 14450 |

![HLS_simulation_time.png](https://raw.githubusercontent.com/nthu110061637/ESL_midterm/main/midterm_result_pic/HLS_simulation_time.png)

![TLM_wait.png](https://raw.githubusercontent.com/nthu110061637/ESL_midterm/main/midterm_result_pic/TLM_wait.png)

![TLM_simulation_time.png](https://raw.githubusercontent.com/nthu110061637/ESL_midterm/main/midterm_result_pic/TLM_simulation_time.png)

### Demo
HLS
![Demo_HLS.png](https://raw.githubusercontent.com/nthu110061637/ESL_midterm/main/midterm_result_pic/Demo_HLS.png)

TLM
![Demo_TLM.png](https://raw.githubusercontent.com/nthu110061637/ESL_midterm/main/midterm_result_pic/Demo_TLM.png)




