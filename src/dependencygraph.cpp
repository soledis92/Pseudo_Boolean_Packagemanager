#include "dependencygraph.h"

/*****************************************************************************
 * Konstruktor, initialisiert alle lemon-maps, in denen zu Knoten / Kanten
 * gehörige Informationen abgespeichert werden. setzt "sourceFile" auf den
 * angegebenen Dateinamen. Erzeugt einen Graphen aus den
 * in packages stehenden Paketen. (siehe buildGraph())
*/
Dependencygraph::Dependencygraph(
        char *&sourcefileName,
        std::vector<std::string> &packages):
    name(this->graph,""),
    constraintAddOn(this->graph,""),
    version(this->graph),
    priority(this->graph,""),
    architecture(this->graph,""),
    dependencies(this->graph,""),
    conflicts(this->graph,""),
    size(this->graph,""),
    section(this->graph,""),
    arcTypeMap(this->graph,"")
{
    this->sourceFile = sourcefileName;
    buildGraph(packages);
}
//end Constructor
//******************************************************************************


/*****************************************************************************
 * displayGraph gibt den Kompletten Graphen in folgendem Format aus:
 * 1.: detailierte Metadaten zu jedem einzelnen Pakte, dass als Knoten im
 * Graphen enthalten ist.
 * 2.: Auflistung aller Knoten im Graphen.
 * 3.: Auflistung aller Kanten im Graphen.
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * graph: G(V,E) der dargestellt werden soll
 * name: Paketname
 * nameidMap: Schlüssel-Wert-Paar: <Paketname, Knoten-Identifikationsnummer im
 * Graphen
 * constraintAddOn: Zusatzinformationen zu Paketnamen, die
 * Installationsbedingungen enthalten: Bsp.: (>= v1.0.1)
 * version: Versionsnummer des Pakets
 * priority: Priorität des Pakets
 * architecture: benötigte Rechner-Architektur
 * dependencies: Abhängigkeitsinformationen (Paketnamen + Zusatzbedingungen)
 * conflicts: Konfliktinformation (Paketnamen + Zusatzbedingungen)
 * size: Größe des Pakets
 * section:
 *
 * Methodeninterne Variablen:
 * iter: Iterator über Knoten im Graphen
*/
void Dependencygraph::displayGraph(){
    std::cout << "detailed node information:" << '\n'<< '\n';
    for(lemon::ListDigraph::NodeIt iter(this->graph);
        iter != lemon::INVALID;
        ++iter){
        std::cout << "name: "
                  << this->name[iter]
                  << '\n';
        std::cout << "id: "
                  << this->nameidMap[this->name[iter] +
                     this->version[iter].wholeVersion()]
                  << '\n';
        std::cout << "constraints: " << this->constraintAddOn[iter] << '\n';
        std::cout << "version: " << this->version[iter] << '\n';
        std::cout << "prio: " << this->priority[iter] << '\n';
        std::cout << "arch: " << this->architecture[iter] << '\n';
        std::cout << "depends: " << this->dependencies[iter] << '\n';
        std::cout << "conflicts: " << this->conflicts[iter] << '\n';
        std::cout << "size:"  << this->size[iter] << '\n';
        std::cout << "section: " << this->section[iter] << '\n';
        std::cout << "-------------------------" << '\n';
        }
    std::cout << "List of all nodes in graph:" << '\n';
    for(lemon::ListDigraph::NodeIt iter(this->graph); iter != lemon::INVALID; ++iter){
        std::cout << this->name[iter] << ' ' << this->version[iter] << '\n';
    }
    std::cout << '\n';
    std::cout << "List of all edges in graph:" << '\n';
    for(lemon::ListDigraph::ArcIt iter(this->graph);
        iter != lemon::INVALID;
        ++iter){
        std::cout << this->name[this->graph.source(iter)]
                << " " << this->version[this->graph.source(iter)]
                << " -> "
                << this->name[this->graph.target(iter)]
                << " " << this->version[this->graph.target(iter)]
                << " (Arctype: " << this->arcTypeMap[iter] << ")"
                << '\n';
    }
    std::cout << std::endl;

}
//end displayGraph()
//******************************************************************************


