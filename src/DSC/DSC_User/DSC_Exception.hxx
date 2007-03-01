// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-02-06 10:44:23 +0100 (mar, 06 f√©v 2007) $
// Id          : $Id$

#ifndef DSC_EXCEPTION_HXX
#define DSC_EXCEPTION_HXX

#include "Utils_SALOME_Exception.hxx"
#include <string>
#include <iostream>
#include <sstream>
#include <memory>

#include "utilities.h"

#ifndef WNT
extern "C"
{
#endif
#include <string.h>
#ifndef WNT
}
#endif


#ifdef _DEBUG_
# ifdef __GNUC__
#  define LOC(message) (message), __FILE__ , __LINE__ , __FUNCTION__
# else
#  define LOC(message) (message), __FILE__, __LINE__
# endif
#else
# define LOC(message)  (message)
#endif


#ifndef SWIG
/**
 * Class OSS is useful when streaming data through a function
 * that expect a string as parameter
 */
class OSS
{
private:
  std::ostringstream oss_;

public:
  explicit OSS() : oss_() {}

  template <class T>
  OSS & operator<<(T obj)
  {
    oss_ << obj;
    return *this;
  }

  operator std::string()
  {
    return oss_.str();
  }

  // Surtout ne pas Ècrire le code suivant:
  // car oss_.str() renvoie une string temporaire
  //   operator const char*()
  //   {
  //     return oss_.str().c_str();
  //   }

}; /* end class OSS */
#endif


// Cette fonction provient de Utils_SALOME_Exception
// Solution pas trËs ÈlÈgante mais contrainte par les manques de la classe SALOME_Exception
const char *makeText( const char *text, const char *fileName, const unsigned int lineNumber);

struct DSC_Exception : public SALOME_Exception {

  // Attention, en cas de modification des paramËtres par dÈfaut
  // il est necessaire de les repporter dans la macro DSC_EXCEPTION ci-dessous
  // Le constructeur de la SALOME_Exception demande une chaine non vide
  // Du coup on est obliger de la dÈsallouer avant d'y mettre la notre
  // car le what n'est pas virtuel donc il faut que le contenu de SALOME_Exception::_text
  // soit utilisable.
  // Ne pas mettre lineNumber=0 ‡ cause du calcul log dans la SALOME_Exception si fileName est dÈfini
  DSC_Exception( const std::string & text, 
		 const char *fileName="", 
		 const unsigned int lineNumber=0, 
		 const char *funcName="" ):
    SALOME_Exception(text.c_str()) ,
    _dscText(text),
    _filefuncName(setFileFuncName(fileName?fileName:"",funcName?funcName:"")),
    _lineNumber(lineNumber),
    _exceptionName("DSC_Exception")
  {
    // Mise en cohÈrence avec l'exception SALOME (‡ revoir)
    delete [] ((char*)SALOME_Exception::_text);
    if (! _filefuncName.empty() )
      SALOME_Exception::_text = makeText(text.c_str(),_filefuncName.c_str(),lineNumber) ;
    else
      SALOME_Exception::_text = makeText(text.c_str(),0,lineNumber) ;
    
    OSS oss ;
    oss << _exceptionName ;
    if (!_filefuncName.empty() ) oss << " in " << _filefuncName;
    if (_lineNumber) oss << " [" << _lineNumber << "]";
    oss << " : " << _dscText;
    _what = oss;
  }

  virtual const char* what( void ) const throw ()
  {
    return _what.c_str()  ;
  }

  // L'opÈrateur = de SALOME_Exception n'est pas dÈfini
  // problËme potentiel concernant la recopie de son pointeur _text
    
  // Le destructeur de la SALOME_Exception devrait Ítre virtuel
  // sinon pb avec nos attributs de type pointeur.
  virtual ~DSC_Exception(void) throw() {};

private:

  std::string setFileFuncName(const char * fileName, const char * funcName) {
    ASSERT(fileName);
    ASSERT(funcName);
    OSS oss;
    if ( strcmp(fileName,"") )
      oss << fileName << "##" << funcName;
  
    return oss;
  };

  //DSC_Exception(void) {};
protected:
  std::string  _dscText;
  std::string  _filefuncName;
  std::string  _exceptionName;
  int          _lineNumber;
  std::string  _what;
};

#define DSC_EXCEPTION(Derived) struct Derived : public DSC_Exception { \
  Derived ( const std::string & text, const char *fileName="", const unsigned int lineNumber=0, const char *funcName="" \
	    ) : DSC_Exception(text,fileName,lineNumber,funcName) {	\
    _exceptionName = #Derived; \
  } \
    virtual ~Derived(void) throw();\
};\

//Sert ‡ eviter le problËme d'identification RTTI des exceptions
//CrÈe un unique typeInfo pour tous les bibliothËques composants SALOME
//dans un fichier cxx
#define DSC_EXCEPTION_CXX(NameSpace,Derived) NameSpace::Derived::~Derived(void) throw() {};

#endif /* DSC_EXCEPTION_HXX */
