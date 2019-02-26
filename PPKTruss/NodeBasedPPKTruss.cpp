#include "NodeBasedPPKTruss.h"
#include "../KTrussUpdate/KTrussUpdate.h"
#include "../KTrussIndex/KTrussQuery.h"


using namespace std;

NodeBasedPPKTruss::NodeBasedPPKTruss(PPGraph *ppGraph) {
    this->graph = ppGraph;
    neighborhoodIndex = new NeighborhoodIndex(ppGraph);
}

NodeBasedPPKTruss::~NodeBasedPPKTruss() {
    delete neighborhoodIndex;
}

void NodeBasedPPKTruss::preProcess() {
    // TODO
}

void NodeBasedPPKTruss::preProcessForNode(int v) {
    neighborhoodIndex->calcPublicIndex(v);
}

AbstractPPKTruss::KTrussAnswer NodeBasedPPKTruss::query(int v, int k) {
    return query(v, k, true);
}

AbstractPPKTruss::KTrussAnswer NodeBasedPPKTruss::query(int v, int k, bool withPreProcess) {
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

    // Add nodes
    for (Edge e: this->graph->privateNeighbors(v)) {
        int u = e.other(v);
        if (vGraph->hasNode(u))
            continue;

        vGraph->addNode(u);
        vector<int> neighbors;
        for (Edge eu: this->graph->publicNeighbors(u)) {
            int w = eu.other(u);
            if (vGraph->hasNode(w)) {
                neighbors.push_back(w);
                vGraph->addEdge(eu);
            }
        }
        KTrussUpdate::updateNodeInsertion(u, neighbors, index);
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
