#include "AbstractPPKTruss.h"
#include "NeighborhoodIndex.h"

#ifndef KTRUSS_V1_EDGEBASEDPPKTRUSS_H
#define KTRUSS_V1_EDGEBASEDPPKTRUSS_H

class EdgeBasedPPKTruss : public AbstractPPKTruss {
public:
    EdgeBasedPPKTruss(PPGraph* ppGraph);
    ~EdgeBasedPPKTruss();

    void preProcess() override ;
    void preProcessForNode(int v);
    AbstractPPKTruss::KTrussAnswer query(int v, int k) override ;
    AbstractPPKTruss::KTrussAnswer query(int v, int k, bool withPreProcess);
private:
    PPGraph* graph;
    NeighborhoodIndex* neighborhoodIndex;
};

#endif //KTRUSS_V1_EDGEBASEDPPKTRUSS_H
