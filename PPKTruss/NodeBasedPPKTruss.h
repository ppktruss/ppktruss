#include "AbstractPPKTruss.h"
#include "NeighborhoodIndex.h"

#ifndef KTRUSS_V1_NODEBASEDPPKTRUSS_H
#define KTRUSS_V1_NODEBASEDPPKTRUSS_H

class NodeBasedPPKTruss : public AbstractPPKTruss {
public:
    NodeBasedPPKTruss(PPGraph* ppGraph);
    ~NodeBasedPPKTruss();

    void preProcess() override ;
    void preProcessForNode(int v);
    AbstractPPKTruss::KTrussAnswer query(int v, int k) override ;
    AbstractPPKTruss::KTrussAnswer query(int v, int k, bool withPreProcess);
private:
    PPGraph* graph;
    NeighborhoodIndex* neighborhoodIndex;
};

#endif //KTRUSS_V1_NODEBASEDPPKTRUSS_H