/*****************************************************************************
 * überprüft, ob ein String mit einem bestimmten Trennzeichen teilbar ist.
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * wholeString: zu prüfender String
 * delimiter: Trennzeichen
*/
bool Dependencygraph::cuttable(
        std::string wholeString,
        std::string delimiter){
    if(wholeString.find(delimiter) != std::string::npos){
        return true;
    }
    else {
        return false;
    }
}
//end cuttable()
//****************************************************************************


/*****************************************************************************
 * teilt einen String per bestimmtem/n Trennzeichen auf und übergiebt diese als
 * std::vector<std::string>. Wenn Trennzeihen nicht enthalten ist, wird der
 * komplette String als std::vector<std::string> übergeben.
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * singleStringVec: std::vector in dem nach dem Teilen die einzelnen Teilstrings
 * stehen.
 * wholeString: zu teilender String
 * delimiter: Trennzeichen
 *
 * Methodeninterne Variablen:
 * pos: aktuelle Position, an der geteilt werden soll
 * token: länge des gefundenen Teilabschnitts
*/
void Dependencygraph::cutString(
        std::vector <std::string>& singleStringVec,
        std::string wholeString,
        std::string delimiter){
    if(cuttable(wholeString,delimiter)){
        size_t pos = 0;
        std::string token;
        while((pos = wholeString.find(delimiter)) != std::string::npos){
            token = wholeString.substr(0,pos);
            singleStringVec.push_back(token);
            wholeString.erase(0, pos + delimiter.length());
        }
        singleStringVec.push_back(wholeString);
    }
    else{
        singleStringVec.push_back(wholeString);
    }
}
//end cutString()
//****************************************************************************


/*****************************************************************************
 * sucht im Paketnamen nach Alternativen (p1 | p2)
 * und Versionsbedingungen (>= v.1.1)
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * pkgName: Name des zu überprüfenden Paketes inkl.
 * Sonderbedingungen (Alternativen/ Versionsbedingungen)
 * splitNameVector: std::vector in dem Paketnamen getrennt von Sonderbedingungen
 * eingetragen werden.
 *
 * Methodeninterne Variablen:
 * s1, s2, s3: Marker für Existenz von Sonderbedingungen
 * wholeNamesOfAlternatives: std::vector in dem die vollständigen Paketnamen
 * (inkl. Sonderbedingungen) verschiedener Alternativen stehen.
 */
void Dependencygraph::checkForExtraConstraints(
        std::string pkgName,
        std::vector<std::string> &splitNameVector){
    std::string s1,s2,s3,s4;
    s1 = "(";
    s2 = "|";
    s3 = ")";
    s4 = " ";

    if(pkgName.find(s2)!= std::string::npos){
        std::vector<std::string> wholeNamesOfAlternatives;
        cutString(wholeNamesOfAlternatives,pkgName,s2);
        for(std::vector<std::string>::iterator it =
            wholeNamesOfAlternatives.begin();
            it != wholeNamesOfAlternatives.end();
            it++){
            checkForExtraConstraints(*it,splitNameVector);
        }
    }
    else{
        if(pkgName.find(s1) != std::string::npos){
            pkgName.erase(pkgName.find(s3),s3.length());
            cutString(splitNameVector,pkgName,s1);
        }
        else{
            splitNameVector.push_back(pkgName);
            splitNameVector.push_back("");
        }
        for(std::vector<std::string>::iterator it = splitNameVector.begin();
            it != splitNameVector.end();
            it++){
            if(it->find(s4) != std::string::npos){
                it->erase(it->find(s4),s4.length());
            }
        }
    }
}
//end checkForExtraConstraints()
//****************************************************************************


