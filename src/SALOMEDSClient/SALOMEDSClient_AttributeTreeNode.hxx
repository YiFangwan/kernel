//  File   : SALOMEDSClient_AttributeTreeNode.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef SALOMEDSClient_AttributeTreeNode_HeaderFile
#define SALOMEDSClient_AttributeTreeNode_HeaderFile

#include "SALOMEDSClient_GenericAttribute.hxx" 

class SALOMEDSClient_AttributeTreeNode
{
public:
  
  virtual void SetFather(SALOMEDSClient_AttributeTreeNode* value) = 0;
  virtual bool HasFather() = 0;
  virtual SALOMEDSClient_AttributeTreeNode* GetFather() = 0;
  virtual void SetPrevious(SALOMEDSClient_AttributeTreeNode* value) = 0;
  virtual bool HasPrevious() = 0;
  virtual SALOMEDSClient_AttributeTreeNode* GetPrevious() = 0;
  virtual void SetNext(SALOMEDSClient_AttributeTreeNode* value) = 0;
  virtual bool HasNext() = 0;
  virtual SALOMEDSClient_AttributeTreeNode* GetNext() = 0;
  virtual void SetFirst(SALOMEDSClient_AttributeTreeNode* value) = 0;
  virtual bool HasFirst() = 0;
  virtual SALOMEDSClient_AttributeTreeNode* GetFirst() = 0;
  virtual void SetTreeID(const char* value) = 0;
  virtual char* GetTreeID() = 0;
  virtual void Append(SALOMEDSClient_AttributeTreeNode* value) = 0;
  virtual void Prepend(SALOMEDSClient_AttributeTreeNode* value) = 0;
  virtual void InsertBefore(SALOMEDSClient_AttributeTreeNode* value) = 0;
  virtual void InsertAfter(SALOMEDSClient_AttributeTreeNode* value) = 0;
  virtual void Remove() = 0;
  virtual int Depth() = 0;
  virtual bool IsRoot() = 0;
  virtual bool IsDescendant(SALOMEDSClient_AttributeTreeNode* value) = 0;
  virtual bool IsFather(SALOMEDSClient_AttributeTreeNode* value) = 0;
  virtual bool IsChild(SALOMEDSClient_AttributeTreeNode* value) = 0;
  virtual char* Label() = 0;

};

#endif
