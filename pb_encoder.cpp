#include "pb_encoder.h"

/*****************************************************************************
 * Konstruktor, initialisiert alle aus "encoder" geerbten member variablen
 * und setzt außerdem die Ausgabedatei auf den übergebenen Namen.
*/
PB_Encoder::PB_Encoder(
        const char* outputFilename,
        const lemon::ListDigraph &givenGraph,
        const lemon::ListDigraph::NodeMap<std::string> &name,
        const lemon::ListDigraph::NodeMap<std::string> &constraintAddOn,
        const lemon::ListDigraph::NodeMap<PkgVersion> &version,
        const lemon::ListDigraph::NodeMap<std::string> &priority,
        const lemon::ListDigraph::NodeMap<std::string> &architecture,
        const lemon::ListDigraph::NodeMap<std::string> &dependencies,
        const lemon::ListDigraph::NodeMap<std::string> &conflicts,
        const lemon::ListDigraph::NodeMap<std::string> &size,
        const lemon::ListDigraph::NodeMap<std::string> &section,
        const lemon::ListDigraph::ArcMap<std::string> &arcTypeMap)
    : Encoder(givenGraph,
              name,
              constraintAddOn,
              version,
              priority,
              architecture,
              dependencies,
              conflicts,
              size,
              section,
              arcTypeMap){
    setOutputFile(outputFilename);
}
// end constructor
//****************************************************************************


/*****************************************************************************
 * Iteriert über alle von einem Knoten "sourceNode" ausgehenden Kanten und
 * schreibt die daraus resultierenden Pseudo-Boolea-Constraints in die
 * Ausgabe-Datei. Dabei unterscheidet der Algorithmus zwischen den verschiedenen
 * Kanten-taypen ("root", "depend", "conflict")
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * outFile: std::ofstream&, Ausgabe-Datei
 * sourceNode: lemon::ListDigraph::Node&, Elternknoten der Kanten.
 *
 * Methodeninterne Variablen:
 * alreadyCheckedNames: std::set<std::string>, alle Paketnamen, der bereits
 *                      abgehandelten Kanten
 * alternativesAsOneString: Alle verschiedenen Alternativen zu einem Paket
 *                          zusammen gefasst als String: (P1 + P2 +...+P_n)
 * tempName: Name des aktuell verwendeten Zielknotens.
 *
*/
void PB_Encoder::outEdgesToPseudoBooleanConstraint(
        std::ofstream& outFile,
        lemon::ListDigraph::Node &sourceNode){
    std::set<std::string> alreadyCheckedNames;
    for(lemon::ListDigraph::OutArcIt arc1_It(this->graph, sourceNode);
        arc1_It != lemon::INVALID;
        ++arc1_It){
        if(this->arcTypeMap[arc1_It] == "root"){
            outFile << this->name[this->graph.target(arc1_It)] +
                       this->version[this->graph.target(arc1_It)].wholeVersion()
                    << " >= 1;"
                    << std::endl;
        }

        if(this->arcTypeMap[arc1_It] == "conflict"){
            outFile << this->name[sourceNode] +
                       this->version[sourceNode].wholeVersion()
                    << " + "
                    << this->name[this->graph.target(arc1_It)] +
                       this->version[this->graph.target(arc1_It)].wholeVersion()
                    << " <= 1;" << std::endl;
        }

        if(this->arcTypeMap[arc1_It] == "depend"){
            std::string alternativesAsOneString;
            std::string tempName(this->name[this->graph.target(arc1_It)]);
            alternativesAsOneString =
            collectAllAlternatives(sourceNode, tempName, alreadyCheckedNames);
            if(alreadyCheckedNames.find(tempName) == alreadyCheckedNames.end()){
                alreadyCheckedNames.insert(tempName);
                if(alternativesAsOneString.find('+') != std::string::npos){
                    outFile << alternativesAsOneString << " <= 1;" << std::endl;
                }
                outFile << alternativesAsOneString << " - "
                        << this->name[sourceNode] +
                           this->version[sourceNode].wholeVersion()
                        << " >= 0;" << std::endl;
            }
        }
    }
}
//end outEdgesToPseudoBooleanConstraint()
//****************************************************************************