/*****************************************************************************
 * Wertet eine einzelne Zusatzbedingung aus und gibt an einen
 * std::vector<std::string> in folgendem Format das Ergebnis zurück:
 * [gültiger Vergleichsoperator, Paketname]
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * constraint: std::string in dem Bedingung + Paketname enthalten ist.
 * resultVector: std::vector<std::string>& in dem Ergebnis gespeichert wird.
 *
 * Methodeninterne Variablen:
 *
*/
void Dependencygraph::evaluateConstraintAddOn(
        std::string constraint,
        std::vector<std::string> &resultVector){
    if(constraint.substr(0,1) == "="){
        resultVector.push_back("=");
        resultVector.push_back(constraint.substr(1));
    }
    if(constraint.substr(0,1) == ">"){
        if(constraint.substr(0,2) == ">="){
            resultVector.push_back(">=");
            resultVector.push_back(constraint.substr(2));
        }
        else{
            resultVector.push_back(">");
            resultVector.push_back(constraint.substr(1));
        }
    }
    if(constraint.substr(0,1) == "<"){
        if(constraint.substr(0,2) == "<="){
            resultVector.push_back("<=");
            resultVector.push_back(constraint.substr(2));
        }
        else{
            resultVector.push_back("<");
            resultVector.push_back(constraint.substr(1));
        }
    }
}
//end evaluateConstraintAddOn()
//****************************************************************************


/*****************************************************************************
 * fügt "tempNode" die in "sourceFile", nach Zeile "tempLine", stehenden
 * Metadaten hinzu.
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * tempNode: lemon::ListDigraph::Node& Knoten im Graphen, dem Metadaten
 *           hinzugefügt werden
 * tempLine: int Zeile in "sourceFile" nach der die entsprechenden
 *           Metadaten folgen.
 *
 * Methodeninterne Variablen:
 * data: std::string, der als Zwischenspeicher für die einzelnen eingelesenen
 *       Zeilen dient.
 * inputfile: std::ifstream zum Öffnen des "sourceFile"
 *
*/
void Dependencygraph::jumpBufferMetadata(
        lemon::ListDigraph::Node &tempNode,
        int tempLine){
    std::string data;
    std::ifstream inputfile(this->sourceFile);
    if(inputfile.is_open()){
        inputfile.close();
        inputfile.open(this->sourceFile);
    }

    for(int i = 0; i < tempLine; i++){
        std::getline(inputfile,data);
    }

    bool done = false;
    while(std::getline(inputfile,data)){
        if(done == true){
            break;
        }
        if(data.substr(0,8) == "Package:"){
            this->name[tempNode] = data.substr(9);
            while(std::getline(inputfile,data)){
                if(data.substr(0,9) == "Priority:"){
                    if(data.substr(9).size() > 0){
                        this->priority[tempNode] = data.substr(10);
                    }
                }
                if(data.substr(0,10) == "Conflicts:"){
                    if(data.substr(10).size() > 0){
                        this->conflicts[tempNode] = data.substr(11);
                    }
                }
                if(data.substr(0,8) == "Depends:"){
                    if(data.substr(8).size() > 0){
                        this->dependencies[tempNode] = data.substr(9);
                    }
                }
                if(data.substr(0,8) == "Section:"){
                    if(data.substr(8).size() > 0){
                        this->section[tempNode] = data.substr(9);
                    }
                }
                if(data.substr(0,15) == "Installed-Size:"){
                    if(data.substr(15).size() > 0){
                        this->size[tempNode] = data.substr(16);
                    }
                }
                if(data.substr(0,8) == "Version:"){
                    if(data.substr(8).size() > 0){
                        this->version[tempNode] = PkgVersion(data.substr(9));
                    }
                }
                if(data.substr(0,13) == "Architecture:"){
                    if(data.substr(13).size() > 0){
                        this->architecture[tempNode] = data.substr(14);
                    }
                }
                if(!data.size()){
                    done = true;
                    break;
                }
            }
        }
    }
    inputfile.close();
}
//end jumpBufferMetadata()
//****************************************************************************


/*****************************************************************************
 * zählt die Zeilenanzahl von "sourceFile"
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 *
 * Methodeninterne Variablen:
 * file: std::ifstream, zum Öffnen von "sourceFile"
 * data: std::string, zeilenweise Zwischenspeicher
 * numberOfLines: int Zeilenzähler
 *
*/
int Dependencygraph::getNumberOfLinesOfSourceFile(){
    std::ifstream file;
    file.open(this->sourceFile);
    std::string data;
    int numberOfLines = 0;
    while(std::getline(file,data)){
        ++numberOfLines;
    }
    file.close();
    return numberOfLines;
}
//end getNumberOfLinesOfSourceFile()
//****************************************************************************


