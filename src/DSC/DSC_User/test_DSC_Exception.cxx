#include "DSC_Exception.hxx"

//Compiler ce fichier en mode _DEBUG_ et sans _DEBUG_
class A {

public:
  DSC_EXCEPTION(Exp1);
  DSC_EXCEPTION(Exp2);

  A() {};
  virtual ~A() {};
  // La salome exception ne permet pas de passer une chaine ""
  void lanceException1_1() throw(Exp1) { throw Exp1("_");}
  void lanceException1_2() throw(Exp1) { throw Exp1("Ceci est l'exception 1_2");}
  void lanceException1_3() throw(Exp1) { throw Exp1(LOC("Ceci est l'exception 1_3"));}
  void lanceException1_4() throw(Exp1) { throw Exp1(LOC(OSS() << "Ceci est l'exeption 1_4" )); }
  void lanceException1_5() throw(Exp1) { 
    int a=1;
    throw Exp1(LOC(OSS() << "Ceci est l'exeption 1_5 avec la valeur A : " << a )); }
  void lanceException1_6() throw(Exp1) {
    Exp1 exp1(LOC(OSS() << "Ceci est l'exeption 1_6"));
    std::cout << "Affichage de exp1.what() dans lanceException1_6() " << exp1.what() << std::endl;
    throw Exp1(exp1); 
  }
  void lanceException1_7() throw(Exp1) {
    throw Exp1(LOC(OSS() << "Ceci est l'exeption 1_7"));
   }
};

DSC_EXCEPTION_CXX(A,Exp1);
DSC_EXCEPTION_CXX(A,Exp2);


int main() {

  A a;
  

  try {
    a.lanceException1_1();
  }
  catch (  const A::Exp1  & ex ) {
    std::cout << "Exception 1 bien reçue" << std::endl;
    std::cout << ex.what() << std::endl;
  } catch ( const DSC_Exception & dscE ) {
    std::cout << "Exception DSC  reçue mais aurait dû recevoir Exception 1" << std::endl;
  } catch ( ...) {
    std::cout << "Exception ... reçue mais aurait dû recevoir Exception 1" << std::endl;
  }

  try {
    a.lanceException1_2();
  }
  // Essai par valeur (ne pas faire çà !)
  catch (  A::Exp1  ex ) {
    std::cout << "Exception 1 bien reçue" << std::endl;
    std::cout << ex.what() << std::endl;
  } catch ( const DSC_Exception & dscE ) {
    std::cout << "Exception DSC  reçue mais aurait dû recevoir Exception 1" << std::endl;
  } catch ( ...) {
    std::cout << "Exception ... reçue mais aurait dû recevoir Exception 1" << std::endl;
  }

  try {
    a.lanceException1_3();
  }
  catch ( const A::Exp1 & ex ) {
    std::cout << "Exception 1 bien reçue" << std::endl;
    std::cout << ex.what() << std::endl;
  } catch ( const DSC_Exception & dscE ) {
    std::cout << "Exception DSC  reçue mais aurait dû recevoir Exception 1" << std::endl;
  } catch ( ...) {
    std::cout << "Exception ... reçue mais aurait dû recevoir Exception 1" << std::endl;
  }

  try {
    a.lanceException1_4();
  }
  catch ( const A::Exp1 & ex ) {
    std::cout << "Exception 1 bien reçue" << std::endl;
    std::cout << ex.what() << std::endl;
  } catch ( const DSC_Exception & dscE ) {
    std::cout << "Exception DSC  reçue mais aurait dû recevoir Exception 1" << std::endl;
  } catch ( ...) {
    std::cout << "Exception ... reçue mais aurait dû recevoir Exception 1" << std::endl;
  }

  try {
    a.lanceException1_5();
  }
  catch ( const A::Exp1 & ex ) {
    std::cout << "Exception 1 bien reçue" << std::endl;
    std::cout << ex.what() << std::endl;
  } catch ( const DSC_Exception & dscE ) {
    std::cout << "Exception DSC  reçue mais aurait dû recevoir Exception 1" << std::endl;
  } catch ( ...) {
    std::cout << "Exception ... reçue mais aurait dû recevoir Exception 1" << std::endl;
  }

  try {
    a.lanceException1_6();
  }
  catch ( SALOME_Exception ex ) {
    std::cout << "Exception SALOME bien reçue" << std::endl;
    std::cout << ex.what() << std::endl;
  } catch ( ...) {
    std::cout << "Exception ... reçue mais aurait dû recevoir Exception SALOME" << std::endl;
  }

  try {
    a.lanceException1_6();
  }
  catch ( const SALOME_Exception & ex ) {
    std::cout << "Exception SALOME bien reçue" << std::endl;
    std::cout << ex.what() << std::endl;
  } catch ( ...) {
    std::cout << "Exception ... reçue mais aurait dû recevoir Exception SALOME" << std::endl;
  }

  try {
    a.lanceException1_7();
  } catch ( const DSC_Exception & ex ) {
    std::cout << "Exception DSC bien reçue" << std::endl;
    std::cout << ex.what() << std::endl;
  } catch ( const SALOME_Exception & ex ) {
    std::cout << "Exception SALOME reçue mais aurais dû recevoir une exception DSC" << std::endl;
    std::cout << ex.what() << std::endl;
  } catch ( ...) {
    std::cout << "Exception ... reçue mais aurait dû recevoir Exception DSC" << std::endl;
  }

};
