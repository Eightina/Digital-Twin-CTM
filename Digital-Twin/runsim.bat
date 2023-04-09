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