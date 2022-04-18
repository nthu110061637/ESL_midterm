
#include "tb.h"
#include <esc.h>		// for the latency logging functions
#include <string>
#include <iostream>

void tb::source()
{
	dout.reset();		// reset the outputs and cycle the design's reset
	rst.write( 0 );		// assert reset (active low)
	wait( 2 );		// wait 2 cycles
	rst.write( 1 );		// deassert reset
	wait(1);
	// Open the stimulus file
	char stim_file[256] = "stimulus.dat";
	infp = fopen( stim_file, "r" );
	if( infp == NULL )
	{
		cout << "Couldn't open stimulus.dat for reading." << endl;
		exit(0);
	}
	total_start_time = sc_time_stamp();
	// Read stimulus values from file and send to DUT
	for( int r = 0 ; r < 10 ; r++){
		for( int i = 0; i < arraysize; i++ )
		{
			int value;
			fscanf(infp, "%d\n", &value);
			dout.put( (input_t)value );
			//start_time[i] = sc_time_stamp();
			wait(2);	// mark the time value was sent
		}
	}
	// Guard condition: after 100000 cycles the sink() thread should have ended the simulation.
	// If we're still here, timeout and print error message.
	wait( 100000 );
    fclose( infp );
	cout << "Error! TB source thread timed out!" << endl;
	esc_stop();
}

// The sink thread reads all the expected values from the design
void tb::sink()
{
	din.reset();
	wait(2);     // to synchronize with reset
	wait(1);
	// Extract clock period from clk port
	sc_clock * clk_p = DCAST < sc_clock * >( clk.get_interface() );
	sc_time clock_period = clk_p->period();

	// Open the simulation results file
	char output_file[256];
	sprintf( output_file, "%s/response.dat", getenv("BDW_SIM_CONFIG_DIR") );
	outfp = fopen(output_file, "wb");
	if (outfp == NULL)
	{
		cout << "Couldn't open output.dat for writing." << endl;
		exit(0);
	}

	// Read outputs from DUT
	
	for( int r = 0 ; r < 10 ; r ++){
		for( int i = 0; i < arraysize; i++ )
		{
			output_t inVal = din.get();
			fprintf( outfp, "%d\n", (int)inVal);
			
		// write value to response file
		}
		total_latency = sc_time_stamp() - total_start_time ;
		cout << "Total latency " << total_latency << "." << endl;
		fprintf( outfp, "\n");
	}
	total_latency = sc_time_stamp() - total_start_time ;
	cout << "Total latency " << total_latency << "." << endl;
	// Close the results file and end the simulation
	fclose( outfp );
	esc_stop();
}
