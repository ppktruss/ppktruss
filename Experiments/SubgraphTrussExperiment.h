#include <vector>


#ifndef KTRUSS_V1_SUBGRAPHTRUSSEXPERIMENT_H
#define KTRUSS_V1_SUBGRAPHTRUSSEXPERIMENT_H

namespace SubGraphTrussExperiment {
    void doSampling(int argc, char *argv[]);
    void doRun(int argc, char *argv[]);
    int subGraphTrussMain(int argc, char *argv[]);
    std::vector<int> readSampleFile(char *path);
};

#endif //KTRUSS_V1_SUBGRAPHTRUSSEXPERIMENT_H