/*****************************************************************************
 * sucht im "sourceFile" hinter Zeile "skipLines" nach nächster Zeile,
 * die den Paketnamen "pkgName" enthält.
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * pkgName: std::string, Name des Pakets
 * skipLines, int Anzahl der Zeilen, die vor der Suche übersprungen werden.
 *
 * Methodeninterne Variablen:
 * data: std::string, Zwischenspeicher der gelesenen Zeile.
 * inputfile: std::ifstream, zum Öffnen von "sourceFile"
 * pkgfound: bool, gibt an ob Paket in "sourceFile" gefunden wurde.
 * lineNumber: int return-value: -1, falls Paket nicht gefunden,
 *             sonst Zeilenzahl in der Paket gefunden wurde.
 *
*/
int Dependencygraph::searchForPkg(
        std::string pkgName,
        int skipLines){
    std::string data;
    std::ifstream inputfile;
    bool pkgfound=false;
    int lineNumber=skipLines;

    if(inputfile.is_open()){
        inputfile.close();
    }
    inputfile.open(this->sourceFile);

    for(int i = 0; i < skipLines; i++){
        std::getline(inputfile,data);
    }
    while(std::getline(inputfile,data)){
        if(data.substr(0,8) == "Package:"){
            if(data.substr(9) == pkgName){
                pkgfound = true;
                break;
            }
        }
        ++lineNumber;
    }
    inputfile.close();
    if(pkgfound == true){
        return lineNumber;
    }
    else{
        return -1;
    }
}
//end searchForPkg()
//****************************************************************************


/*****************************************************************************
 * siehe searchForPkg(std::string pkgName, int skipLines)
 * akzeptiert gefundenes Paket jedoch nur, wenn Versionsbedingung erfüllt ist.
*/
int Dependencygraph::searchForPkg(
        std::string pkgName,
        std::string pkgVersionConstraint,
        int skipLines){
    std::string data;
    std::ifstream inputfile;
    if(inputfile.is_open()){
        inputfile.close();
    }
    inputfile.open(this->sourceFile);

    for(int i = 0; i < skipLines; i++){
        std::getline(inputfile,data);
    }

    bool pkgfound = false;
    std::vector<std::string> constraintEval;
    evaluateConstraintAddOn(pkgVersionConstraint,constraintEval);
    PkgVersion compareVersion = PkgVersion(constraintEval[1]);

    int lineNumber=skipLines;

    while(std::getline(inputfile,data)){
        if(data.substr(0,8) == "Package:"){
            if(data.substr(9) == pkgName){
                while(std::getline(inputfile,data)){
                    if(data.substr(0,8) == "Version:"){
                        PkgVersion tempVersion = PkgVersion(data.substr(9));
                        if(constraintEval[0] == "="){
                            if(tempVersion == compareVersion){pkgfound = true; goto end;}
                            else{goto outerWhileLoop;}
                        }
                        if(constraintEval[0] == "<"){
                            if(tempVersion < compareVersion){pkgfound = true; goto end;}
                            else{goto outerWhileLoop;}
                        }
                        if(constraintEval[0] == "<="){
                            if(tempVersion <= compareVersion){pkgfound = true; goto end;}
                            else{goto outerWhileLoop;}
                        }
                        if(constraintEval[0] == ">"){
                            if(tempVersion > compareVersion){pkgfound = true; goto end;}
                            else{goto outerWhileLoop;}
                        }
                        if(constraintEval[0] == ">="){
                            if(tempVersion >= compareVersion){pkgfound = true; goto end;}
                            else{goto outerWhileLoop;}
                        }
                        else{goto outerWhileLoop;}
                    }
                }
            }
            else{goto outerWhileLoop;}
        }
        outerWhileLoop:
        lineNumber++;
    }

    end:
    inputfile.close();
    if(pkgfound == true){
        return lineNumber;
    }
    else{
        return -1;
    }
}
//end searchForPkg()
//****************************************************************************


