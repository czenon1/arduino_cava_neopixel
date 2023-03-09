#!/bin/bash

SERIAL=${1:-/dev/ttyACM0}

sudo stty -F $SERIAL cs8 -cstopb -parenb 1000000
(cava cava -p ./config | sudo tee $SERIAL) >/dev/null
