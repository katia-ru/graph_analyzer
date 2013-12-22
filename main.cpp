#include "./program_options_parser.h"
#include <iostream>
#include <stdexcept>


using std::cerr;
using std::endl;
using std::exception;


int main(int argc, char** argv) {
  try {
    ProgramOptionsParser program_options_parser;
    program_options_parser.Parse(argc, argv);
    if (program_options_parser.help_message_printed()) {
      return 0;
    }
  } catch (const exception& except) {
    cerr << "Fatal error occurred: " << except.what() << endl;
    return 1;
  }
  return 0;
}