/*****************************************************************************
 * Itereiert über alle von "sourceNode" ausgehende Kanten und gibt alle zu
 * varaibleName gefundenen Alternativen in einem String zurück.
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * sourceNode: lemon::ListDigraph::Node&, Elternknoten
 * variableName: std::string, Name, zu dem Alternativen gesucht werden.
 * alreadyCheckedNames: std::set<std::string>&, set, in dem alle
 *                      bereits behandelten Namen stehen
 *
 * Methodeninterne Variablen:
 * alternativesAsOneString: std::string, Alle verschiedenen Alternativen zu
 *                          einem Paket zusammen gefasst als String:
 *                          (P1 + P2 +...+P_n)
 *
*/
std::string PB_Encoder::collectAllAlternatives(
        lemon::ListDigraph::Node& sourceNode,
        std::string variableName,
        std::set<std::string>& alreadyCheckedNames){
    std::string alternativesAsOneString;

    for(lemon::ListDigraph::OutArcIt arcIt(this->graph,sourceNode);
        arcIt != lemon::INVALID;
        ++arcIt){
        if(this->name[this->graph.target(arcIt)] == variableName){
            if(alreadyCheckedNames.find(this->name[this->graph.target(arcIt)])==
               alreadyCheckedNames.end()){
                if(alternativesAsOneString.empty()){
                    alternativesAsOneString =
                    this->name[this->graph.target(arcIt)] +
                    this->version[this->graph.target(arcIt)].wholeVersion();
                }
                else{
                    alternativesAsOneString +=
                    (" + " +
                     this->name[this->graph.target(arcIt)] +
                     this->version[this->graph.target(arcIt)].wholeVersion());
                }
            }
        }
    }
    return alternativesAsOneString;
}
//end collectAllAlternatives()
//****************************************************************************


/*****************************************************************************
 * Itereiert über alle von "sourceNode" ausgehende Kanten und gibt alle zu
 * varaibleName gefundenen Alternativen in einem String zurück.
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * sourceNode: lemon::ListDigraph::Node&, Elternknoten
 * variableName: std::string, Name, zu dem Alternativen gesucht werden.
 * alreadyCheckedNames: std::set<std::string>&, set, in dem alle
 *                      bereits behandelten Namen stehen
 *
 * Methodeninterne Variablen:
 * alternativesAsOneString: std::string, Alle verschiedenen Alternativen zu
 *                          einem Paket zusammen gefasst als String:
 *                          (P1 + P2 +...+P_n)
 *
*/
void PB_Encoder::collectAllAlternatives(
        lemon::ListDigraph::Node &sourceNode,
        std::string variableName,
        std::list<PkgVersion> &alternatives){
    for(lemon::ListDigraph::OutArcIt arcIt(this->graph,sourceNode);
        arcIt != lemon::INVALID;
        ++arcIt){
        if(this->name[this->graph.target(arcIt)] == variableName){
                alternatives.push_back(this->version[this->graph.target(arcIt)]);
        }
    }
    alternatives.sort();
    alternatives.unique();
}
//end collectAllAlternatives()
//***************************************************************************


/*****************************************************************************
 * Setzt das Gewicht von "variable" auf "weight". Falls die variable noch nicht
 * in weightMap steht wird sie der map hinzugefügt.
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * variable: std::string, VariablenName
 * weight: int, Gewicht der Variable.
 *
 * Methodeninterne Variablen:
 *
*/
void PB_Encoder::setWeight(
        std::string variable,
        int weight){
    if(this->weightMap.find(variable) != this->weightMap.end()){
        this->weightMap.at(variable) = weight;
    }
    else{
        this->weightMap.insert(std::pair<std::string, int>(variable,weight));
    }
}//end setWeight
//****************************************************************************


/*****************************************************************************
 * Initialisiert die Gewichte aller Knoten im Graphen auf 1, wobei die Gewichte
 * dem Namen + Version zugeordnet werden.
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 *
 * Methodeninterne Variablen:
 *
*/
void PB_Encoder::initalizeWeights(){
    for(lemon::ListDigraph::NodeIt iter(this->graph); iter != lemon::INVALID; ++iter){
        setWeight(this->name[iter] + this->version[iter].wholeVersion(), 1);
    }
}
//end initalizeWeights
//****************************************************************************


/*****************************************************************************
 * Schreibt die Benutzerbedingung in die Ausgabedatei entsprechend der
 * Vorgabe("removal", "number", "freshness")
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * outFile: std::ofstream&, Ausgabe-Datei
 * userPolicy: const std::string&, Benutzervorgabe für objective-function
 *
 * Methodeninterne Variablen:
 *
*/
void PB_Encoder::userPolicyToObjectiveFunction(
        std::ofstream& outFile,
        const std::string& userPolicy){
    //minimizes the amount of removed packages:
    if(userPolicy == "removal" || userPolicy == "A"){
        removalAsUserPolicy(outFile);
        return;
    }
    // minimizes the total amount of packages being installed:
    if(userPolicy == "number" || userPolicy == "B"){
        numberAsUserPolicy(outFile);
        return;
    }
    // minimizes the number of packages, that are not updated to the newest version
    if(userPolicy == "freshness" || userPolicy == "C"){
        freshnessAsUserPolicy(outFile);
        return;
    }
    else{
        std::cout << "Die folgende Benutzereingabe ist nicht möglich: "
                  << userPolicy << std::endl;
    }
}
//end userPolicyToObjectiveFunction()
//****************************************************************************


