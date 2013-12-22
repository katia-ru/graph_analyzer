#include "./program_options_parser.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace boost::program_options;
using std::cout;
using std::runtime_error;
using std::string;
using std::vector;

void ProgramOptionsParser::Parse(int argc, char** argv) {
  variables_map program_options_storage;
  string configuration_filename = "graph_analyzer.conf";
  options_description program_options_description("Options for program:");
  program_options_description.add_options()
      ("config_filename, c", value<string>(&configuration_filename),
          "Configuration file")
      ("help,h", value<bool>(&help_message_printed_)->implicit_value(true),
          "Print help message and exit")
      ("input_filename", value<string>(&input_filename_),
          "File with input data")
      ("cycles_filename", value<string>(&cycles_filename_),
          "File for writing found cycles")
      ("maximum_cycle_length", value<int>(&maximum_cycle_length_),
          "Maximum length of cycles to detect")
      ("maximum_planar_subgraph_filename",
          value<string>(&maximum_planar_subgraph_filename_),
          "File where maximum planar subgraph should be written")
      ("maximum_outerplanar_subgraph_filename",
          value<string>(&maximum_outerplanar_subgraph_filename_),
          "File where maximum outerplanar subgraph should be written")
      ("thickness_filename", value<string>(&thickness_filename_),
          "File where thickness value should be written")
      ("outerthickness_filename", value<string>(&outerthickness_filename_),
          "File where outerthickness should be written");
  store(parse_command_line(argc, argv, program_options_description),
        program_options_storage);
  notify(program_options_storage);
  if (!configuration_filename.empty()) {
    store(parse_config_file<char>(configuration_filename.data(),
                                  program_options_description),
          program_options_storage);
    notify(program_options_storage);
  }
  if (help_message_printed_) {
    cout << program_options_description << "\n";
  }
  if (!cycles_filename_.empty() && maximum_cycle_length_ <= 2) {
    throw runtime_error("Wrong maximum length of cycle to find");
  }
}