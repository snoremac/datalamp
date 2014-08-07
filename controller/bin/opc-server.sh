#!/bin/bash

EMOJIPXEL_HOME=$(dirname $0)/..
OPC_HOME=$HOME/software/arduino/openpixelcontrol

mkdir -p $EMOJIPXEL_HOME/build

$EMOJIPXEL_HOME/bin/layout $@ > $EMOJIPXEL_HOME/build/neomatrix-layout.json
$OPC_HOME/bin/gl_server $EMOJIPXEL_HOME/build/neomatrix-layout.json
