import SALOMEDS__POA

class SALOME_DriverPy_i(SALOMEDS__POA.Driver):
    """
    """
    # class variable = default if instance variable is not found
    _ComponentDataType = None

    def __init__ (self, componentDataType):
        print "SALOME_DriverPy.__init__: ",componentDataType
        # instance variable : need self.
        self._ComponentDataType = componentDataType

    def IORToLocalPersistentID(self, theSObject, IORString, isMultiFile, isASCII):
        return theSObject.GetID()

    def LocalPersistentIDToIOR(self, theSObject, PersistentID, isMultiFile, isASCII):
        return ""

    def ComponentDataType(self):
        return _ComponentDataType

    def Save(self, theComponent, theURL, isMultiFile):
        return NULL

    def SaveASCII(self, theComponent, theURL, isMultiFile):
        return self.Save(theComponent, theURL, isMultiFile)

    def Load(self, theComponent, theStream, theURL, isMultiFile):
        return 1

    def LoadASCII(self, theComponent, theStream, theURL, isMultiFile):
        return self.Load(theComponent, theStream, theURL, isMultiFile)

    def Close(self, theComponent):
        pass

    def CanPublishInStudy(self, theIOR):
        return 1

    def PublishInStudy(self, theStudy, theSObject, theObject, theName):
        return NULL

    def CanCopy(self, theObject):
        return 0

