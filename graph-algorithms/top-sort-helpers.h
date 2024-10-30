#pragma once

#include <unordered_map>

#include "weighted-graph.hpp"
#include "graph-types.h"

template <typename T>
void computeIndegrees(const WeightedGraph<T>& graph, std::unordered_map<value_type<T>, int>& indegrees) {
    // TODO store the indegree for each vertex in the graph in the indegrees map
    //we can get each vertex of the graph by traversing thru our graph using an iterator
    //each time, we'll want to instantiate the vertex's val in our indegree map as 0
    //we then want to traverse thru the adjacency list for each of our vertices
    //for each vertex, in the adj list, increment it's count in indegree


    for(auto curr = graph.begin(); curr != graph.end(); curr++) {
        indegrees[curr->first] = 0;
    }

    for(auto curr = graph.begin(); curr != graph.end(); curr++) {
        for(auto [vertex, weight] : graph.at(curr->first)) {
            indegrees[vertex] += 1;
        }
    }


}
