#!/bin/sh 

SALOME_ROOT=${HOME}/SALOME2
INSTALL_ROOT=${SALOME_ROOT}/Install

APPLI_ROOT=`pwd`

# --- clean appli
rm -rf bin lib share doc envd setAppliPath.sh searchFreePort.sh runAppli runConsole runSession env.d

# --- install appli

mkdir -p env.d
ln -fs bin/salome/appliskel/envd .
ln -fs bin/salome/appliskel/setAppliPath.sh .
ln -fs bin/salome/appliskel/searchFreePort.sh .
ln -fs bin/salome/appliskel/runAppli .
ln -fs bin/salome/appliskel/runConsole .
ln -fs bin/salome/appliskel/runSession .
ln -fs bin/salome/appliskel/.bashrc .

# --- prerequisites

if [ x${PREREQUISITE_SH} != x ]; then
  cp ${PREREQUISITE_SH} env.d/envProducts.sh;
else
  # --- unless PREREQUISITE_SH defines the prerequisite script, 
  #     edit and uncomment the following line to set it, and comment the echo and exit lines
  #ln -fs myPathForSalomePrerequisiteScriptToBeSourced env.d/envProducts.sh
  echo "The file to source for SALOME prerequisite definition is not known:"
  echo "--> Edit the corresponding line in " $0 ",Comment this message and the exit command,"
  exit 1;
fi

# --- symbolic links creation, from modules_root_dir

VERSION=BR_PR_V320b1

for module in KERNEL MED GUI GEOM SMESH VISU SUPERV LIGHT NETGENPLUGIN PYCALCULATOR PYTIX; 
do 
    echo " ========= " ${module};
    python virtual_salome.py -v --prefix="." --module=$INSTALL_ROOT/${module}_${VERSION}
    echo "export ${module}_ROOT_DIR=$APPLI_ROOT" >> env.d/configSalome.sh
done

# --- HELLO module

echo " =========  HELLO1";
python virtual_salome.py -v --prefix="." --module=$INSTALL_ROOT/HELLO1_${VERSION}
echo "export HELLO_ROOT_DIR=$APPLI_ROOT" >> env.d/configSalome.sh

# --- PYHELLO module

echo " ========= PYHELLO1";
python virtual_salome.py -v --prefix="." --module=$INSTALL_ROOT/PYHELLO1_${VERSION}
echo "export PYHELLO_ROOT_DIR=$APPLI_ROOT" >> env.d/configSalome.sh

# --- GUI config

echo "export SalomeAppConfig=$APPLI_ROOT:$APPLI_ROOT/share/salome/resources" >> env.d/configGUI.sh
echo "export SUITRoot=$APPLI_ROOT/share/salome" >> env.d/configGUI.sh

# --- 