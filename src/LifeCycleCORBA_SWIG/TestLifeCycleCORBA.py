
import Engines
import LifeCycleCORBA

lcc = LifeCycleCORBA.LifeCycleCORBA()

#obj=lcc.FindOrLoad_Component("FactoryServer","SalomeTestComponent")
#comp=obj._narrow(Engines.TestComponent)
#comp.Coucou(1)

param={}
param['hostname']='cli70ac'
param['container_name']='myContainer'
smesh=lcc.FindElseLoadComponent(param,'SMESH')

container=lcc.FindContainer('myContainer')
engine=lcc.FindComponent(param,'SMESH')
geom=lcc.LoadComponent(param,'GEOM')
