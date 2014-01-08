#include "./program_options_parser.h"
#include "./graph_analyzer.h"
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
    GraphAnalyzer graph_analyzer;
    graph_analyzer.Init(program_options_parser.input_filename());
    graph_analyzer.Analyze(
        program_options_parser.cycles_filename(),
        program_options_parser.maximum_cycle_length(),
        program_options_parser.maximum_planar_subgraph_filename(),
        program_options_parser.maximum_outerplanar_subgraph_filename(),
        program_options_parser.thickness_filename(),
        program_options_parser.outerthickness_filename());
  } catch (const exception& except) {
    cerr << "Fatal error occurred: " << except.what() << endl;
    return 1;
  }
  return 0;
}