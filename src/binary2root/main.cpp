#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "CompassUnpacker.h"

using namespace std;

int main(int argc, char *argv[]) {
  if(argc == 2) {
    ifstream input(argv[1]);
    string junk, data, output;
    input>>junk>>data;
    input>>junk>>output;
    input.close();
    cout<<"---------binary2root---------"<<endl;
    cout<<"Name of data file: "<<data<<endl;
    cout<<"Name of output file: "<<output<<endl;
    CompassUnpacker goodluck(data);
    goodluck.Run(output);
    cout<<"-----------------------------"<<endl;
    return 0;
  } else { 
    cerr<<"No input file given; need a file which gives fullpath name of file to be"
        <<" parsed and the fullpath name of the file to be created."<<endl;
    return 1;
  }
}
