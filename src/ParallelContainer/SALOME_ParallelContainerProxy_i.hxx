//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  SALOME_ParallelContainerProxy : implementation of container and engine for Parallel Kernel
//  File   : SALOME_ParallelContainerProxy_i.hxx
//  Author : André RIBES, EDF

#ifndef _SALOME_PARALLEL_CONTAINER_PROXY_I_HXX_
#define _SALOME_PARALLEL_CONTAINER_PROXY_I_HXX_

#include "utilities.h"
#include "SALOME_PACOExtensionPaCO_Engines_PACO_Container_server.hxx"
#include "SALOME_ParallelGlobalProcessVar_i.hxx"
#include "SALOME_NamingService.hxx"
#include <map>
#include <dlfcn.h>
#include <paco_omni.h>

class Container_proxy_impl_final :
  virtual public Engines::PACO_Container_proxy_impl,
  virtual public ParallelGlobalProcessVar_i
{
  public:
    Container_proxy_impl_final(CORBA::ORB_ptr orb, 
			       paco_fabrique_thread * fab_thread,
			       PortableServer::POA_ptr poa,
			       std::string containerName,
			       bool is_a_return_proxy = false);

    virtual ~Container_proxy_impl_final();

    virtual void Shutdown();

    virtual ::CORBA::Boolean load_component_Library(const char* componentName);
    virtual Engines::Component_ptr create_component_instance(const char* componentName, ::CORBA::Long studyId);

  private:
    std::map<std::string, std::string> _libtype_map; // libname -> libtype (seq ou par)
    typedef void (*PACO_TEST_FUNCTION) ();
    typedef PortableServer::ObjectId * (*FACTORY_FUNCTION) (CORBA::ORB_ptr,
							    paco_fabrique_thread *,
							    PortableServer::POA_ptr,
							    PortableServer::ObjectId *, 
							    const char *,
							    int);
    int _numInstance;
    std::string _containerName;
    PortableServer::POA_var _poa;
    PortableServer::ObjectId * _id;
    SALOME_NamingService *_NS;
};

#endif
