import batchmode_salome
import SALOME_ModuleCatalog

print "======================================================================"
print "           Get Catalog "
print "======================================================================"
obj = batchmode_salome.naming_service.Resolve('Kernel/ModulCatalog')
catalog = obj._narrow(SALOME_ModuleCatalog.ModuleCatalog)
catalog.GetComponentList()

C = catalog.GetComponent("AddComponent")
print C

print C.GetInterfaceList()

#I = C.GetInterface("Adder")
#print I

#I = C.GetInterface("AddComponent")
#print I