/*****************************************************************************
 * schreibt: Minimum der Summe aller Variablen
 * in die Augabe-Datei (number-Policy)
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * outFile: std::ofstream&, Ausgabe-Datei
 *
 * Methodeninterne Variablen:
 * variableNames: std::vector<std::string>, vector in dem alle
 *                Variablennamen (Name + Version) stehen
*/
void PB_Encoder::numberAsUserPolicy(
        std::ofstream& outFile){
    std::vector<std::string> variableNames;
    for(lemon::ListDigraph::NodeIt nodeIter(this->graph);
        nodeIter != lemon::INVALID;
        ++nodeIter){
        if(this->graph.id(nodeIter) != 0){
            variableNames.push_back(this->name[nodeIter] +
                                    this->version[nodeIter].wholeVersion());
        }
    }
    outFile << "min: ";
    for(std::vector<std::string>::iterator iter = variableNames.begin();
        iter != variableNames.end();
        iter++){
        if(iter == variableNames.begin()){
            outFile << this->weightMap[*iter] << "*" << *iter;
        }
        else{
            outFile << " + " << this->weightMap[*iter] << "*" << *iter;
        }
    }
    outFile << ";" << std::endl;
}
//end userPolicyToObjectiveFunction()
//****************************************************************************


/*****************************************************************************
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * outFile: std::ofstream&, Ausgabe-Datei
 *
 * Methodeninterne Variablen:
 *
*/
void PB_Encoder::removalAsUserPolicy(
        std::ofstream &outFile){

}
//end removalAsUserPolicy()
//****************************************************************************


/*****************************************************************************
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * outFile: std::ofstream&, Ausgabe-Datei
 *
 * Methodeninterne Variablen:
 *
*/
void PB_Encoder::freshnessAsUserPolicy(
        std::ofstream &outFile){
    std::set<std::string> variableNames;
    for(lemon::ListDigraph::NodeIt nodeIter(this->graph);
        nodeIter != lemon::INVALID;
        ++nodeIter){
        if(this->graph.id(nodeIter) != 0){
            for(lemon::ListDigraph::InArcIt arcIt(this->graph,nodeIter); arcIt != lemon::INVALID; ++arcIt){
                if(variableNames.find(this->name[nodeIter] + this->version[nodeIter].wholeVersion()) == variableNames.end()){
                    setWeight(this->name[nodeIter] +
                              this->version[nodeIter].wholeVersion(),
                              calculateNormalizedDistance(
                                  this->graph.source(arcIt),
                                  this->name[nodeIter],
                                  this->version[nodeIter]));
                    variableNames.insert(this->name[nodeIter] + this->version[nodeIter].wholeVersion());
                }
            }// end inner for (arciter)
        } //end if
    }//end outer for (nodeiter)
    outFile << "min: ";
    for(std::set<std::string>::iterator it = variableNames.begin();
        it != variableNames.end();
        it++){
        if(it == variableNames.begin()){
            outFile << this->weightMap[*it] << "*" << "'" + *it +"'";
        }
        else{
            outFile << " + " << this->weightMap[*it] << "*" << *it;
        }
    }
    outFile << ";" << std::endl;
}
//end freshnessAsUserPolicy()
//****************************************************************************


/*****************************************************************************
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * outFile: std::ofstream&, Ausgabe-Datei
 *
 * Methodeninterne Variablen:
 *
*/
int PB_Encoder::calculateNormalizedDistance(
        lemon::ListDigraph::Node sourceNode,
        std::string name,
        PkgVersion version){
    int distance = 0;
    std::list<PkgVersion> alternatives;
    collectAllAlternatives(sourceNode, name, alternatives);
    for(std::list<PkgVersion>::iterator it = alternatives.begin(); it != alternatives.end(); it++){
        if(*it == version){
            return distance;
        }
        else{
            distance++;
        }
    }
}
//end calculateNormalizedDistance()
//****************************************************************************


/*****************************************************************************
 * schreibt die objective-function in die Ausgabe-Datei und
 * Kodiert den Graphen in Pseudo-Boolean-Bedingungen.
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 *
 * Methodeninterne Variablen:
 * outfile, std::ofstream, Ausgabe-Datei
 *
*/
void PB_Encoder::encode(){
    initalizeWeights();
    std::ofstream outfile;

    outfile.open("car_test_new.opb");
    outfile << "/* objective function comes first: */" << std::endl;
    userPolicyToObjectiveFunction(outfile,"freshness");
    outfile << "/* constraints: */" << std::endl;

    for(lemon::ListDigraph::NodeIt nodesIt(this->graph); nodesIt != lemon::INVALID; ++nodesIt){
        outEdgesToPseudoBooleanConstraint(outfile,nodesIt);
    }
    outfile.close();
}
//end encode()
//****************************************************************************
