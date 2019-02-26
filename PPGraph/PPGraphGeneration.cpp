#include "PPGraphGeneration.h"
#include "../Graph/Graph.h"
#include "PPGraph.h"

using namespace std;

PPGraph *PPGraphGeneration::generateCopyPPGraph(Graph *publicGraph) {
    int n = publicGraph->getMaxN();
    PPGraph *ppGraph = new PPGraph(publicGraph);
    for (int i: publicGraph->getNodes()) {
        Graph::Neighbors ngV = publicGraph->neighbors(i);
        int v = n + i;
        for (int j = 0; j < (int) ngV.size(); j++) {
            int u = ngV[j].other(i);
            if (rand() % 3 == 0)
                ppGraph->addPrivateEdge(v, u);
            else
                ppGraph->getPublicGraph()->addEdge(v, u);
        }
    }
    return ppGraph;
}


PPGraph* PPGraphGeneration::generateFromGraph(Graph *publicGraph, Graph *privateGraph) {
    PPGraph *ppGraph = new PPGraph(publicGraph);
    for (Edge e: privateGraph->getEdges()) {
        if (!ppGraph->hasPublicEdge(e.getX(), e.getY()))
            ppGraph->addPrivateEdge(e);
    }
    return ppGraph;
}
