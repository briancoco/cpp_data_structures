#pragma once

#include <istream>
#include <ostream>
#include <sstream>
#include <limits>
#include <list>
#include <queue>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "weighted-graph.hpp"

#include "graph-types.h"

// if the arrow is a box, change to the other line
#define ARROW_SEPARATOR " \u2192 "
// #define ARROW_SEPARATOR " -> "

// contains initializeSingleSource, relax, & updateHeap as well as the DijkstraComaparator
#include "dijkstras-helpers.h"

/**
 * @brief Dijkstra's Algorithm - https://canvas.tamu.edu/courses/136654/files/35930572/preview Slide 16
 *
 * @tparam T type of data stored by a vertex
 * @param graph weighted, directed graph to find single-source shortest-path
 * @param initial_node source node in graph for shortest path
 * @param destination_node destination node in graph for shortest path
 * @return std::list<value_type<T>> list of nodes along shortest path including initial_node and destination_node, empty if no path exists
 */
template <typename T>
std::list<value_type<T>> dijkstrasAlgorithm(const WeightedGraph<T>& graph, vertex_type<T> initial_node, vertex_type<T> destination_node)
{
    std::unordered_map<value_type<T>, weight_type<T>> distances;
    std::unordered_map<value_type<T>, std::optional<value_type<T>>> predecessors;
    std::unordered_set<value_type<T>> s;
    std::priority_queue<value_type<T>, std::vector<value_type<T>>, DijkstraComparator<T>> q(DijkstraComparator<T>{distances});

    
    //first initialize our predecessors map and distances map using the helper func initializeSingleSource
    //while our priority queue is not empty, get the top of the queue
    //make sure vertex is not visited, if yes the skip
    //if no, then add to visited and continue algo
    //traverse thru vertex's adj list
    //for each vertex, call relax fn which will redefine each adj vertice's distance and predecessor if new min
    //found
    //if we did find new mins, then we also want to update our heap after
    //which we can do using the updateHeap() fn

    initializeSingleSource<T>(graph, initial_node, distances, predecessors);
    for(auto [vertex, adjList] : graph) {
        q.push(vertex);
    }
    
    while(!q.empty()) {
        auto vertex = q.top();

        //why?? dont understand
        //I think when we reach this case, that means we have gotten all possible
        //shortest paths from source to other vertices
        if(distances[vertex] == infinity<T>()) {
            break;
        }
        q.pop();

        //how come we dont do anything if we have visited the node
        //this is because we will never visit the same node twice in our priority queue
        //since our priority queue starts with all nodes and we pop each iteration of our loop
        //we simply used the visited set to denote which vertices we've already found the best
        //distance for, so we do not traverse and perform any unneccesary relax computations
        if(s.find(vertex) == s.end()) {
            s.insert(vertex);
        }
        
        for(auto [adjVertex, weight] : graph.at(vertex)) {

            if(s.find(adjVertex) != s.end()) {
                continue;
            }
            if(relax<T>(vertex, adjVertex, weight, distances, predecessors)) {
                updateHeap<T>(q, distances);
            }
        }
        
    }


    //TODO create list by walking backwards through predecessors from the end
    //traverse thru our predecessor array starting at destination node
    //to find shortest path fron origin to dest
    //each time add to the front of the list
    std::list<value_type<T>> res;
    value_type<T> curr = destination_node;

    if(initial_node == destination_node || predecessors[curr].has_value())
        res.push_back(destination_node);
    
    while(predecessors[curr].has_value()) {
        res.push_front(predecessors[curr].value());
        curr = predecessors[curr].value();
    }


    return res;
}

#include "top-sort-helpers.h"

/**
 * @brief Returns a Topological Ordering of the Graph - https://en.wikipedia.org/wiki/Topological_sorting#Depth-first_search
 *
 * @tparam T type of data stored by a vertex
 * @param graph graph upon which to perform a topological ordering
 * @return std::list<value_type<T>> list of nodes in a topological order, or an empty list if no such ordering exists
 */
template <typename T>
std::list<value_type<T>> topologicalSort(const WeightedGraph<T>& graph)
{
    std::unordered_map<value_type<T>, int> indegrees;
    std::unordered_map<value_type<T>, int> topological_numbers;
    std::list<value_type<T>> res;
    computeIndegrees(graph, indegrees);
    //where we start at a source node and traverse thru the graph until we have visited all nodes
    //our starting point (source node) is the first node we find with an indegree of 0

    //after we find our starting point, we'll want to use a queue data structure to traverse thru the graph
    //each time we'll pop from the queue, add the node we popped to the our visited set
    //and find all outgoing edges for the current node, adding them to the queue
    //if they have not been visited

    //we'll continue this until our queue is empty, then we'll return the resulting list
    std::queue<value_type<T>> q;
    for(auto curr = indegrees.begin(); curr != indegrees.end(); curr++) {
        if(curr->second == 0) {
            q.push(curr->first);
        }
    }

    while(q.size() > 0) {
        value_type<T> vertex = q.front();
        res.push_back(vertex);
        q.pop();

        for(auto [adjVertex, weight] : graph.at(vertex)) {
            indegrees[adjVertex] -= 1;
            if(indegrees[adjVertex] == 0) {
                q.push(adjVertex);
            }
        }

    }

    if(res.size() != graph.size()) {
        res.clear();
    }

    return res;



}

template <typename T>
std::ostream &operator<<(std::ostream &o, const WeightedGraph<T> &graph)
{
    for (auto it = graph.begin(); it != graph.end(); ++it)
    {
        const vertex_type<T> &vertex = it->first;
        const adjacency_list<T> &list = it->second;
        o << vertex << ": ";
        for (auto iit = list.begin(); iit != list.end(); ++iit)
        {
            const vertex_type<T> &destination = iit->first;
            const weight_type<T> &weight = iit->second;
            o << destination << "(" << weight << ')';
            auto next_iter = iit;
            if (++next_iter != list.end())
            {
                o << ARROW_SEPARATOR;
            }
        }
        auto next_iter = it;
        if (++next_iter != graph.end())
        {
            o << "\n";
        }
    }
    return o;
}

template <typename T>
std::istream &readEdge(std::istream &i, value_type<T> &vertex, weight_type<T> &weight)
{
    std::string s_vertex, s_weight;
    std::getline(i, s_vertex, '(');
    std::getline(i, s_weight, ')');
    std::istringstream stream(s_vertex + " " + s_weight);
    stream >> vertex >> weight;
    if (stream.fail())
    {
        i.setf(stream.flags());
    }
    return i;
}

template <typename T>
std::istream &operator>>(std::istream &i, WeightedGraph<T> &graph)
{
    std::string line;
    while (std::getline(i, line))
    {
        if (line.empty())
            break;
        std::istringstream line_stream(line);
        value_type<T> vertex;
        std::string s_vertex;
        std::getline(line_stream, s_vertex, ':');
        std::istringstream stream_vertex(s_vertex);
        stream_vertex >> vertex;
        if (stream_vertex.fail())
        {
            i.setf(stream_vertex.flags());
            break;
        }
        graph.push_vertex(vertex);
        std::string separator;
        value_type<T> edge_end;
        weight_type<T> edge_weight;
        while (readEdge<T>(line_stream, edge_end, edge_weight))
        {
            line_stream >> separator;
            graph.push_edge(vertex, edge_end, edge_weight);
        }
    }

    if(i.eof() and i.fail())
        i.clear(std::ios::eofbit);
    
    return i;
}
