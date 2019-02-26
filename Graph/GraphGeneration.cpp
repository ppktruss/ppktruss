#include "GraphGeneration.h"
#include "../Parser/Parser.h"
#include <string>

using namespace std;

Graph* GraphGeneration::generateGraph(string graphFileName) {
    Parser parser = Parser();
    Graph *g = new Graph();
    parser.readSimpleGraph(graphFileName, g);
    return g;
}

