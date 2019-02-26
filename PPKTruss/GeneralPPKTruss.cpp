#include "GeneralPPKTruss.h"
#include "../KTrussIndex/KTrussIndex.h"
#include "../PPGraph/PPGraph.h"
#include "../KTrussUpdate/KTrussUpdate.h"
#include "../Graph/Graph.h"
#include "../KTrussIndex/KTrussQuery.h"
#include "../Utilities/ProcessTimer.h"
#include "../KTrussIndex/KTrussDecomposition.h"

using namespace std;

Graph GeneralPPKTruss::queryGeneralPPKTrussNodeBased(int v, int k, KTrussIndex *publicIndex, PPGraph *ppGraph) {
    Graph *publicGraph = publicIndex->getGraph();
    if (publicGraph->hasNode(v))
        KTrussUpdate::updateNodeDeletion(v, publicIndex);
    for (Edge e: ppGraph->privateNeighbors(v))
        publicGraph->addEdge(e);
    vector<int> ngV;
    for (Edge e: publicGraph->neighbors(v))
        ngV.push_back(e.other(v));
    KTrussUpdate::updateNodeInsertion(v, ngV, publicIndex);
    return KTrussQuery::findKTruss(v, k, publicIndex);
}

Graph GeneralPPKTruss::queryGeneralPPKTrussEdgeBased(int v, int k, KTrussIndex *publicIndex, PPGraph *ppGraph) {
    Graph *publicGraph = publicIndex->getGraph();
    for (Edge e: ppGraph->privateNeighbors(v)) {
        publicGraph->addEdge(e);
        KTrussUpdate::updateEdgeInsertion(e, publicIndex);
    }
    return KTrussQuery::findKTruss(v, k, publicIndex);
}

Graph GeneralPPKTruss::queryGeneralPPKTrussDecompositionBased(int v, int k, Graph *publicG, PPGraph *ppGraph) {
    Graph *publicGraph = publicG;
    for (Edge e: ppGraph->privateNeighbors(v))
        publicGraph->addEdge(e);
    KTrussDecomposition ktd = KTrussDecomposition(publicGraph);
    ktd.createIndex();
    KTrussIndex *publicIndex = ktd.getFinalIndex();
    auto ans = KTrussQuery::findKTruss(v, k, publicIndex);
	delete publicIndex;
	return ans;
}
