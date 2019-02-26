#include "../Graph/Edge.h"
#include "../Graph/Graph.h"
#include <unordered_map>
#include <unordered_set>

#ifndef KTRUSS_V1_PPGRAPH_H
#define KTRUSS_V1_PPGRAPH_H

class PPGraph {
public:
    PPGraph(Graph* publicGraph);
    ~PPGraph();

    Graph* getPublicGraph() const;
    void addPrivateEdge(Edge e);
    void addPrivateEdge(int x, int y);
    const Graph::Neighbors& privateNeighbors(int v) const;
    const Graph::Neighbors& publicNeighbors(int v) const;
    const std::vector<Edge>& getPrivateEdges() const;
    const std::unordered_set<int>& getPrivateNodes() const;
    bool hasPublicEdge(int x, int y) const;
    bool hasPrivateEdge(int x, int y) const;
    bool hasPrivateNode(int x) const;

    friend std::ostream& operator << (std::ostream &out, const PPGraph &ppGraph);
private:
    Graph* publicGraph;
    std::unordered_map<int, Graph::Neighbors> privateNg;
    std::unordered_set<int> privateNodes;
    std::vector<Edge> privateEdges;
    AdjacencyMatrix privateAdjMatrix;
};

#endif //KTRUSS_V1_PPGRAPH_H