/*****************************************************************************
 * nimmt Paketnamen inkl. Zusatzbedingungen entgegen, versucht entsprechende
 * Pakete zum Graphen hinzu zu fügen und gibt anschließend alle neuen
 * Paket-Ids an einen std::vector<int> zurück.
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * newPkgIds: std::vector<int>&,
 *            vector, in dem alle neuen Paket-Ids gespeichert werden.
 * sourceNodeId: int, Id des Knoten, an den neue Knoten angefügt werden sollen.
 *               enthält Abhängigkeitscode: -1* Id, falls conflict-Knoten
 *                                          angehängt werden.
 *                                          Id, falls normaler depend-Knoten.
 *                                          0, falls Paket Wurzelknoten ist.
 * pkgname: std::string, Name des Pakets
 *
 * Methodeninterne Variablen:
 * newPkgFound: bool, gibt an, ob mind. 1 neues Paket hinzugefügt wurde.
 * idOfnewPkgs: std::vector<int> enthält Ids aller gefundenen Pakete,
 *              wird nur weitergegeben, wenn mind. 1 neues Paket mit dabei ist.
 *
*/
void Dependencygraph::addNodeByName(
        std::vector<int>& newPkgIds,
        int sourceNodeId,
        std::string pkgname){
    bool newPkgFound = false;
    std::vector<int> idOfnewPkgs;
        std::vector<std::string> splitPkgName;
        checkForExtraConstraints(pkgname,splitPkgName);
        // i%2, weil in splitPkgName immer
        //[pkgname(pkg1),versionConstraint(pkg1), ...]
        for(int i = 0; i < splitPkgName.size(); i++){
            if(i%2 == 0){
                addNewNodeAndFillMetadata(splitPkgName[i],
                                          splitPkgName[i+1],
                                          newPkgFound,
                                          idOfnewPkgs);
            }
        }
    if(newPkgFound == true){
        for(std::vector<int>::iterator it = idOfnewPkgs.begin();
            it != idOfnewPkgs.end();
            it++){
            newPkgIds.push_back(*it);
            addArcByType(sourceNodeId,*it);
        }
    }
}
//end addNodeByName()
//****************************************************************************


