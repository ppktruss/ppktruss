#include "NaivePPKTruss.h"
#include "../KTrussIndex/KTrussDecomposition.h"
#include "../KTrussIndex/KTrussIndex.h"
#include "../Graph/Graph.h"
#include "../KTrussIndex/KTrussQuery.h"
#include <algorithm>

using namespace std;

NaivePPKTruss::NaivePPKTruss(PPGraph *ppGraph)
        : AbstractPPKTruss(ppGraph) {
}

NaivePPKTruss::~NaivePPKTruss() {
}

void NaivePPKTruss::preProcess() {
}

AbstractPPKTruss::KTrussAnswer NaivePPKTruss::query(int v, int k) {
    Graph gNV = createNeighborhoodGraph(v, k);
    KTrussDecomposition kTrussDecomposition(&gNV);
    kTrussDecomposition.createIndex();
    KTrussIndex* index = kTrussDecomposition.getFinalIndex();
    KTrussAnswer answer = KTrussQuery::findKTruss(v, k, index);
    delete index;
    return answer;
}

Graph NaivePPKTruss::createNeighborhoodGraph(int v, int k) {
    // get all neighbors
    vector<int> nodes;
    for (Edge e: ppGraph->privateNeighbors(v)) {
        int u = e.other(v);
        nodes.push_back(u);
    }
    for (Edge e: ppGraph->getPublicGraph()->neighbors(v)) {
        int u = e.other(v);
        nodes.push_back(u);
    }

    // make graph
    Graph gNV = Graph();
    sort(nodes.begin(), nodes.end());
    for (int u: nodes) {
        Graph::Neighbors nU = ppGraph->publicNeighbors(u);

        // Find intersection(N(u), N(v))
        vector<int> triangleNodes;
        if (nodes.size() > nU.size()) {
            for (int j = 0; j < (int) nU.size(); j++) {
                Edge newE = nU[j];
                int w = newE.other(u);
                if (binary_search(nodes.begin(), nodes.end(), w))
                    triangleNodes.push_back(w);
            }
        } else {
            for (int w: nodes)
                if (ppGraph->hasPublicEdge(w, u))
                    triangleNodes.push_back(w);
        }

        // Add edges
        for (int w: triangleNodes)
            gNV.addEdge(u, w);
        gNV.addEdge(v, u);
    }
    return gNV;
}


