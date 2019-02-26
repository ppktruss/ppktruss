#include "NeighborhoodIndex.h"
#include "../KTrussIndex/KTrussIndex.h"
#include "../Graph/Graph.h"
#include "../KTrussIndex/KTrussDecomposition.h"
#include "../Graph/GraphUtility.h"
#include <vector>

using namespace std;

NeighborhoodIndex::NeighborhoodIndex(PPGraph *ppGraph) {
    this->graph = ppGraph;
}

NeighborhoodIndex::~NeighborhoodIndex() {
    for (auto x: publicIndex) {
        KTrussIndex *index = x.second;
        delete index->getGraph();
        delete index;
    }
}

KTrussIndex* NeighborhoodIndex::getPublicIndex(int v) const {
    if (publicIndex.find(v) != publicIndex.end())
        return publicIndex.at(v);
    return nullptr;
}

void NeighborhoodIndex::calcPublicIndex(int v) {
    vector<int> nodes;
    for (Edge e: graph->getPublicGraph()->neighbors(v)) {
        int u = e.other(v);
        nodes.push_back(u);
    }
    Graph *g = GraphUtility::getInducedSubGraph(graph->getPublicGraph(), nodes);
    KTrussDecomposition decomposition(g);
    decomposition.createIndex();
    KTrussIndex *index = decomposition.getFinalIndex();
    publicIndex[v] = index;
}