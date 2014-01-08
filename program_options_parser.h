#include <string>

class ProgramOptionsParser {
 public:
  void Parse(int argc, char** argv);

 private:
  bool help_message_printed_ = false;
  std::string input_filename_ = "";
  std::string cycles_filename_ = "";
  int maximum_cycle_length_ = 7;
  std::string maximum_planar_subgraph_filename_ = "";
  std::string maximum_outerplanar_subgraph_filename_ = "";
  std::string thickness_filename_ = "";
  std::string outerthickness_filename_ = "";

 public:
  bool help_message_printed() const noexcept {
    return help_message_printed_;
  }

  std::string input_filename() const noexcept {
    return input_filename_;
  }

  std::string cycles_filename() const noexcept {
    return cycles_filename_;
  }

  int maximum_cycle_length() const noexcept {
    return maximum_cycle_length_;
  }

  std::string maximum_planar_subgraph_filename() const noexcept {
    return maximum_planar_subgraph_filename_;
  }

  std::string maximum_outerplanar_subgraph_filename() const noexcept {
    return maximum_outerplanar_subgraph_filename_;
  }

  std::string thickness_filename() const noexcept {
    return thickness_filename_;
  }

  std::string outerthickness_filename() const noexcept {
    return outerthickness_filename_;
  }
};