/*****************************************************************************
 * Filtert zunächst alle Zeilen aus "sourceFile", in denen
 * Pakete (pkgName,versionConstraint) anfangen.
 * Wenn kein entsprechendes Paket gefunden wird, steht in idOfNewPkgs eine -1.
 * Ansonsten werden alle gefundenen Pakete dem Graphen hinzugefügt,
 * falls kein entsprechendes Paket bereits existiert.
 * Die Knoten-Ids werden in idOfNewPkgs abgespeichert.
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * pkgName: std::string, Name des Pakets
 * versionConstraint: std::string, zusätzliche Versionsbedingung
 * pkgFound: bool&, gibt an ob mind. 1 neues Paket gefunden wurde.
 * idOfNewPkgs: std::vector<int>&, Speicher für neu gefundene Paket-Ids.
 *
 * Methodeninterne Variablen:
 * lengthOfFile: int, ZeilenZahl von "sourceFile"
 * tempLine: std::vector<int>
 *
*/
void Dependencygraph::addNewNodeAndFillMetadata(std::string pkgName,
                                                std::string versionConstraint,
                                                bool &pkgFound,
                                                std::vector<int>& idOfNewPkgs){
    int lengthOfFile = getNumberOfLinesOfSourceFile();
    std::vector<int> tempLine;
    //handle <pkgname>:any, by simply deleting :any
    if(pkgName.find(":any") != std::string::npos){
        pkgName.erase(pkgName.find(":any"),4);
    }

    if(versionConstraint == ""){
        do{
            if(tempLine.empty()){
                tempLine.push_back(searchForPkg(pkgName, 0));
            }
            else if(tempLine.empty() == false){
                tempLine.push_back(searchForPkg(pkgName,tempLine.back()+1));
            }
        }while((tempLine.back()+1 < lengthOfFile)
               && (tempLine.back() != -1));
    }
    else{
        do{
            if(tempLine.empty() == false){
                tempLine.push_back(searchForPkg(pkgName,
                                                versionConstraint,
                                                tempLine.back()+1));
            }
            else if(tempLine.empty()){
                tempLine.push_back(searchForPkg(pkgName, versionConstraint, 0));
            }
        }while(tempLine.back()+1 < lengthOfFile && (tempLine.back() != -1));
    }

    if(tempLine.size() > 1 && tempLine.back() == -1){
        tempLine.erase(tempLine.end()-1);
    }

    for(std::vector<int>::iterator it = tempLine.begin();
                                   it != tempLine.end();
                                   it++){
        lemon::ListDigraph::Node tempPkg;
        tempPkg = this->graph.addNode();

        if((*it >= 0) && (*it < lengthOfFile)){
            jumpBufferMetadata(tempPkg,*it);
            // Paket existiert schon
            if((this->nameidMap.find(this->name[tempPkg]
             + (this->version[tempPkg]).wholeVersion()))
             != this->nameidMap.end()){
                this->graph.erase(tempPkg);
            }
            else if(this->nameidMap.find((this->name[tempPkg]
                  + this->version[tempPkg].wholeVersion()))
                  == this->nameidMap.end()){
                pkgFound = true;
                this->nameidMap[(this->name[tempPkg]
                              + (this->version[tempPkg]).wholeVersion())]
                              = lemon::ListDigraph::id(tempPkg);
                idOfNewPkgs.push_back(lemon::ListDigraph::id(tempPkg));
            }
        }

        if(*it == -1 && tempLine.size() == 1){
            pkgFound = false;
            std::cout << "abhängiges Paket: "<< pkgName
                      << " nicht gefunden!" << std::endl;
            std::cout << "Versionsbed.: " << versionConstraint << std::endl;
            this->graph.erase(tempPkg);
            if(idOfNewPkgs.empty()){
                idOfNewPkgs.push_back(-1);
            }
        }

        if(*it > lengthOfFile && *it != -1){
            std::cout << "Achtung Fehler!" << '\n'
                      << "Paketfile ist kürzer als Zeile in der gesucht wird!"
                      << std::endl
                      << *it << " > " << lengthOfFile
                      << std::endl;
            std::cout << tempLine.size() << std::endl;
        }
    }
}
//end addNewNodeAndFillMetadata()
//****************************************************************************


/*****************************************************************************
 * fügt dem Graphen eine gerichtete Kante zwischen sourceNodeId -> targetNodeId
 * hinzu. Zusätzlich wird die Art der Kante (root, depend, conflict) in
 * arcTypeMap eingetragen. Falls der Typ der Kante "conflict" ist,
 * wird eine weitere "conflict"-Kante von targetNodeId -> sourceNodeId eingefügt
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * sourceNodeId: const int&, Knoten-Id des Ursprungsknoten.
 *               Benutzt folgende Kodierung:
 *               -1 * Id, falls "conflict"
 *               Id, falls "depend"
 *               0, falls "root"
 * targetNodeId: const int&, Knoten-Id des Zielknotens
 *
 * Methodeninterne Variablen:
 *
*/
void Dependencygraph::addArcByType(
        const int &sourceNodeId,
        const int &targetNodeId){
    //source == root
    if(sourceNodeId == 0){
        lemon::ListDigraph::Arc tempArc =
        this->graph.addArc(lemon::ListDigraph::nodeFromId(sourceNodeId),
                           lemon::ListDigraph::nodeFromId(targetNodeId));
        this->arcTypeMap[tempArc] = "root";
    }
    //source == normal parent
    if(sourceNodeId > 0){
        lemon::ListDigraph::Arc tempArc =
        this->graph.addArc(lemon::ListDigraph::nodeFromId(sourceNodeId),
                           lemon::ListDigraph::nodeFromId(targetNodeId));
        this->arcTypeMap[tempArc] = "depend";
    }
    //source == conflict parent
    if(sourceNodeId < 0){
        lemon::ListDigraph::Arc tempArc =
        this->graph.addArc(lemon::ListDigraph::nodeFromId(-1*sourceNodeId),
                           lemon::ListDigraph::nodeFromId(targetNodeId));
        this->arcTypeMap[tempArc] = "conflict";
        tempArc =
        this->graph.addArc(lemon::ListDigraph::nodeFromId(targetNodeId),
                           lemon::ListDigraph::nodeFromId(-1*sourceNodeId));
        this->arcTypeMap[tempArc] = "conflict";
    }
}
// end addArcByType()
//****************************************************************************


