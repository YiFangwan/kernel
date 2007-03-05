// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate$
// Id          : $Id$

#include "calcium.h"
#include <stdlib.h>
#include <stdio.h>

// Interface C de SalomeCalcium 

typedef int InfoType;
typedef int bool;

// INTERFACES DE LECTURE


// Definition des méthodes calcium étendues 
// permettant le 0 copy.
//const char * nomvar
#define CALCIUM_EXT_LECT_INTERFACE_C_(_name,_timeType,_type,_typeName,_qual) \
  InfoType ecp_##_name (void * component, int mode,			\
			_timeType * ti, _timeType * tf, int * i,	\
		        char * nomvar, int bufferLength,		\
			int * nRead, _type _qual ** data ) {		\
    size_t _nRead;							\
    long   _i=*i;							\
    									\
    /*    std::cerr << "-------- CalciumInterface(C Part) MARK 1 ------------------" << std::endl; */ \
    InfoType info =  ecp_lecture_##_typeName (component, mode, ti, tf, &_i, \
					      nomvar, bufferLength, &_nRead, \
					      data );			\
    /*    std::cout << "-------- CalciumInterface(C Part), Valeur de data : " << std::endl; */ \
    /*    std::cout << "Ptr :" << *data << std::endl;				*/ \
    /*    for (int j=0; j<_nRead;++j)						*/ \
    /*      printf("--- Valeur de data[%d] : %d \n",j,(*data)[j]);		*/ \
    /*    std::cout << "Ptr :" << *data << std::endl;				*/ \
    /*									*/ \
    /*    std::cerr << "-------- CalciumInterface(C Part) MARK 2 ------------------" << std::endl; */ \
    *i = _i;								\
    *nRead=_nRead;							\
    /*    std::cerr << "-------- CalciumInterface(C Part) MARK 3 ------------------" << std::endl; */ \
									\
    return info;							\
  };									\
  void ecp_##_name##_free ( _type _qual * data) {			\
    ecp_lecture_##_typeName##_free(data);					\
  };

CALCIUM_EXT_LECT_INTERFACE_C_(len,float,int,int,);
CALCIUM_EXT_LECT_INTERFACE_C_(lre,float,float,float,);
CALCIUM_EXT_LECT_INTERFACE_C_(ldb,double,double,double,);
CALCIUM_EXT_LECT_INTERFACE_C_(llo,float,bool,bool,);
CALCIUM_EXT_LECT_INTERFACE_C_(lcp,float,float,cplx,);
#define STAR *
//  CALCIUM_EXT_LECT_INTERFACE_C_(lch,float,char,STAR[]); 
// La signature n'est pas la même pour les chaines de caractères il y a aussi 
// la taille des chaines

//const char * nomvar

#define CALCIUM_LECT_INTERFACE_C_(_name,_timeType,_type,_typeName,_qual)		\
  InfoType cp_##_name (void * component, int mode,			\
		       _timeType * ti, _timeType * tf, int * i,		\
		       char * nomvar, int bufferLength,			\
		       int * nRead, _type _qual * data ) {		\
    size_t _nRead;							\
    long   _i=*i;							\
    									\
    if ( (data == NULL) || (bufferLength < 1) ) return CPNTNULL;	\
    									\
    InfoType info =  ecp_lecture_##_typeName (component, mode, ti, tf, &_i,	\
					 nomvar, bufferLength, &_nRead, \
					 &data );			\
    *i = _i;								\
    *nRead=_nRead;							\
									\
    return info;							\
  };									\
  void cp_##_name##_free ( _type _qual * data) {			\
    ecp_lecture_##_typeName##_free(data);					\
  };


// Definition des méthodes calcium standard 

CALCIUM_LECT_INTERFACE_C_(len,float,int,int,);
CALCIUM_LECT_INTERFACE_C_(lre,float,float,float,);
CALCIUM_LECT_INTERFACE_C_(ldb,double,double,double,);
CALCIUM_LECT_INTERFACE_C_(llo,float,bool,bool,);
CALCIUM_LECT_INTERFACE_C_(lcp,float,float,cplx,);
#define STAR *
//  CALCIUM_LECT_INTERFACE_C_(lch,float,char,STAR);


// INTERFACES DE DÉBUT ET DE FIN DE COUPLAGE

InfoType cp_cd (void * component, char * instanceName) {
  /* TODO : Trouver le nom de l'instance SALOME*/
  if (instanceName) strcpy(instanceName,"UNDEFINED");
  return CPOK;
}
InfoType cp_fin (void * component, int code) {
  /* TODO : gérer avec les callbacks des ports DSC */
  
  InfoType info =  ecp_fin_(component,code);
  
  return info;
}



#define CALCIUM_ECR_INTERFACE_C_(_name,_timeType,_type,_typeName,_qual)	\
  InfoType cp_##_name (void * component, int mode,			\
		       _timeType t, int i,				\
		       char * nomvar, int nbelem,			\
		       _type _qual * data ) {				\
									\
    /*long   _i=i;*/							\
    if ( (data == NULL) || (nbelem < 1) ) return CPNTNULL;		\
    printf("cp_name : Valeur de nomvar %s\n",nomvar);	\
									\
    InfoType info =  ecp_ecriture_##_typeName (component, mode, &t, i,	\
					       nomvar, nbelem,		\
					       data );			\
									\
    return info;							\
  };									\


// Definition des méthodes calcium standard 

CALCIUM_ECR_INTERFACE_C_(een,float,int,int,);
CALCIUM_ECR_INTERFACE_C_(ere,float,float,float,);
CALCIUM_ECR_INTERFACE_C_(edb,double,double,double,);
CALCIUM_ECR_INTERFACE_C_(elo,float,bool,bool,);
CALCIUM_ECR_INTERFACE_C_(ecp,float,float,cplx,);
