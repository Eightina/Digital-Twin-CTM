@REM INPUT: nod + edg&typ       --(netconvert)---------->   net
@REM        net                 --(generateTurnDefs.py)->   turndefs
@REM        net                 --(randomTrips.py)------>   trip          
@REM        turndefs + trip     --(jtrrouter)----------->   rou
@REM        net & rou           ------------------------>   cfg


@REM call "netconvert_script.bat"
@REM call "generateTurnDefs_script.bat"
@REM call "randomTrips_script.bat"
call "jtrrouter_script.bat"


sumo-gui -c .\SUMO_Input\Test4.sumocfg 
@REM -r 10000.rou.xml --time-to-teleport 1000 --no-warnings --no-step-log
@REM <sink edges="2 6 7 3"/>
@REM <edgeRelation from="1" to="3" probability="70"/>
@REM <edgeRelation from="1" to="6" probability="20"/>
@REM <edgeRelation from="1" to="7" probability="10"/>
@REM <edgeRelation from="2" to="1" probability="0"/>
@REM <edgeRelation from="3" to="4" probability="0"/>
@REM <edgeRelation from="4" to="2" probability="60"/>
@REM <edgeRelation from="4" to="6" probability="20"/>
@REM <edgeRelation from="4" to="7" probability="20"/>
@REM <edgeRelation from="5" to="2" probability="10"/>
@REM <edgeRelation from="5" to="3" probability="30"/>
@REM <edgeRelation from="5" to="7" probability="60"/>
@REM <edgeRelation from="6" to="5" probability="0"/>
@REM <edgeRelation from="7" to="8" probability="0"/>
@REM <edgeRelation from="8" to="2" probability="20"/>
@REM <edgeRelation from="8" to="3" probability="10"/>
@REM <edgeRelation from="8" to="6" probability="70"/>
@REM <sink edges="2 6 7 3"/>
