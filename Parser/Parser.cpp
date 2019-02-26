#include "Parser.h"
#include "../Graph/Graph.h"
#include <cstdio>
#include <string>

using namespace std;

void Parser::readSimpleGraph(string graphFileName, Graph *graph) const {
    FILE *fin = fopen(graphFileName.c_str(), "r");
    int x, y;
    while (fscanf(fin, "%d %d", &x, &y) != EOF) {
        if (x == y)
            continue;
        if (graph->hasEdge(x, y))
            continue;
        graph->addEdge(x, y);
    }
    fclose(fin);
}

