//---xnode.hpp---

#ifndef XNODE_HPP
#define XNODE_HPP

#include <iostream>
#include <list>
#include <string>

//---------------------------------------------------------
//Class Name: xmlError
//
//Purpose: Exception class
//---------------------------------------------------------
class xmlError
{
   public:
      explicit xmlError(std::string e) : error(e) {}
      std::string getError(){return error;}
   private:
      std::string error;
};

//---------------------------------------------------------
//Class Name: xnode
//
//Purpose: Stores information relative to a tag in an xml document
//---------------------------------------------------------

class xnode
{
    public:
        xnode(std::string s, xnode* p);
        ~xnode();

        std::string getValue(){return value;}
        std::string getQuotedValue();
        void setValue(std::string v, bool ignore_perm=true);
        void setValueNoNotify(std::string v);
        void setValueNoNotify(float v);

        std::string getName(){return name;}
        xnode* getParent(){return parent;}

        void addChild(xnode* xn){children.push_back(xn);}
        xnode* getChild(std::string childname);
        std::list<xnode*>& getChildren(){return children;}

        void allowEV(bool b){allowEmptyValue=b;}
        bool allowEV(){return allowEmptyValue;}

        void lockNode(bool lock){ locked=lock;}
        bool lockNode(){return locked;}
        void setNodePermissions(int perm);
        void reset();
        void printNode(std::ostream& os) const;
        void writeToNode(std::istream& is);
        bool commit();
        void endTag();

        float getFloat() const;
        int getInt() const;
        bool getBool() const;

        void useDefaultQuery(bool d){use_default_query=d;}
        bool useDefaultQuery(){return use_default_query;}

        enum {R,W,RW};

    private:
        std::string name;
        std::string value;
        std:: string last;
        xnode* parent;
        std::list<xnode*> children;
        bool allowEmptyValue;
        bool locked;
        int permissions;
        bool changed;
        bool node_queried; //node was queried with a ?

        bool use_default_query;

        friend std::ostream& operator << (std::ostream& os, const xnode& xn);
    };

inline std::ostream& operator << (std::ostream& os, const xnode& xn)
{
   xn.printNode(os);
   return os;
}

inline void operator << (float& val, const xnode& xn)
{
    val=xn.getFloat();
}

inline void operator << (int& val, const xnode& xn)
{
    val=xn.getInt();
}

#endif
