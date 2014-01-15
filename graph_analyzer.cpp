#include "./graph_analyzer.h"
#include <boost/algorithm/string.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/graph_traits.hpp>
#include <algorithm>
#include <cmath>
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
using std::getline;
using std::find_if;
using std::ifstream;
using std::isspace;
using std::make_pair;
using std::max;
using std::min;
using std::not1;
using std::ptr_fun;
using std::ofstream;
using std::pair;
using std::queue;
using std::runtime_error;
using std::set;
using std::string;
using std::vector;

void GraphAnalyzer::Init(const string& filename) {
  cout << "Parsing graph from " << filename << "\n";
  ifstream file(filename.data());
  if (!file) {
    throw runtime_error("Failed to read graph from file");
  }
  string line;
  while (!file.eof()) {
    getline(file, line);
    line.erase(find_if(line.rbegin(),
                       line.rend(),
                       not1(ptr_fun<int, int>(isspace))).base(),
               line.end());
    if (!line.empty()) {
      vector<string> tokens;
      split(tokens, line, is_any_of("\t "));
      if (tokens.size() == 2) {
        size_t source_vertex = stoi(tokens[0]);
        size_t destination_vertex = stoi(tokens[1]);
        size_t max_vertex = max(destination_vertex, source_vertex);
        if (graph_.size() < max_vertex + 1) {
          graph_.resize(max_vertex + 1, vector<size_t>());
        }
        graph_[source_vertex].push_back(destination_vertex);
        graph_[destination_vertex].push_back(source_vertex);
      }
    }
  }
  file.close();
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
              cycle.size() <= static_cast<size_t>(maximum_cycle_length)) {
            cycles.insert(cycle);
          }
        } else if (depthes[current_vertex] < maximum_cycle_length) {
          parents[adjacent_vertex] = current_vertex;
          depthes[adjacent_vertex] = depthes[current_vertex] + 1;
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

void GraphAnalyzer::FindMaximumPlanarSubgraph(const string& filename) const {
  cout << "Finding maximum planar subgraph\n";
  set<pair<size_t, size_t>> edges;
  for (size_t vertex = 0; vertex < graph_.size(); ++vertex) {
    for (const auto adjacent_vertex: graph_[vertex]) {
      edges.insert(make_pair(min(vertex, adjacent_vertex),
                             max(vertex, adjacent_vertex)));
    }
  }
  set<pair<size_t, size_t>> maximum_planar_subgraph_edges =
      FindMaximumPlanarSubgraph(edges);
  vector<vector<size_t>> maximum_planar_subgraph = FormGraph(
      maximum_planar_subgraph_edges);
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

void GraphAnalyzer::FindThickness(const string& filename) const {
  cout << "Finding thickness\n";
  set<pair<size_t, size_t>> thickness_edges;
  set<pair<size_t, size_t>> edges;
  for (size_t vertex = 0; vertex < graph_.size(); ++vertex) {
    for (const auto adjacent_vertex: graph_[vertex]) {
      edges.insert(make_pair(min(vertex, adjacent_vertex),
                             max(vertex, adjacent_vertex)));
    }
  }
  while (edges.size() > 0) {
    set<pair<size_t, size_t>> maximum_planar_subgraph_edges =
        FindMaximumPlanarSubgraph(edges);
    for (const auto& edge: maximum_planar_subgraph_edges) {
      edges.erase(edge);
    }
    thickness_edges.insert(maximum_planar_subgraph_edges.begin(),
                           maximum_planar_subgraph_edges.end());
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

void GraphAnalyzer::FindDiameter(const string& filename) const {
  cout << "Finding diameter\n";
  vector<vector<size_t>> distances = ComputeAllDistances();
  size_t diameter = 0;
  for (const auto& adjacency_list: distances) {
    for (const auto distance: adjacency_list) {
      if (distance < kInfinity_ && distance > diameter) {
        diameter = distance;
      }
    }
  }
  ofstream file(filename.data());
  if (!file) {
    throw runtime_error("Failed to write diameter");
  }
  file << diameter << "\n";
  file.close();
}

void GraphAnalyzer::FindAveragePathLength(const string& filename) const {
  cout << "Finding average path length\n";
  vector<vector<size_t>> distances = ComputeAllDistances();
  size_t average_path_length = 0.0;
  for (const auto& adjacency_list: distances) {
    for (const auto& distance: adjacency_list) {
      if (distance < kInfinity_) {
        average_path_length += distance;
      }
    }
  }
  ofstream file(filename.data());
  if (!file) {
    throw runtime_error("Failed to write average path length");
  }
  file << static_cast<double>(average_path_length) /
      (static_cast<double>(graph_.size()) *
       static_cast<double>(graph_.size() - 1))  << "\n";
  file.close();
}

void GraphAnalyzer::FindWinerIndex(const string& filename) const {
  cout << "Finding Winer index\n";
  vector<vector<size_t>> distances = ComputeAllDistances();
  size_t winer_index = 0;
  for (const auto& adjacency_list: distances) {
    for (const auto distance: adjacency_list) {
      if (distance < kInfinity_) {
        winer_index += distance;
      }
    }
  }
  ofstream file(filename.data());
  if (!file) {
    throw runtime_error("Failed to write Winer index");
  }
  file << winer_index << "\n";
  file.close();
}

void GraphAnalyzer::FindRandichIndex(const string& filename) const {
  cout << "Finding Randich index\n";
  double randich_index = 0.0;
  for (size_t vertex = 0; vertex < graph_.size(); ++vertex) {
    for (const auto adjacent_vertex: graph_[vertex]) {
      randich_index += 1.0 / sqrt(static_cast<double>(graph_[vertex].size())) *
          1.0 / sqrt(static_cast<double>(graph_[adjacent_vertex].size()));
    }
  }
  ofstream file(filename.data());
  if (!file) {
    throw runtime_error("Failed to write Randich index");
  }
  file << randich_index << "\n";
  file.close();
}

void GraphAnalyzer::TryToFormCycle(const vector<int>& parents,
                                   int vertex,
                                   set<int>* cycle) const noexcept {
  while (vertex != -1) {
    auto result = cycle->insert(vertex);
    vertex = parents[vertex];
    if (!result.second && vertex != -1) {
      cycle->clear();
      return;
    }
  }
}

set<pair<size_t, size_t>> GraphAnalyzer::FindMaximumPlanarSubgraph(
    const set<pair<size_t, size_t>>& edges) const noexcept {
  set<pair<size_t, size_t>> current_edges(edges.begin(), edges.end());
  set<pair<size_t, size_t>> maximum_planar_subgraph_edges;
  while (current_edges.size() > 0) {
    pair<size_t, size_t> edge = *current_edges.begin();
    maximum_planar_subgraph_edges.insert(edge);
    current_edges.erase(current_edges.begin());
    if (!IsPlanar(maximum_planar_subgraph_edges)) {
      maximum_planar_subgraph_edges.erase(edge);
    }
  }
  return maximum_planar_subgraph_edges;
}

bool GraphAnalyzer::IsPlanar(const set<pair<size_t, size_t>>& edges)
    const noexcept {
  typedef adjacency_list<vecS,
                         vecS,
                         undirectedS,
                         property<vertex_index_t, size_t>> Graph;
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

vector<vector<size_t>> GraphAnalyzer::ComputeAllDistances() const noexcept {
  vector<vector<size_t>> distances(graph_.size(),
                                   vector<size_t>(graph_.size(), kInfinity_));
  for (size_t vertex = 0; vertex < graph_.size(); ++vertex) {
    for (const auto adjacent_vertex: graph_[vertex]) {
      distances[vertex][adjacent_vertex] = 1;
    }
  }
  for (size_t vertex = 0; vertex < graph_.size(); ++vertex) {
    distances[vertex][vertex] = 0;
  }
  for (size_t aux_vertex = 0; aux_vertex < graph_.size(); ++aux_vertex) {
    for (size_t src_vertex = 0; src_vertex < graph_.size(); ++src_vertex) {
      for (size_t dest_vertex = 0; dest_vertex < graph_.size(); ++dest_vertex) {
        unsigned long long new_distance = distances[src_vertex][aux_vertex] +
            distances[aux_vertex][dest_vertex];
        if (distances[src_vertex][dest_vertex] > new_distance) {
          distances[src_vertex][dest_vertex] =
              static_cast<size_t>(new_distance);
        }
      }
    }
  }
  return distances;
}
