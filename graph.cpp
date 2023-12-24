export module graph;

import <iostream>;
import <random>;
import <algorithm>;

export import <vector>;
export import <optional>;

import utility;

export
namespace yy
{

struct Edge {
    int from;
    int to;
};

template <bool RandomSeed = true, int Seed = 57>
std::vector <Edge> generate_random_dag(int total_nodes)
{
    std::vector <Edge> out;

    std::mt19937 mt;
    if constexpr (RandomSeed) {
        std::random_device rd;
        mt.seed(rd());
    } else {
        mt.seed(Seed);
    }

    std::vector<int> node_mapping(total_nodes);
    std::iota(node_mapping.begin(), node_mapping.end(), 0);
    std::shuffle(node_mapping.begin(), node_mapping.end(), mt);

#ifdef DAG_DEBUG
    std::cout << "node_mapping: " << node_mapping << '\n';
#endif

    /// \see https://mathematica.stackexchange.com/questions/608/how-to-generate-random-directed-acyclic-graphs
    /// "the graph is acyclic if and only if there exists a vertex ordering
    /// which makes the adjacency matrix lower triangular"

    for (int j = 1; j < total_nodes; ++j) {
        // 0: 0
        // 1: 0 0
        // 2: 0 0 0
        int max_node = 1 << j;
        std::uniform_int_distribution<int> gen_connections(0, max_node);
        int edge_dist = gen_connections(mt);
        for (int i = 0; i < j; ++i) {
            if ((edge_dist >> i) & 1) {
                out.emplace_back(node_mapping[i], node_mapping[j]);
//                out.emplace_back(i, j);
            }
        }
    }

    return out;
}

class DirectedAcyclicGraph {
public:
    DirectedAcyclicGraph(int num_nodes, const std::vector <Edge>& edge_list) : dependency_graph_(num_nodes)
    {
        for (auto& edge: edge_list)
            dependency_graph_[edge.to].push_back(edge.from);
    }

    const std::vector<int>* get_dependency_list(int dependent)
    {
        if (dependent >= dependency_graph_.size())
            return nullptr;
        return &dependency_graph_[dependent];
    }

private:
    std::vector <std::vector<int>> dependency_graph_;
};

class TopologicalSorter {
    enum Status {
        kUnknown = 0,
        kVisiting,
        kVisited,
    };

    bool dfs(int i, const std::vector <std::vector<int>>& graph, std::vector <Status>& record) const
    {
        if (record[i] == kVisiting)
            return false;
        if (record[i] == kVisited)
            return true;

        record[i] = kVisiting;

        for (int dep: graph[i]) {
            if (!dfs(dep, graph, record))
                return false;
        }

        record[i] = kVisited;
        return true;
    }

    bool dfs(int i, const std::vector <std::vector<int>>& graph, std::vector <Status>& record,
             std::vector<int>& sorted) const
    {
        if (record[i] == kVisiting)
            return false;
        if (record[i] == kVisited)
            return true;

        record[i] = kVisiting;

        for (int dep: graph[i]) {
            if (!dfs(dep, graph, record, sorted))
                return false;
        }

        sorted.push_back(i);
        record[i] = kVisited;
        return true;
    }

public:
    bool can_sort(int num_nodes, const std::vector <Edge>& edge_list) const
    {
        // dependency graph
        std::vector <std::vector<int>> graph(num_nodes);
        for (auto& edge: edge_list)
            graph[edge.to].push_back(edge.from);

        std::vector <Status> record(num_nodes, kUnknown);

        for (int i = 0; i < num_nodes; ++i) {
            if (!dfs(i, graph, record))
                return false;
        }
        return true;
    }

    std::optional <std::vector<int>> operator()(int num_nodes, const std::vector <Edge>& edge_list) const
    {
        std::vector <std::vector<int>> graph(num_nodes);
        for (auto& edge: edge_list)
            graph[edge.to].push_back(edge.from);

#ifdef DAG_DEBUG
        std::cout << "graph: " << graph << '\n';
#endif

        std::vector <Status> record(num_nodes, kUnknown);
        std::vector<int> order;
        order.reserve(num_nodes);

        for (int i = 0; i < num_nodes; ++i) {
            if (!dfs(i, graph, record, order))
                return std::nullopt;
        }
        return order;
    }
};

}

namespace yy
{

template <>
struct ToString<Edge> {
    std::string operator()(const Edge& edge) const && {
        return std::to_string(edge.from) + " -> " + std::to_string(edge.to);
    }
};

}