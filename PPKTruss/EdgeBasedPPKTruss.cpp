#include "EdgeBasedPPKTruss.h"
#include "../Graph/Graph.h"
#include "../KTrussUpdate/KTrussUpdate.h"
#include "../KTrussIndex/KTrussQuery.h"

using namespace std;

EdgeBasedPPKTruss::EdgeBasedPPKTruss(PPGraph *ppGraph) {
    this->graph = ppGraph;
    neighborhoodIndex = new NeighborhoodIndex(ppGraph);
}

EdgeBasedPPKTruss::~EdgeBasedPPKTruss() {
    delete neighborhoodIndex;
}

void EdgeBasedPPKTruss::preProcess() {
    // TODO
}

void EdgeBasedPPKTruss::preProcessForNode(int v) {
    neighborhoodIndex->calcPublicIndex(v);
}

AbstractPPKTruss::KTrussAnswer EdgeBasedPPKTruss::query(int v, int k) {
    return query(v, k, true);
}

AbstractPPKTruss::KTrussAnswer EdgeBasedPPKTruss::query(int v, int k, bool withPreProcess) {
    // Copy public index from pre-process
    if (withPreProcess)
        preProcessForNode(v);

    KTrussIndex *preIndex = neighborhoodIndex->getPublicIndex(v);
    if (preIndex == nullptr)
        throw ;
    Graph* vGraph = new Graph(*preIndex->getGraph());
    KTrussIndex *index = new KTrussIndex(vGraph);
    for (Edge e: preIndex->getGraph()->getEdges())
        index->setTruss(e, preIndex->getTruss(e));

    // Gather new edges
    vector<Edge> edgesToAdd;
    for (Edge e: this->graph->privateNeighbors(v)) {
        if (vGraph->hasEdge(e))
            continue;

        int u = e.other(v);
        vGraph->addNode(u);
        for (Edge eu: this->graph->publicNeighbors(u)) {
            int w = eu.other(u);
            if (vGraph->hasNode(w))
                edgesToAdd.push_back(eu);
        }
    }

    // Add edges
    for (Edge e: edgesToAdd) {
        vGraph->addEdge(e);
        KTrussUpdate::updateEdgeInsertion(e, index);
    }

    AbstractPPKTruss::KTrussAnswer answer;
    for (int u: vGraph->getNodes()) {
        if (answer.hasNode(u))
            continue;
        auto tmpKTruss = KTrussQuery::findKTruss(u, k - 1, index);
        answer.unionGraph(tmpKTruss);
    }
    vector<int> answerNodes;
    for (int u: answer.getNodes())
        answerNodes.push_back(u);
    for (int u: answerNodes)
        answer.addEdge(v, u);
	delete vGraph;
	delete index;
    return answer;
}
