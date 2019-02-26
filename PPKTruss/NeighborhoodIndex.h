#include <unordered_map>
#include "../KTrussIndex/KTrussIndex.h"
#include "../PPGraph/PPGraph.h"

#ifndef KTRUSS_V1_NEIGHBORHOODINDEX_H
#define KTRUSS_V1_NEIGHBORHOODINDEX_H

class NeighborhoodIndex {
public:
    NeighborhoodIndex(PPGraph* ppGraph);
    ~NeighborhoodIndex();

    void calcPublicIndex(int v);
    KTrussIndex* getPublicIndex(int v) const;
private:
    PPGraph* graph;
    std::unordered_map<int, KTrussIndex*> publicIndex;
};

#endif //KTRUSS_V1_NEIGHBORHOODINDEX_H
