# importing =====================================================
import SALOMEDS
#import SALOMEDS_Attributes_idl
import geompy
import salome
# ===============================================================

# creating new object ===========================================
obj = geompy.myBuilder.NewObject(geompy.father)

# adding Name attribute =========================================
A1 = geompy.myBuilder.FindOrCreateAttribute(obj,"AttributeName")
name = A1._narrow(SALOMEDS.AttributeName)
name.SetValue("First Object")

# adding Comment attribute ======================================
A2 = geompy.myBuilder.FindOrCreateAttribute(obj,"AttributeComment")
comment = A2._narrow(SALOMEDS.AttributeComment)
comment.SetValue("This is Comment attribute")

# adding IOR attribute ==========================================
#A3 = geompy.myBuilder.FindOrCreateAttribute(obj,"AttributeIOR")
# ...

# adding integer attribute ======================================
A4 = geompy.myBuilder.FindOrCreateAttribute(obj,"AttributeInteger")
integer = A4._narrow(SALOMEDS.AttributeInteger)
integer.SetValue(12)

# adding real attribute =========================================
A5 = geompy.myBuilder.FindOrCreateAttribute(obj,"AttributeReal")
real = A5._narrow(SALOMEDS.AttributeReal)
real.SetValue(0.25)

# adding sequence of integer attribute ==========================
A6 = geompy.myBuilder.FindOrCreateAttribute(obj,"AttributeSequenceOfInteger")
intseq = A6._narrow(SALOMEDS.AttributeSequenceOfInteger)
intseq.Add(1)
intseq.Add(2)
intseq.Add(3)


# adding sequence of real attribute =============================
A7 = geompy.myBuilder.FindOrCreateAttribute(obj,"AttributeSequenceOfReal")
realseq = A7._narrow(SALOMEDS.AttributeSequenceOfReal)
realseq.Add(0.001)
realseq.Add(0.002)
realseq.Add(0.003)

# adding PersistentRef attribute ================================
#A8 = geompy.myBuilder.FindOrCreateAttribute(obj,"AttributePersistentRef")
# ...

# adding Drawable attribute =====================================
A9 = geompy.myBuilder.FindOrCreateAttribute(obj,"AttributeDrawable")
drawable = A9._narrow(SALOMEDS.AttributeDrawable)
drawable.SetDrawable(1)

# adding Selectable attribute ===================================
#A10 = geompy.myBuilder.FindOrCreateAttribute(obj,"AttributeSelectable")
# ...

# adding Expandable attribute ===================================
#A11 = geompy.myBuilder.FindOrCreateAttribute(obj,"AttributeExpandable")
# ...

# adding Opened attribute =======================================
#A12 = geompy.myBuilder.FindOrCreateAttribute(obj,"AttributeOpened")
# ...

# adding TextColor attribute ====================================
A13 = geompy.myBuilder.FindOrCreateAttribute(obj,"AttributeTextColor")
textcolor = A13._narrow(SALOMEDS.AttributeTextColor)

# ...

# adding TextHighlightColor of real attribute ===================
#A14 = geompy.myBuilder.FindOrCreateAttribute(obj,"AttributeTextHighlightColor")
# ...

# adding PixMap of real attribute ===============================
#A15 = geompy.myBuilder.FindOrCreateAttribute(obj,"AttributePixMap")
# ...

# opening OCAF Browser ==========================================
# result = salome.myStudy.OCAFBrowser()
