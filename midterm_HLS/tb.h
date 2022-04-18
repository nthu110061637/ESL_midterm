
#ifndef TB_H
#define TB_H

//const int arraysize = 10;

#include <cynw_p2p.h>	// The P2P port header file
#include "defines.h"	// The type definitions for the input and output

SC_MODULE( tb )
{
	sc_in< bool >	clk;		// Declare clock and reset ports
	sc_out< bool >	rst;		// The source thread drives rst_out.

	cynw_p2p < input_t >::out dout; // data going in to the design
	cynw_p2p < output_t >::in din;  // data going out of the design
  sc_time total_latency;
  sc_time total_start_time;
	SC_CTOR( tb )
	: clk( "clk" )
	, rst( "rst" )
	, din( "din" )
	, dout( "dout" )
	{
		// Declare the source and sink threads. Since they 
		// drive the reset signal, no reset_signal_is() is needed
		SC_CTHREAD( source, clk.pos() );
		SC_CTHREAD( sink, clk.pos() );

		// connect clk/rst to cynw_p2p ports
		din.clk_rst(clk, rst);
		dout.clk_rst(clk, rst);
	}
  
	void source();
	void sink();
	sc_time start_time[arraysize];	// sc_time array to store times that input is sent to DUT

	FILE *infp;			// File pointer for stimulus file
	FILE *outfp;		// File pointer for results file
};

#endif // TB_H
