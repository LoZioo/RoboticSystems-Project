clear

# Nel caso in cui uno dei comandi torni un valore diverso da zero, esci.
set -e

# Compilazione del sorgente.
~/.platformio/penv/Scripts/platformio run

# Script per caricare binario (hex) tramite avrdude utilizzando un HC-05.
# Direttamente da PlatformIO, utilizzando un HC-05, l'upload fallisce sempre.
# Compilare quindi il binario e caricarlo richiamando sempre questo script.

# Per l'aggiunta dell'eseguibile avrdude a $PATH (inserire nel .bashrc).
# export PATH	=	$PATH:"~/.platformio/packages/tool-avrdude":

# File di configurazione di avrdude creato da PlatformIO (path di Windows).
conf="C:\Users\lozio\.platformio\packages\tool-avrdude\avrdude.conf"

cpu="atmega328p"
env="nanoatmega328new"
com="COM4"
speed="115200"

# Caricamento del binario.
avrdude -v -p $cpu -C $conf -c arduino -b $speed -D -P $com -U flash:w:.pio/build/$env/firmware.hex:i

# Debug seriale.
~/.platformio/penv/Scripts/platformio device monitor --environment nanoatmega328new
