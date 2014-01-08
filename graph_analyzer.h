#include <set>
#include <string>
#include <utility>
#include <vector>


class GraphAnalyzer {
 public:
  void Init(const std::string& graph_filename);

 private:
  std::vector<std::vector<size_t>> graph_;

 public:
  void FindCycles(const std::string& filename, int maximum_cycle_length) const;

  void FindMaximumPlanarSubgraph(const std::string& filename) const;

  void FindThickness(const std::string& filename) const;

  void FindDiameter(const std::string& filename) const;

 private:
  void TryToFormCycle(const std::vector<int>& parents,
                      int vertex,
                      std::set<int>* cycle) const noexcept;

  bool IsPlanar(const std::set<std::pair<size_t, size_t>>& edges)
      const noexcept;

  std::vector<std::vector<size_t>> FormGraph(
      const std::set<std::pair<size_t, size_t>>& edges) const noexcept;

  std::vector<std::vector<size_t>> ComputeAllDistances() const noexcept;
};