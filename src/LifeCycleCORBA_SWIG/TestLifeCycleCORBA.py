
import Engines
import LifeCycleCORBA

lcc = LifeCycleCORBA.LifeCycleCORBA()

obj=lcc.FindOrLoad_Component("FactoryServer","SalomeTestComponent")
comp=obj._narrow(Engines.TestComponent)
comp.Coucou(1)

