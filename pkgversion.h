#ifndef PKGVERSION_H
#define PKGVERSION_H

#include <stdlib.h>
#include <string>
#include <iostream>

class PkgVersion
{
public:
    explicit PkgVersion(std::string version);
    PkgVersion();
    PkgVersion(const PkgVersion& other){*this = other;}

    std::string wholeVersion() const {return this->wholeVersionString;}
    int epoch() const {return this->epochval;}
    std::string upStream() const {return this->upStreamval;}
    std::string revision() const {return this->revisionval;}

    void epoch(int new_epoch){epochval = new_epoch;}
    void upStream(std::string new_upStream){upStreamval = new_upStream;}
    void revision(std::string new_revision){revisionval = new_revision;}
    void clearRevision(){revision("0");}

    void splitContent();

    PkgVersion& operator=(const PkgVersion& rightArg);
    bool operator==(const PkgVersion& rightArg);
    bool operator!=(const PkgVersion& rightArg);
    bool operator<(const PkgVersion& rightArg);
    bool operator>(const PkgVersion& rightArg);
    bool operator>=(const PkgVersion& rightArg);
    bool operator<=(const PkgVersion& rightArg);

    friend std::ostream& operator<<(std::ostream& os,const PkgVersion& version);


protected:
    std::string wholeVersionString;
    int epochval;
    std::string upStreamval;
    std::string revisionval;

    //static int CompareComponent(std::string a, std::string b); //c++-version
    static int CompareComponent(const char* a,const char* b); // c-version
};



#endif // PKGVERSION_H
