#include "./graph_analyzer.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/graph_traits.hpp>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace boost;
using std::cout;
using std::ifstream;
using std::make_pair;
using std::ofstream;
using std::pair;
using std::queue;
using std::runtime_error;
using std::set;
using std::string;
using std::vector;

void GraphAnalyzer::Init(const string& filename) {
  ifstream file(filename.data());
  if (!file) {
    throw runtime_error("Failed to read graph from file");
  }
  size_t number_vertices;
  size_t number_edges;
  file >> number_vertices >> number_edges;
  graph_.resize(number_vertices, vector<size_t>());
  for (size_t edge = 0; edge < number_edges; ++edge) {
    size_t source_vertex;
    size_t destination_vertex;
    file >> source_vertex >> destination_vertex;
    graph_[source_vertex].push_back(destination_vertex);
    graph_[destination_vertex].push_back(source_vertex);
  }
  file.close();
}

void GraphAnalyzer::Analyze(
    const string& cycles_filename,
    int maximum_cycle_length,
    const string& maximum_planar_subgraph_filename,
    const string& maximum_outerplanar_subgraph_filename,
    const string& thickness_filename,
    const string& outerthickness_filename) const {
  if (!cycles_filename.empty() && maximum_cycle_length > 2) {
    FindCycles(cycles_filename, maximum_cycle_length);
  }
  if (!maximum_planar_subgraph_filename.empty()) {
    FindMaximumPlanarSubgraph(maximum_planar_subgraph_filename);
  }
  if (!maximum_outerplanar_subgraph_filename.empty()) {
    FindMaximumOuterPlanarSubgraph(maximum_outerplanar_subgraph_filename);
  }
  if (!thickness_filename.empty()) {
    FindThickness(thickness_filename);
  }
  if (!outerthickness_filename.empty()) {
    FindOuterThickness(outerthickness_filename);
  }
}

void GraphAnalyzer::FindCycles(const string& filename,
                               int maximum_cycle_length) const {
  cout << "Finding cycles\n";
  set<set<int>> cycles;
  for (size_t vertex = 0; vertex < graph_.size(); ++vertex) {
    vector<int> depthes(graph_.size(), 0);
    vector<int> parents(graph_.size(), -1);
    queue<int> vertices_queue;
    vertices_queue.push(vertex);
    depthes[vertex] = 1;
    while (!vertices_queue.empty()) {
      int current_vertex = vertices_queue.front();
      vertices_queue.pop();
      for (const auto adjacent_vertex: graph_[current_vertex]) {
        if (depthes[adjacent_vertex] > 0) {
          set<int> cycle;
          TryToFormCycle(parents, adjacent_vertex, &cycle);
          TryToFormCycle(parents, current_vertex, &cycle);
          if (cycle.size() > 2 &&
              cycle.size() < static_cast<size_t>(maximum_cycle_length)) {
            cycles.insert(cycle);
          }
        } else if (depthes[current_vertex] < maximum_cycle_length) {
          parents[adjacent_vertex] = current_vertex;
          vertices_queue.push(adjacent_vertex);
        }
      }
    }
  }
  ofstream file(filename.data());
  if (!file) {
    throw runtime_error("Failed to write cycles");
  }
  for (const auto& cycle: cycles) {
    for (const auto vertex: cycle) {
      file << vertex << "\t";
    }
    file << "\n";
  }
  file.close();
}

void GraphAnalyzer::TryToFormCycle(const vector<int>& parents,
                                   int vertex,
                                   set<int>* cycle) const noexcept {
  while (vertex != -1) {
    auto result = cycle->insert(vertex);
    if (!result.second) {
      break;
    }
    vertex = parents[vertex];
  }
}

void GraphAnalyzer::FindMaximumPlanarSubgraph(const string& filename) const {
  set<pair<size_t, size_t>> forming_edges;
  set<pair<size_t, size_t>> edges;
  for (size_t vertex = 0; vertex < graph_.size(); ++vertex) {
    for (const auto adjacent_vertex: graph_[vertex]) {
      edges.insert(make_pair(vertex, adjacent_vertex));
    }
  }
  while (edges.size() > 0) {
    pair<size_t, size_t> edge = *edges.begin();
    forming_edges.insert(edge);
    edges.erase(edges.begin());
    if (!IsPlanar(forming_edges)) {
      forming_edges.erase(edge);
    }
  }
  vector<vector<size_t>> maximum_planar_subgraph = FormGraph(forming_edges);
  ofstream file(filename.data());
  if (!file) {
    throw runtime_error("Failed to write found maximum planar subgraph");
  }
  for (size_t vertex = 0; vertex < maximum_planar_subgraph.size(); ++vertex) {
    for (const auto adjacent_vertex: maximum_planar_subgraph[vertex]) {
      file << vertex << "\t" << adjacent_vertex << "\n";
    }
  }
  file.close();
}

bool GraphAnalyzer::IsPlanar(const set<pair<size_t, size_t>>& edges)
    const noexcept {
  typedef adjacency_list<vecS, vecS, bidirectionalS> Graph;
  Graph testing_graph(graph_.size());
  for (const auto& edge: edges) {
    add_edge(edge.first, edge.second, testing_graph);
  }
  return boyer_myrvold_planarity_test(testing_graph);
}

vector<vector<size_t>> GraphAnalyzer::FormGraph(
    const set<pair<size_t, size_t>>& edges) const noexcept {
  vector<vector<size_t>> forming_graph(graph_.size(), vector<size_t>());
  for (const auto& edge: edges) {
    forming_graph[edge.first].push_back(edge.second);
  }
  return forming_graph;
}

void GraphAnalyzer::FindMaximumOuterPlanarSubgraph(const string& filename)
    const {
  ofstream file(filename.data());
  if (!file) {
    throw runtime_error("Failed to write maximum outerplanar subgraph");
  }
  file.close();
}

void GraphAnalyzer::FindThickness(const string& filename) const {
  cout << "Finding thickness\n";
  set<pair<size_t, size_t>> thickness_edges;
  set<pair<size_t, size_t>> edges;
  for (size_t vertex = 0; vertex < graph_.size(); ++vertex) {
    for (const auto adjacent_vertex: graph_[vertex]) {
      edges.insert(make_pair(vertex, adjacent_vertex));
    }
  }
  while (edges.size() > 0) {
    set<pair<size_t, size_t>> current_edges(edges.begin(), edges.end());
    set<pair<size_t, size_t>> forming_edges;
    while (current_edges.size() > 0) {
      pair<size_t, size_t> edge = *current_edges.begin();
      forming_edges.insert(edge);
      current_edges.erase(current_edges.begin());
      if (!IsPlanar(forming_edges)) {
        forming_edges.erase(edge);
      }
    }
    for (const auto& edge: forming_edges) {
      edges.erase(edge);
    }
    thickness_edges.insert(forming_edges.begin(), forming_edges.end());
  }
  vector<vector<size_t>> thickness = FormGraph(thickness_edges);
  ofstream file(filename.data());
  if (!file) {
    throw runtime_error("Failed to write thickness of graph");
  }
  for (size_t vertex = 0; vertex < thickness.size(); ++vertex) {
    for (const auto& adjacent_vertex: thickness[vertex]) {
      file << vertex << "\t" << adjacent_vertex << "\n";
    }
  }
  file.close();
}

void GraphAnalyzer::FindOuterThickness(const string& filename) const {
  ofstream file(filename.data());
  if (!file) {
    throw runtime_error("Failed to write outerthickness");
  }
  file.close();
}