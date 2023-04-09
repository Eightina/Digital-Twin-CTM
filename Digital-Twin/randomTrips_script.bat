python "D:\programs\envs\python38\Lib\site-packages\sumo\tools\randomTrips.py" --net-file=.\SUMO_Input\net\Test4.net.xml ^
--jtrrouter ^
--output-trip-file .\SUMO_Input\route\Test4.trip.xml ^
--binomial 1 ^
--period 1.33 ^
--flows 4
@REM --insertion-rate 2700
@REM flows: generates INT flows that together output vehicles with the specified period
@REM group.add_argument("-p", "--period", type=str, nargs="+", metavar="FLOAT",
@REM                    action=sumolib.options.SplitAction,
@REM                    help="Generate vehicles with equidistant departure times and period=FLOAT (default 1.0). " +
@REM                    "If option --binomial is used, the expected arrival rate is set to 1/period.")
@REM op.add_argument("--binomial", type=int, metavar="N",
@REM                 help="If this is set, the number of departures per second will be drawn from a binomial " +
@REM                 "distribution with n=N and p=PERIOD/N where PERIOD is the argument given to --period")