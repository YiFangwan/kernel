#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import unittest
from os import getcwd
from Engines import ContainerParameters, ResourceParameters
import SALOME
import salome

from time import sleep

class TestResourceManager(unittest.TestCase):
    def getContainer(self, name):
        rp = ResourceParameters(name="localhost",
                                hostname="localhost",
                                can_launch_batch_jobs=False,
                                can_run_containers=True,
                                OS="Linux",
                                componentList=[],
                                nb_proc=1,
                                mem_mb=1000,
                                cpu_clock=1000,
                                nb_node=1,
                                nb_proc_per_node=1,
                                policy="first",
                                resList=[])
        cp = ContainerParameters(container_name=name,
                                 mode="start",
                                 workingdir=getcwd(),
                                 nb_proc=1,
                                 isMPI=False,
                                 parallelLib="",
                                 resource_params=rp)
        cm = salome.naming_service.Resolve("/ContainerManager")
        return cm.GiveContainer(cp)

    def test1(self):
        cont = self.getContainer("test_container_1")
        cont.sendData(104550400) # 100 Mb
        cont.Shutdown()

    def test2(self):
        cont = self.getContainer("test_container_2")
        cont.sendData(2143483648) # ~ 2Gb
        cont.Shutdown()

    def test3(self):
        cont = self.getContainer("test_container_3")
        cont.sendData(2684354560) # 2.5 Gb
        cont.Shutdown()

if __name__ == '__main__':
    salome.standalone()
    salome.salome_init()
    unittest.main()
