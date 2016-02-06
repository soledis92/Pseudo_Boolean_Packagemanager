#include "pkgversion.h"

PkgVersion::PkgVersion(std::string version)
{
    this->wholeVersionString = version;
    this->splitContent();
}

PkgVersion::PkgVersion(){
    this->wholeVersionString = "";
    this->epochval = 0;
    this->upStreamval = "";
    this->revisionval = "";
}

void PkgVersion::splitContent(){
    std::string::size_type endOfEpoch =
    this->wholeVersionString.find_first_of(":");
    std::string::size_type startOfUpstream;

    if(endOfEpoch == std::string::npos){
        this->epochval = 0;
        startOfUpstream =
        this->wholeVersionString.find_first_of(this->wholeVersionString);
    }
    if(endOfEpoch != std::string::npos){
        this->epochval = atoi(this->wholeVersionString.c_str());
        startOfUpstream = endOfEpoch + 1;
    }
    std::string::size_type endOfUpstream =
    this->wholeVersionString.find_first_of('-',startOfUpstream) - 2;
    if(endOfUpstream == std::string::npos){
        this->upStreamval = this->wholeVersionString.substr(startOfUpstream);
        this->revisionval = "0";
    }
    if(endOfUpstream != std::string::npos){
        this->upStreamval =
                this->wholeVersionString.substr(startOfUpstream,endOfUpstream);
        this->revisionval =
                this->wholeVersionString.substr(endOfUpstream + 3);
    }
}

bool PkgVersion::operator!=(const PkgVersion& rightArg){
    return !(*this == rightArg);
}

bool PkgVersion::operator==(const PkgVersion& rightArg){
    return this->epochval == rightArg.epochval
           && (CompareComponent(this->upStreamval.c_str(),
                                rightArg.upStreamval.c_str()) == 0)
           && (CompareComponent(this->revisionval.c_str(),
                                rightArg.revisionval.c_str()) == 0);
}

bool PkgVersion::operator<(const PkgVersion& rightArg){
    if(this->epochval != rightArg.epochval){
        return this->epochval < rightArg.epochval;
    }
    int result = CompareComponent(this->upStreamval.c_str(),
                                  rightArg.upStreamval.c_str());
    if(result){
        return -1 == result;
    }
    return -1 == CompareComponent(this->revisionval.c_str(),
                                  rightArg.revisionval.c_str());
}

bool PkgVersion::operator <=(const PkgVersion& rightArg){
    return (*this == rightArg)
           || (*this < rightArg);
}

bool PkgVersion::operator >(const PkgVersion& rightArg){
    if(this->epochval != rightArg.epochval){
        return this->epochval > rightArg.epochval;
    }
    int result = CompareComponent(this->upStreamval.c_str(),
                                  rightArg.upStreamval.c_str());
    if(result){
        return 1 == result;
    }
    return 1 == CompareComponent(this->revisionval.c_str(),
                                 rightArg.revisionval.c_str());
}

bool PkgVersion::operator >=(const PkgVersion& rightArg){
    return (*this == rightArg)
           || (*this > rightArg);
}

PkgVersion& PkgVersion::operator=(const PkgVersion& rightArg){
    this->wholeVersionString = rightArg.wholeVersionString;
    this->epochval = rightArg.epochval;
    this->upStreamval = rightArg.upStreamval;
    this->revisionval = rightArg.revisionval;
    return *this;
}


/*
 * From: http://codingcastles.blogspot.de/2009/05/comparing-version-numbers.html
 * sat. 01.23.16, 12:30
 * Compare two components of a Debian-style version.  Return -1, 0, or 1
 * if a is less than, equal to, or greater than b, respectively.
 */
int PkgVersion::CompareComponent(const char *a, const char *b){
    while (*a && *b) {
        while (*a && *b && !isdigit(*a) && !isdigit(*b)) {
            if (*a != *b) {
                if (*a == '~') return -1;
                if (*b == '~') return 1;
                return *a < *b ? -1 : 1;
            }
            a++;
            b++;
        }
        if (*a && *b && (!isdigit(*a) || !isdigit(*b))) {
            if (*a == '~') return -1;
            if (*b == '~') return 1;
            return isdigit(*a) ? -1 : 1;
        }

        char *next_a, *next_b;
        long int num_a = strtol(a, &next_a, 10);
        long int num_b = strtol(b, &next_b, 10);
        if (num_a != num_b) {
            return num_a < num_b ? -1 : 1;
        }
        a = next_a;
        b = next_b;
    }
    if (!*a && !*b) {
        return 0;
    } else if (*a) {
        return *a == '~' ? -1 : 1;
    } else {
        return *b == '~' ? 1 : -1;
    }
}

// trying to copy the above function as a newer std::String version
/*int PkgVersion::CompareComponent(std::string a, std::string b){
    std::string::iterator a_it = a.begin();
    std::string::iterator b_it = b.begin();
    while(a_it != a.end() && b_it != b.end()){
        while(a_it != a.end() && b_it != b.end()
              && !isdigit(*a_it) && !isdigit(*b_it)){
            if(*a_it != *b_it){
                if(*a_it == '~'){return -1;}
                if(*b_it == '~'){return 1;}
                return *a_it < *b_it ? -1 : 1;
            }
            a_it++;
            b_it++;
        }
        if(a_it != a.end() && b_it != b.end() && (!isdigit(*a_it)
           || !isdigit(*b_it))){
            if(*a_it == '~'){return -1;}
            if(*b_it == '~'){return 1;}
            return isdigit(*a_it) ? -1 : 1;
        }
        char* a_next,b_next;
        long int num_a = strtol(a.c_str(),&a_next,10);
        long int num_b = strtol(b.c_str(),&b_next,10);
        if(num_a != num_b){
            return num_a < num_b ? -1 : 1;
        }
        a_it = a_next;
        b_it = b_next;
    }
    if(a_it == a.end() && b_it == b.end()){
        return 0;
    }
    else if(a_it != a.end()){
        return (*a_it) == '~' ? -1 : 1;
    }
    else if(b_it != b.end()){
        return (*b_it) == '~' ? 1 : -1;
    }
}*/


std::ostream& operator <<(std::ostream& os , const PkgVersion& version){
    os << version.wholeVersionString;
    return os;
}
