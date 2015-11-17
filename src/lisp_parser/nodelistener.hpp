//---nodelistener.hpp---
#ifndef NODE_LISTENER_HPP
#define NODE_LISTENER_HPP

//---------------------------------------------------------
//Class Name: nodelistener
//
//Purpose: Interface for node update
//---------------------------------------------------------
class xnode;

class nodelistener{
   public:
      nodelistener(){}
      virtual ~nodelistener(){}
      virtual void nodeChange(xnode*)=0;
      virtual void nodeQueried(xnode*){}
};

#endif
