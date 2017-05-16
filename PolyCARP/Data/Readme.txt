Added 5 files:

8 Vertex format:
boxesSwitchTower.txt	- Single switching tower with guy-wires, 30 boxes
boxes2Towers.txt	- Two steel lattice towers and conductors, 373 boxes
boxesKlondike.txt	- Subsection of Southern Company training facility, 5738 boxes
rotatedBoxesSwitchTower.txt	- Same as boxesSwitchTower.txt but boxes do not have flat tops and bottoms, 30 boxes

Quaternion Format:
rotatedBoxesSwitchTowerQuaternion.txt	- Same boxes as rotatedBoxesSwitchTower.txt but represented as Quaternion, position vector, and dimensions


Notes on the formats:
All coordinates are in relative meters (no Lat Long information present at this time)
In both formats, each box has its own line ('\n' delimiter)

8 Vertex format list:
	-each vertex is seperated by tabs
	-x,y,z for each vertex is comma seperated

Quaternion Format:
	-All elements are tab seperated
	-Order:
	<Quaternion X>	<Quaternion Y>	<Quaternion Z>	<Quaternion W>	<Position X>	<Position Y>	<Position Z>	<Width>	<Height>	<Depth>
