#!/bin/bash

# Script para ejecutar los comandos en diferentes terminales

# Función para ejecutar un comando en una nueva terminal
function run_in_new_terminal {
  gnome-terminal -- bash -c "$1; exec bash"
}

# Ejecutar los scripts en diferentes terminales
run_in_new_terminal "./consola filesystem/filesystem_3"
run_in_new_terminal "./consola filesystem/filesystem_3"
run_in_new_terminal "./consola filesystem/filesystem_3"
run_in_new_terminal "./consola filesystem/filesystem_3"


