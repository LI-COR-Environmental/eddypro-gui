//---xnode.cpp---

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

#include "xnode.hpp"

//---------------------------------------------------------
//Class Name: xnode
//
//Purpose: Stores information relative to a tag in an xml document
//---------------------------------------------------------
xnode::xnode(std::string s, xnode* p)
   :
   name(s),
   value(""),
   //attribute(""),
   last(""),
   parent(p),
   allowEmptyValue(false),
   locked(false),
   permissions(RW),
   changed(false),
   node_queried(false),
   use_default_query(true)
{}

xnode::~xnode()
{
   std::list<xnode*>::const_iterator it=children.begin();
   while(it != children.end())
   {
      delete *it;
      ++it;
   }
}

//---------------------------------------------------------
//Method Name: printNode
//
//Purpose: Prints the context of the current node
//   ie <name>george</name>
//Precondition: Pass an ostream object to output the data to
//Postcondition: Returns void
//---------------------------------------------------------
void xnode::printNode(std::ostream& os) const
{

   if(permissions == R || permissions == RW)
   {
        std::list<xnode*>::const_iterator it=children.begin();
        os<<"("<<name<<" ";
        if(value!="")
            //os<<" "<<value;
            os<<value;
        while(it != children.end())
        {
            os<<(**it);
            ++it;
        }
        os<<")";
   }
}

//---------------------------------------------------------
//Method Name: getQuotedValue
//
//Purpose: Returns the node value enclosed in double quotes
//---------------------------------------------------------
std::string xnode::getQuotedValue()
{
    std::ostringstream os;
    os<<"\""<<value<<"\"";
    return os.str();
}

//---------------------------------------------------------
//Method Name: setValue
//
//Purpose: Set the value of the current node (alternative to writeToNode)
//Precondition: pass a string containing the value
//Postcondition: Returns void
//---------------------------------------------------------
void xnode::setValue(std::string val, bool ignore_perm)
{
    if(!locked || ignore_perm)
    {
        if(val == "?" && use_default_query)
        {
            node_queried = true;
        }
        else
        {
            value=val;
            changed=true;
            last=value;
        }
    }
    else
    {
        std::ostringstream os;
        os<<"The value of "<<name<<" is is read-only (locked) and can not be updated.";
        throw xmlError(os.str());
    }
}

void xnode::setValueNoNotify(std::string val)
{
    value=val;
    changed=true;
}

void xnode::setValueNoNotify(float val)
{
    std::ostringstream os;
    os<<val;
    value=os.str();
    changed=true;
}

void xnode::reset()
{
    value=last;
    changed=false;
}

//---------------------------------------------------------
//Method Name: getChild
//
//Purpose: Retrieves a child xnode* based upon the name
//Precondition: the child's name is passed as a string to the function.
//Postcondition: Returns a pointer to the child node if found.
//  Returns NULL otherwise
//---------------------------------------------------------
xnode* xnode::getChild(std::string childname)
{
   bool found=false;
   xnode* retval=0;
   std::list<xnode*>::const_iterator it=children.begin();
   while(it != children.end() && found==false)
   {
      if((*it)->getName() == childname)
      {
         found=true;
         retval=*it;
      }
      ++it;
   }
   return retval;
}

void xnode::setNodePermissions(int perm)
{
    if( perm != R && perm != W && perm != RW)
        throw xmlError("Invalid permission.");
    if(perm==R)
        locked=true;
    permissions=perm;
}

float xnode::getFloat() const
{
    float val=0.0;

    if(value !="")
    {
        std::istringstream is(value);
        is.exceptions(std::ios::failbit);

        try
        {
            is>>val;
        }
        catch(std::exception& e)
        {
            std::ostringstream os;
            os<<"Float conversion error.";
            std::cout << e.what() << std::endl;
            throw xmlError(os.str());
        }
    }

    return val;
}

int xnode::getInt() const
{
    int val=0;
    if(value !="")
    {
        std::istringstream is(value);
        is.exceptions(std::ios::failbit);

        try
        {
            is>>val;
        }
        catch(std::exception& e)
        {
            std::ostringstream os;
            os<<"Integer conversion error. "<<name;
            std::cout << e.what() << std::endl;
            throw xmlError(os.str());
        }
    }
    return val;
}

bool xnode::getBool() const
{
    bool retval=false;
    std::string val = value;
    std::transform(val.begin(), val.end(), val.begin(), toupper);

    if(val == "TRUE")
        retval=true;
    else if(val == "FALSE" || val=="")
        retval = false;
    else
    {
        std::ostringstream os;
        os<<"Boolean conversion error. "<<name;
        throw xmlError(os.str());
     }

    return retval;
}

void xnode::endTag()
{
    node_queried=false;
}

bool xnode::commit()
{
    bool retval=changed;
    changed=false;
    return retval;
}
