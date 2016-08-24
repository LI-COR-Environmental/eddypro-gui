//---xtree.hpp---

#ifndef XTREE_HPP
#define XTREE_HPP

#include "xnode.hpp"
#include <stack>
#include <sstream>

//---------------------------------------------------------
//Class Name: xtreefuncs
//
//Purpose:  Provide a base of common methods
//---------------------------------------------------------
class xtreefuncs
{
   public:
      xtreefuncs(){}
   protected:
      std::string getToken(std::istream& dtd, bool& end_node);
      std::string getValue(std::istream& dtd);
};

//---------------------------------------------------------
//Class Name: xtree
//
//Purpose:  The xml parsing tree.
//---------------------------------------------------------
class xtree : public xtreefuncs
{
   public:
      explicit xtree(xnode* xn):root(xn){}
      ~xtree(){delete root;}
      xnode* query(std::istream& xml);
      xnode* query(const char* xml);
      void parse(std::istream& xml, bool ignore_perm=true, bool ignore_extra_xml=false);
      void parse(const char* xml, bool ignore_perm=true, bool ignore_extra_xml=false);
      const xnode* rootNode(){return root;}
   private:
      xnode* root;
};

//---------------------------------------------------------
//Class Name: xtreefactory
//
//Purpose: will create a xml parsing tree based on a pseudo dtd
//ie <animal><dog><legs></legs></dog><cat><legs></legs></cat></animal>
//---------------------------------------------------------
class xtreefactory : public xtreefuncs
{
   public:
      xtreefactory();
      xtree* createTree(std::istream& dtd);
};

#endif
