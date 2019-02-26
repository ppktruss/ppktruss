#include "../Graph/Graph.h"

#ifndef KTRUSS_V1_PARSER_H
#define KTRUSS_V1_PARSER_H

class Parser {
public:
    Parser() {}
    void readSimpleGraph(std::string graphFileName, Graph* graph) const;
};

#endif //KTRUSS_V1_PARSER_H
