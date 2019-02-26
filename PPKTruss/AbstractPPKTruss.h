#include "../PPGraph/PPGraph.h"
#include <unordered_map>
#include "../Graph/Graph.h"

#ifndef KTRUSS_V1_ABSTRACTPPKTRUSS_H
#define KTRUSS_V1_ABSTRACTPPKTRUSS_H

class AbstractPPKTruss {
public:
    typedef Graph KTrussAnswer;
    AbstractPPKTruss();
    ~AbstractPPKTruss();
    AbstractPPKTruss(PPGraph* ppGraph);

    virtual void preProcess() = 0;
    virtual KTrussAnswer query(int v, int k) = 0;
protected:
    PPGraph* ppGraph;
};

#endif //KTRUSS_V1_ABSTRACTPPKTRUSS_H
