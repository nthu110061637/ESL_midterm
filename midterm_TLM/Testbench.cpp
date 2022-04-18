#include <cassert>
#include <cstdio>
#include <cstdlib>
using namespace std;
#include <string>
#include <iostream>
#include "Testbench.h"
/////////////////////////////////////////////////////////////////////////////////////
int data_to_be_sort[num][arraysize]={0};
/////////////////////////////////////////////////////////////////////////////////////
Testbench::Testbench(sc_module_name n)
    : sc_module(n), initiator("initiator") {
  SC_THREAD(do_sort);
}

Testbench::~Testbench() {}
/////////////////////////////////////////////////////////////////////////////////////
void Testbench::read_data(string infile_name){
  infp = NULL;
  infp = fopen(infile_name.c_str(),"r");
  if (infp == NULL) {
    printf("fopen %s error\n", infile_name.c_str());
  }
  for(int j = 0 ; j < num ; j++){
  for(int i = 0 ; i < arraysize ; i++){
    fscanf(infp, "%d", &data_to_be_sort[j][i]);
  }
  }
  fclose(infp);
}
/////////////////////////////////////////////////////////////////////////////////////
void Testbench::write_data(string outfile_name){
  outfp = NULL;
	outfp = fopen(outfile_name.c_str(), "wb");
	if (outfp == NULL)
	{
		cout << "Couldn't open response.txt for writing." << endl;
		exit(0);
	}
  for(int j = 0 ; j < num ; j++){
	for( int i = 0; i < arraysize; i++ )
	{
		fprintf( outfp, "%d\n", data_to_be_sort[j][i]);	// write value to response file
	}
    fprintf( outfp, "\n");
  }
	// Close the results file and end the simulation
	fclose( outfp );
}
/////////////////////////////////////////////////////////////////////////////////////
void Testbench::do_sort() {
  
  //wait(5 * CLOCK_PERIOD, SC_NS);
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


     