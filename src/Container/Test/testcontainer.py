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

    def checkLoads(self, cont, loads):
        self.assertEqual(len(loads), cont.getNumberOfCPUCores())
        for load in loads:
            self.assertTrue(0.0 <= load <= 1.0)

    def test1(self):
        # Check loadOfCPUCores
        cont = self.getContainer("test_container_1")
        loads1 = cont.loadOfCPUCores()
        self.checkLoads(cont, loads1)
        sleep(1)
        loads2 = cont.loadOfCPUCores()
        self.checkLoads(cont, loads2)
        self.assertNotEqual(loads1, loads2)

    def test2(self):
        # Check custom script
        cont = self.getContainer("test_container_2")
        cont.setPyScriptForCPULoad('cpu_loads = [0.1, 0.2, 0.3, 0.4]')
        loads1 = cont.loadOfCPUCores()
        self.assertEqual(loads1, [0.1, 0.2, 0.3, 0.4])
        cont.resetScriptForCPULoad()
        loads2 = cont.loadOfCPUCores()
        self.checkLoads(cont, loads2)

    def test3(self):
        # Check bad script
        cont = self.getContainer("test_container_3")
        cont.setPyScriptForCPULoad("bla-bla-bla")
        self.assertRaises(Exception, cont.loadOfCPUCores)

    def test4(self):
        # check memory sizes
        cont = self.getContainer("test_container_4")
        memory_total = cont.getTotalPhysicalMemory()
        memory_in_use = cont.getTotalPhysicalMemoryInUse()
        memory_by_me = cont.getTotalPhysicalMemoryInUseByMe()
        self.assertGreater(memory_total, memory_in_use)
        self.assertGreater(memory_in_use, memory_by_me)


if __name__ == '__main__':
    salome.standalone()
    salome.salome_init()
    unittest.main()
