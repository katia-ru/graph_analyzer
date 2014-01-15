#include <limits>
#include <set>
#include <string>
#include <utility>
#include <vector>


class GraphAnalyzer {
 public:
  void Init(const std::string& graph_filename);

 private:
  const size_t kInfinity_ = std::numeric_limits<int>::max();
  std::vector<std::vector<size_t>> graph_;

 public:
  void FindCycles(const std::string& filename, int maximum_cycle_length) const;

  void FindMaximumPlanarSubgraph(const std::string& filename) const;

  void FindThickness(const std::string& filename) const;

  void FindDiameter(const std::string& filename) const;

  void FindWinerIndex(const std::string& filename) const;

  void FindRandichIndex(const std::string& filename) const;

  void FindAveragePathLength(const std::string& filename) const;

 private:
  void TryToFormCycle(const std::vector<int>& parents,
                      int vertex,
                      std::set<int>* cycle) const noexcept;

  std::vector<int> FormConnectedCycle(int vertex,
                                      int adjacent_vertex,
                                      int cycle_size,
                                      const std::vector<int>& parents)
                                      const noexcept;

  std::set<std::pair<size_t, size_t>> FindMaximumPlanarSubgraph(
      const std::set<std::pair<size_t, size_t>>& edges) const noexcept;

  bool IsPlanar(const std::set<std::pair<size_t, size_t>>& edges)
      const noexcept;

  std::vector<std::vector<size_t>> FormGraph(
      const std::set<std::pair<size_t, size_t>>& edges) const noexcept;

  std::vector<std::vector<size_t>> ComputeAllDistances() const noexcept;
};