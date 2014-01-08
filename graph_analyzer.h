#include <set>
#include <string>
#include <utility>
#include <vector>


class GraphAnalyzer {
 public:
  void Init(const std::string& graph_filename);

  void Analyze(const std::string& cycles_filename,
               int maximum_cycle_length,
               const std::string& maximum_planar_subgraph_filename,
               const std::string& maximum_outerplanar_subgraph_filename,
               const std::string& thickness_filename,
               const std::string& outerthickness_filename) const;

 private:
  std::vector<std::vector<size_t>> graph_;

 private:
  void FindCycles(const std::string& filename, int maximum_cycle_length) const;

  void TryToFormCycle(const std::vector<int>& parents,
                      int vertex,
                      std::set<int>* cycle) const noexcept;

  void FindMaximumPlanarSubgraph(const std::string& filename) const;

  bool IsPlanar(const std::set<std::pair<size_t, size_t>>& edges)
      const noexcept;

  std::vector<std::vector<size_t>> FormGraph(
      const std::set<std::pair<size_t, size_t>>& edges) const noexcept;

  void FindMaximumOuterPlanarSubgraph(const std::string& filename) const;

  void FindThickness(const std::string& filename) const;

  void FindOuterThickness(const std::string& filename) const;
};