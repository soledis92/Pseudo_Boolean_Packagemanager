#include <iostream>

#include "dependencygraph.h"
#include "pb_encoder.h"

/*
 * Hauptprogramm:
 * liest als erstes Argument den Namen der Datei, in dem die Pakete gesucht
 * werden sollen ein.
 * weitere Argumente sind Paketnamen der zu installierenden Pakete.
*/
int main(int argn, char** argv)
{
    std::vector<std::string> packages;
    for(int i = 2; i < argn; i++){
        packages.push_back((std::string)argv[i]);

    }
    Dependencygraph* testgraph = new Dependencygraph(argv[1],packages);
    testgraph->displayGraph();

    PB_Encoder* testencoder = new PB_Encoder("ergebnis_testgraph.opb",
                                             testgraph->graph,
                                             testgraph->name,
                                             testgraph->constraintAddOn,
                                             testgraph->version,
                                             testgraph->priority,
                                             testgraph->architecture,
                                             testgraph->dependencies,
                                             testgraph->conflicts,
                                             testgraph->size,
                                             testgraph->section,
                                             testgraph->arcTypeMap);
    testencoder->encode();

    return 0;
}
//end main()


/*
 * Anmerkungen:
 * - in Dependencygraph muss in der methode jumpbuffer unter version noch korrigiert werden.
*/
