//---xtree.cpp---

#include "xtree.hpp"

//---------------------------------------------------------
//Class Name: xtreefuncs
//
//Purpose:  Provide a base of common methods
//---------------------------------------------------------

//---------------------------------------------------------
//Method Name: getToken
//Purpose:  Get the next token (tag name) in the xml stream
//Precondition: Pass a istream object to be separated into tokens
//Postcondition: Returns a string representing the next token
//---------------------------------------------------------
std::string xtreefuncs::getToken(std::istream& data, bool& end_node)
{
   end_node=false;
   std::ostringstream cur_token;

   char c;
   bool add=true;
   bool first_paren = false;

   while(add)
   {
      c = static_cast<char>(data.get());
      if(data.eof())
      {
         add=false;
      }
      else
      {
         switch(c)
         {
            case '(':
                if(!first_paren)
                    first_paren=true;
                else
                {
                    data.putback(c);
                    add=false;
                }
                break;
            case ')':
               add=false;
               end_node=true;
               break;

            case ' ':
                if(first_paren==true) //if you encounter a space after a (+node name you are done.
                {
                    add=false;
                }
                break;
            case '\r':
               break;
            default:
                if(c > 31 && c < 127)
                  cur_token<<c;
         }
      }
   }
   std::string g = cur_token.str();
   return g;
}
//---------------------------------------------------------
//Method Name: getValue

//Purpose:  Get the next value in the xml stream
//Precondition: Pass a istream object to be separated into values
//Postcondition: Returns a string representing the next value
//---------------------------------------------------------
std::string xtreefuncs::getValue(std::istream& data)
{
   std::string val;
   char c;
   bool add=true;
   while(add)
   {
      c = static_cast<char>(data.get());

      if(data.eof())
      {
         data.putback(c);
         add=false;
      }
      else
      {
         switch(c)
         {
            case '(':
            case ')':
               add=false;
               data.putback(c);
               break;
            //case ' ':
            case '\n':
            case '\t':
            case '\r':
            case '\"':
                break;
            default:
               if ((c > 31 && c < 127) || c == 6)
                  val += c;
         }
      }
   }
   return val;
}

//---------------------------------------------------------
//Class Name: xtree
//
//Purpose:  The xml parsing tree.
//---------------------------------------------------------

//---------------------------------------------------------
//Method Name: parse
//
//Purpose:  Populate the xml tree with data
//Precondition: Pass a istream object to be parsed into the current xml tree
//PostconditionL  Returns void
//---------------------------------------------------------

void xtree::parse(std::istream& xml, bool ignore_perm, bool ignore_extra_xml)
{
    xnode* currentnode=root;
    xnode* tnode=0;
    std::ostringstream log;
    std::string token;
    std::string value;
    std::stack<xnode*> node_stack;
    bool end_node;

    std::ostringstream currentstate;
    currentstate<<*root;

    while(!xml.eof())
    {
        token=getToken(xml, end_node);
        if(end_node)
        {
            currentnode->endTag();
            if(token == "")
            {
                if(!node_stack.empty())
                {
                    node_stack.pop();
                    if(!node_stack.empty())
                        currentnode=node_stack.top();
                }
            }

        }
        else if(!xml.eof())
        {
            log<<token<<" ";

            std::string x = currentnode->getName();

            if(token == x)
            {
                node_stack.push(currentnode);
            }
            else
            {
                tnode=currentnode->getChild(token);
                if(tnode)
                {
                    currentnode=tnode;
                    node_stack.push(currentnode);
                    value=getValue(xml);

                    if (value != "" || currentnode->allowEV())
                        currentnode->setValue(value, ignore_perm);
                }
                else
                {

                    bool found = false;
                    if(ignore_extra_xml)
                    {
                        int pcount = 1;
                        while(!found && !xml.eof())
                        {
                            char c = static_cast<char>(xml.get());
                            if(c=='(')
                                pcount++;
                            else if(c==')')
                                pcount--;

                            if(pcount == 0)
                                found = true;

                        }
                    }

                    if(!found)
                    {
                        std::ostringstream os;
                        os<<"Parse Error: "<<token<<" in :"<<log.str();
                        throw xmlError(os.str());
                     }
                }
            }
        }
    }
}

void xtree::parse(const char* xml, bool ignore_perm, bool ignore_extra_xml)
{
    std::istringstream is(xml);
    parse(is, ignore_perm, ignore_extra_xml);
}


//---------------------------------------------------------
//Method Name: query
//
//Purpose:  Query a specific node in the xml document
//Precondition: Pass a istream object that represents the node you are looking for.
//   ie <animal><dog><feet>
//Post Condition: This will return an xnode* to the 'feet' xnode object
//---------------------------------------------------------

xnode* xtree::query(std::istream& xml)
{
    xnode* currentnode=root;
    xnode* tnode=0;
    std::ostringstream log;
    std::string token;
    std::stack<xnode*> node_stack;
    bool end_node;

    while(!xml.eof())
    {
        token=getToken(xml, end_node);

        if(!xml.eof())
        {
            log<<token<<" ";
            if(token == currentnode->getName())
            {
                node_stack.push(currentnode);
            }
            else
            {
                tnode=currentnode->getChild(token);
                if(tnode)
                {
                    currentnode=tnode;
                    node_stack.push(currentnode);
                }
                else
                {
                    std::ostringstream os;
                    os<<"Could not process query: "<<token<<" in :"<<log.str();
                    throw xmlError(os.str());
                }
            }
        }
        if(end_node)
        {
            node_stack.pop();
        }
    }
    return currentnode;
}

xnode* xtree::query(const char* xml)
{
    std::istringstream is(xml);
    return query(is);
}

//---------------------------------------------------------
//Class Name: xtreefactory
//
//Purpose: will create a xml parsing tree based on a pseudo dtd
//   ie <animal><dog><legs></legs></dog><cat><legs></legs></cat></animal>
//---------------------------------------------------------
xtreefactory::xtreefactory()
{}

//---------------------------------------------------------
//Method Name: createTree()
//Purpose: Create a new xtree based upon the pseudo dtd
//Precondition:  Must pass a istream object representing the pseudo dtd
//Postcondition:  Returns a pointer to a new xtree
//---------------------------------------------------------
xtree* xtreefactory::createTree(std::istream& dtd)
{
    xnode* node=0;
    xnode* root=0;
    std::string token;
    bool end_node=false;
    std::stack<xnode*> node_stack;
    while(!dtd.eof())
    {
        token=getToken(dtd, end_node);

        if(!dtd.eof() && token != "")
        {
            int perm=xnode::RW;
            if(node_stack.empty())
            {
                node=new xnode(token,0);
                node->setNodePermissions(perm);
                node_stack.push(node);
                root=node;
            }
            else
            {
                xnode* tn=node_stack.top();
                node=new xnode(token,tn);
                node->setNodePermissions(perm);

                tn->addChild(node);
                node_stack.push(node);
            }
        }
        if(end_node)
            node_stack.pop();
    }

    return new xtree(root);
}