/*****************************************************************************
 * erzeugt einen Stack, auf dem alle hinzuzufügenden
 * Pakete + Id des Elternknotens abgelegt werden.
 * Ansachließend werden die einzelnen Elemente nach einander vom Stack genommen
 * und gegebenen Falls vom jeweiligen Element abhängige (depend / conflict)
 * Pakete dem Stack hinzugefügt. Dies geschieht bis der Stack leer ist.
 *
 * Variablenübersicht:
 * An Methode übergebene Variablen:
 * packages: std::vector<std::string>&, Alle Pakete, die Wurzel des Graphen sind
 *
 * Methodeninterne Variablen:
 * root: lemon::ListDigraph::Node, "super"-Wurzel des Graphen
 *       (notwendig, damit die "root"-Pakete auch ganz normal auf dem Stack
 *        abgelegt werden können)
 * packageStack: std::stack<std::pair<std::string, int> >, Paket-Stack
 * newPkgAsSource: std::vector<int>, Alle Paket-Alternativen, die während des
 *                 Hinzufügens eines Pakets gefunden wurden.
 *                 Diese werden auch auf dem Stack abgelegt.
 * depends: std::vector<std::string>, alle Paketnamen + Zusatzbed.,
 *          die vom obersten Element auf dem Stack abhängen.
 * conflicts: std::vector<std::string>, alle Paketnamen + Zusatzbed.,
 *            die im Konflikt mit dem obersten Element des Stacks stehen.
 *
*/
void Dependencygraph::buildGraph(
        std::vector<std::string> &packages){
    // add root node:
    lemon::ListDigraph::Node root;
    root = this->graph.addNode();
    this->name[root] = "root";
    std::stack<std::pair<std::string, int> > packageStack;
    //push all main arguments on pkg-stack
    for(std::vector<std::string>::iterator it = packages.begin(); it != packages.end(); it++){
        packageStack.push(std::pair<std::string,int>(*it,0));
    }

    std::vector<int> newPkgAsSource;
    while(!packageStack.empty()){
        addNodeByName(newPkgAsSource, packageStack.top().second, packageStack.top().first);
        packageStack.pop();

        for(std::vector<int>::iterator sourceNodes = newPkgAsSource.begin(); sourceNodes != newPkgAsSource.end(); sourceNodes++){
            //adds all depending nodes to the graph
            if(this->dependencies[lemon::ListDigraph::nodeFromId(*sourceNodes)] != ""){
                std::vector<std::string> depends;
                cutString(depends,this->dependencies[lemon::ListDigraph::nodeFromId(*sourceNodes)],", ");
                for(std::vector<std::string>::iterator depIt = depends.begin(); depIt != depends.end(); depIt++){
                    packageStack.push(std::pair<std::string,int>(*depIt,*sourceNodes));
                }
            }//end if (depending nodes)
            if(this->conflicts[lemon::ListDigraph::nodeFromId(*sourceNodes)] != ""){
                std::vector<std::string> conflicts;
                cutString(conflicts, this->conflicts[lemon::ListDigraph::nodeFromId(*sourceNodes)],", ");
                for(std::vector<std::string>::iterator conIt = conflicts.begin(); conIt != conflicts.end(); conIt++){
                    packageStack.push(std::pair<std::string,int>(*conIt,-1*(*sourceNodes)));
                }
            }
        }
        newPkgAsSource.clear();
    }

}//end buildGraph()
//****************************************************************************
