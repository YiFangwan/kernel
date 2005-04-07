//  File   : SALOMEDS_AttributeTreeNode.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef SALOMEDS_AttributeTreeNode_HeaderFile
#define SALOMEDS_AttributeTreeNode_HeaderFile

#include "SALOMEDSClient_AttributeTreeNode.hxx"
#include "SALOMEDS_GenericAttribute.hxx"
#include "SALOMEDSImpl_AttributeTreeNode.hxx"

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

class SALOMEDS_AttributeTreeNode: public SALOMEDS_GenericAttribute, public SALOMEDSClient_AttributeTreeNode
{
public:  
  SALOMEDS_AttributeTreeNode(const Handle(SALOMEDSImpl_AttributeTreeNode)& theAttr);
  SALOMEDS_AttributeTreeNode(SALOMEDS::AttributeTreeNode_ptr theAttr);
  ~SALOMEDS_AttributeTreeNode();

  virtual void SetFather(SALOMEDSClient_AttributeTreeNode* value);
  virtual bool HasFather();
  virtual SALOMEDSClient_AttributeTreeNode* GetFather();
  virtual void SetPrevious(SALOMEDSClient_AttributeTreeNode* value);
  virtual bool HasPrevious();
  virtual SALOMEDSClient_AttributeTreeNode* GetPrevious();
  virtual void SetNext(SALOMEDSClient_AttributeTreeNode* value);
  virtual bool HasNext();
  virtual SALOMEDSClient_AttributeTreeNode* GetNext();
  virtual void SetFirst(SALOMEDSClient_AttributeTreeNode* value);
  virtual bool HasFirst();
  virtual SALOMEDSClient_AttributeTreeNode* GetFirst();
  virtual void SetTreeID(const std::string& value);
  virtual std::string GetTreeID();
  virtual void Append(SALOMEDSClient_AttributeTreeNode* value);
  virtual void Prepend(SALOMEDSClient_AttributeTreeNode* value);
  virtual void InsertBefore(SALOMEDSClient_AttributeTreeNode* value);
  virtual void InsertAfter(SALOMEDSClient_AttributeTreeNode* value);
  virtual void Remove();
  virtual int Depth();
  virtual bool IsRoot();
  virtual bool IsDescendant(SALOMEDSClient_AttributeTreeNode* value);
  virtual bool IsFather(SALOMEDSClient_AttributeTreeNode* value);
  virtual bool IsChild(SALOMEDSClient_AttributeTreeNode* value);
  virtual std::string Label();                                        
};

#endif
