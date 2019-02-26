#include <vector>
#include <unordered_set>
#include "../Graph/Graph.h"
#include "../Graph/Edge.h"
#include "PPGraph.h"

using namespace std;

PPGraph::PPGraph(Graph* graph) {
    this->publicGraph = graph;
};

PPGraph::~PPGraph() {
}

void PPGraph::addPrivateEdge(Edge e) {
    if (publicGraph->hasEdge(e))
        return;
    int u = e.getX(), v = e.getY();
    privateNg[u].push_back(e);
    privateNg[v].push_back(e);
    privateNodes.insert(u);
    privateNodes.insert(v);
    privateEdges.push_back(e);
    privateAdjMatrix.addEdge(u, v);
}

void PPGraph::addPrivateEdge(int x, int y) {
    addPrivateEdge(Edge(x, y));
}

const Graph::Neighbors& PPGraph::privateNeighbors(int v) const {
    return privateNg.at(v);
}

const Graph::Neighbors& PPGraph::publicNeighbors(int v) const {
    return publicGraph->neighbors(v);
}

bool PPGraph::hasPublicEdge(int x, int y) const {
    return publicGraph->hasEdge(x, y);
}

bool PPGraph::hasPrivateEdge(int x, int y) const {
    return privateAdjMatrix.hasEdge(x, y);
}

const unordered_set<int>& PPGraph::getPrivateNodes() const {
    return privateNodes;
}

Graph* PPGraph::getPublicGraph() const {
    return publicGraph;
}

const vector<Edge>& PPGraph::getPrivateEdges() const {
    return privateEdges;
}

bool PPGraph::hasPrivateNode(int x) const {
    return privateNodes.count(x);
}

std::ostream& operator << (std::ostream &out, const PPGraph &ppGraph) {
    out << "PPG(\n Public Graph:" << *ppGraph.getPublicGraph() << ")\n Private Edges: [\n";
    for (auto e: ppGraph.getPrivateEdges())
        out << e << ",\n";
    out << "]";
    return out;
}
