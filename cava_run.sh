#!/bin/bash

SERIAL=${1:-/dev/ttyACM0}

# Configuration du port série à 1 000 000 bauds
sudo stty -F $SERIAL cs8 -cstopb -parenb 1000000

# Capture des fréquences avec cava et envoie vers l'Arduino
(cava -p ./config | sudo tee $SERIAL) >/dev/null
