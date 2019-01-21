import pylauncher as pyla
jyf=pyla.Job_YACSFile()
jyf.setJobName("YDFX")
jyf.setWorkDirectory("/scratch/H87074/Example")
jyf.setLocalDirectory("/home/H87074/Example")
jyf.setResultDirectory("/home/H87074/Example")
jyf.setLauncherFile("")
jyf.setLauncherArgs("")
jyf.setJobFile("/tmp/EvalYFX_H87074_180119_093600.xml") # schema YACS
jyf.setPreCommand("")
jyf.setEnvFile("")
for elt in []:
    jyf.add_in_file(elt)
for elt in ["EvalYFX_H87074_180119_093600"]:
    jyf.add_out_file(elt)
jyf.setMaximumDuration("00:05")
jyf.setQueue("")
jyf.setPartition("")
jyf.setExclusive(False)
jyf.setMemPerCpu(0)
jyf.setWCKey("P11U5:CARBONES")
jyf.setExtraParams("")
#
rp=pyla.resourceParams()
rp.name = "athos"
rp.hostname = ""
rp.OS = "Linux"
rp.nb_proc = 5
rp.nb_node = 0
rp.nb_proc_per_node = 1
rp.cpu_clock = -1
rp.mem_mb = 0
jyf.setResourceRequiredParams(rp)
jyf.checkSpecificParameters()
l = pyla.Launcher_cpp()
res = pyla.ResourcesManager_cpp("/home/H87074/salome/V9_DEV/appli_V9_2_0/CatalogResources.xml")
l.SetResourcesManager(res)
l.createJob(jyf)
"""nb=jyf.getNumber()
l.launchJob(nb)
l.getJobState(nb)
l.getJobResults(nb,jyf.getLocalDirectory())"""
