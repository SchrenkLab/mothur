//
//  optidata.hpp
//  Mothur
//
//  Created by Sarah Westcott on 5/10/18.
//  Copyright © 2018 Schloss Lab. All rights reserved.
//

#ifndef optidata_hpp
#define optidata_hpp

#include "mothurout.h"
#include "listvector.hpp"
#include "sparsedistancematrix.h"
#include "counttable.h"

class OptiData {
    
public:
    
    //OptiData()          { m = MothurOut::getInstance(); cutoff = 1.0; }
    OptiData(double c)  { m = MothurOut::getInstance(); cutoff = c; }
    //OptiData(string, string, double, bool); //distfile, distformat, cutoff, sim
    //OptiData(string, string, string, string, double, bool); //distfile, name or count, format, distformat, cutoff, sim
    virtual ~OptiData(){}
    
    set<int> getCloseSeqs(int i);// { return closeness[i]; }
    bool isClose(int, int);
    int getNumClose(int);
    map<string, int> getNameIndexMap();
    string getName(int); //name from nameMap index
    
    long long getNumSeqs() { return closeness.size(); }
    long long getNumSingletons() { return singletons.size(); }
    long long getNumDists(); //number of distances under cutoff
    ListVector* getListSingle();
    
    //for mgcluster - reading blast files
    virtual vector< set<int> > getBlastOverlap() { vector< set<int> > blank; return blank; }
    virtual string getOverlapName(int) { return ""; } //name from nameMap index
    
    virtual vector<int> getNumSeqs(vector<vector<string> >&, vector< vector<int> >&) { vector<int> temp; return temp;  }
    virtual long long getNumFitSingletons() { return 0; } //user singletons
    virtual long long getNumRefSingletons() { return 0; } //reference singletons
    
    virtual long long getNumFitDists() { return 0; } //user distances under cutoff
    virtual long long getNumRefDists() { return 0; } //ref distances under cutoff
    
    //virtual ListVector* getRefListSingle();
    virtual ListVector* getFitListSingle() { ListVector* list = NULL; return list; }
    
    virtual vector<int> getRefSeqs() { vector<int> temp; return temp;  }
    virtual vector<int> getFitSeqs() { vector<int> temp; return temp;  }
    
    virtual int getNumFitClose(int) { return 0;  }
    virtual int getNumRefClose(int) { return 0;  }
    virtual set<int> getCloseFitSeqs(int i) { set<int> temp; return temp;  }
    virtual set<int> getCloseRefSeqs(int i) { set<int> temp; return temp;  }
    virtual bool isCloseFit(int j, int i) { return false; }
    
protected:
    
    Utils util;
    MothurOut* m;
    vector< set<int> > closeness;  //closeness[0] contains indexes of seqs "close" to seq 0.
    vector<string> singletons;
    vector<string> nameMap;
    double cutoff;
    
    long int print(ostream&);
    
};


#endif /* optidata_hpp */