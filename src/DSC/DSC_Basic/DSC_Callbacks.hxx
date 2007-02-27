// André Ribes EDF R&D - 2006
//
#ifndef _DSC_CALLBACKS_HXX_
#define _DSC_CALLBACKS_HXX_

#include "DSC_Engines.hh"

/*! \class DSC_Callbacks
 *  \brief this is an abstract that defines the methods that the component
 *  uses to prevent the component user code that the state of the component has changed.
 *  Currently only port's connection modifications are signaled.
 */
class DSC_Callbacks
{
  public:
    virtual ~DSC_Callbacks() {}

    /*!
     * \brief This method is used by the component when the number of connection
     * on a provides port changes. This information helps the user code to detect
     * operation on his ports.
     * 
     *
     * \param provides_port_name the name of the provides name that has changed.
     * \param connection_nbr the new number of connection on the provides port.
     * \param message contains informations about the modification of the port.
     */
    virtual void provides_port_changed(const char* provides_port_name,
				       int connection_nbr,
				       const Engines::DSC::Message message) = 0;

    /*!
     * \brief This method is used by the component when the number of connection
     * on a uses port changes. This information helps the user code to detect
     * operation on his ports.
     * 
     *
     * \param uses_port_name the name of the uses name that has changed.
     * \param connection_nbr the new sequence representing the uses port.
     * \param message contains informations about the modification of the port.
     */
    virtual void uses_port_changed(const char* uses_port_name,
				   Engines::DSC::uses_port * new_uses_port,
				   const Engines::DSC::Message message) = 0;
};

#endif
