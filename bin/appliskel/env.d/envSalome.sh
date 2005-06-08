#!/bin/sh

# --- example to adapt to your own configuration ---
#
# --- envSalome.sh gives a set of MODULE_ROOT_DIR, to source
# --- you must at least define and export KERNEL_ROOT_DIR

REPINST=/home/prascle/SALOME2/Install

export KERNEL_ROOT_DIR=${REPINST}/KERNEL_V222_lc

export GEOM_ROOT_DIR=${REPINST}/GEOM_V222_lc
export MED_ROOT_DIR=${REPINST}/MED_V222_lc
export SMESH_ROOT_DIR=${REPINST}/SMESH_V222_lc
export SUPERV_ROOT_DIR=${REPINST}/SUPERV_V222_lc
export VISU_ROOT_DIR=${REPINST}/VISU_V222_lc
export NETGENPLUGIN_ROOT_DIR=${REPINST}/NETGENPLUGIN_V222_lc
export COMPONENT_ROOT_DIR=${REPINST}/COMPONENT_V222_lc
export PYCALCULATOR_ROOT_DIR=${REPINST}/PYCALCULATOR_V222_lc
export PYHELLO_ROOT_DIR=${REPINST}/PYHELLO_V222_lc
export PYTIX_ROOT_DIR=${REPINST}/PYTIX_V222_lc

