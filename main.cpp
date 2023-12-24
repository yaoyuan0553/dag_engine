import utility;
import graph;

import <iostream>;
import <iomanip>;
import <string>;
import <random>;

void print_as_dot_graph(const std::vector<yy::Edge>& edge_list) {
    std::cout << "digraph {\n";
    for (auto edge : edge_list) {
        std::cout << "  " << std::quoted(std::to_string(edge.from)) << " -> "
                  << std::quoted(std::to_string(edge.to)) << ";\n";
    }
    std::cout << "}\n";
}

void test_dag_generation(int num_nodes, int loops) {
    yy::TopologicalSorter top_sorter;
    for (int i = 0; i < loops; ++i) {
        auto edge_list = yy::generate_random_dag(num_nodes);
        if (!top_sorter.can_sort(num_nodes, edge_list)) {
            std::cout << "******** failed ********\n";
            print_as_dot_graph(edge_list);
            return;
        }
    }
}

int main() {
    static constexpr int NUM_NODES = 10;

    auto edge_list = yy::generate_random_dag<false, 57>(NUM_NODES);
//    auto rng = std::default_random_engine{};
//    std::shuffle(edge_list.begin(), edge_list.end(), rng);

    print_as_dot_graph(edge_list);
    yy::TopologicalSorter top_sorter;
    auto sorted = top_sorter(NUM_NODES, edge_list);
    if (sorted) {
        std::cout << *sorted << '\n';
    }

    return 0;
}
