#python netconvertor.py --node-files=.\SUMO_Input\hello.nod.xml --edge-files=.\SUMO_Input\hello.edg.xml --tllogic-files=.\SUMO_Input\hello.additional.xml --output-file=.\SUMO_Input\hello.net.xml

import sumo
import sys

netGenerator = sumo.netconvert()
print('-------------------------')
netGenerator()