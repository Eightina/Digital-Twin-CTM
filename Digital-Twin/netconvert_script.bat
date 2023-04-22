netconvert --node-files=.\SUMO_Input\net\Test4.nod.xml ^
--edge-files=.\SUMO_Input\net\Test4.edg.xml ^
--type-files=.\SUMO_Input\net\Test4.typ.xml ^
--output-file=.\SUMO_Input\net\Test4.net.xml ^
--tllogic-files=.\SUMO_Input\net\Test4.additional.xml
netedit
@REM netedit --net-files=.\SUMO_Input\Test4.net.xml