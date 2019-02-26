#include "../KTrussIndex/KTrussIndex.h"
#include "../PPGraph/PPGraph.h"

#ifndef KTRUSS_V1_GENERALPPKTRUSS_H
#define KTRUSS_V1_GENERALPPKTRUSS_H

namespace GeneralPPKTruss {
    Graph queryGeneralPPKTrussNodeBased(int v, int k, KTrussIndex *publicIndex, PPGraph *ppGraph);
    Graph queryGeneralPPKTrussEdgeBased(int v, int k, KTrussIndex *publicIndex, PPGraph *ppGraph);
    Graph queryGeneralPPKTrussDecompositionBased(int v, int k, Graph* publicG, PPGraph *ppGraph);
};

#endif //KTRUSS_V1_GENERALPPKTRUSS_H
