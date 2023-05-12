from typing import List


def generate_cons_file(
    clock: int = 2, 
    epsilon: float = 0.001,
    initoccp: float = 0,  
    initctrl : int = 0,
    cell_scale : int = 1,
    losttime : int = 4,
    file_name="Test4",
    # demand_coeff = 0.9,
):
    
    # print(real_demand)
    with open("Input/cons_i_{}.txt".format(file_name), "w") as f:
        f.write("setting" + "\n")
        f.write(
            "clock\t{}\n".format(clock)
        )
        f.write(
            "epsilon\t{}\n".format(epsilon)
        )
        f.write(
            "initoccp\t{}\n".format(initoccp)
        )
        f.write(
            "initctrl\t{}\n".format(initctrl)
        )
        f.write(
            "cellscale\t{}\n".format(cell_scale)
        )
        f.write(
            "losttime\t{}\n".format(losttime)
        )
        f.write("end\n\n")

        f.write("geometry\n")
        f.write("node        1        1        1100        2300\n")
        f.write("node        2        0        1100        1300\n") 
        f.write("node        3        2        1200        2300\n")
        f.write("node        4        0        1200        1300\n")
        f.write("node        5        0        1100        1000\n")
        f.write("node        6        2        1100        0\n")
        f.write("node        7        0        1200        1000\n")
        f.write("node        8        1        1200        0\n")
        f.write("node        9        1        0           1100\n")
        f.write("node       10        0        1000        1100\n")
        f.write("node       11        2        0           1200\n")
        f.write("node       12        0        1000        1200\n")
        f.write("node       13        0        1300        1100\n")
        f.write("node       14        2        2300        1100\n")
        f.write("node       15        0        1300        1200\n")
        f.write("node       16        1        2300        1200\n")
        f.write("node       17        1        1060        2300\n")
        f.write("node       18        0        1060        1300\n")
        f.write("node       19        1        1140        2300\n")
        f.write("node       20        0        1140        1300\n")
        f.write("node       21        2        1160        2300\n")
        f.write("node       22        0        1160        1300\n")
        f.write("node       23        2        1240        2300\n")
        f.write("node       24        0        1240        1300\n")
        f.write("node       25        0        1060        1000\n")
        f.write("node       26        2        1060        0\n")
        f.write("node       27        0        1140        1000\n")
        f.write("node       28        2        1140        0\n")
        f.write("node       29        0        1160        1000\n")
        f.write("node       30        1        1160        0\n")
        f.write("node       31        0        1240        1000\n")
        f.write("node       32        1        1240        0\n")
        f.write("node       33        1        0           1140\n")
        f.write("node       34        0        1000        1140\n")
        f.write("node       35        1        0           1060\n")
        f.write("node       36        0        1000        1060\n")
        f.write("node       37        2        0           1240\n")
        f.write("node       38        0        1000        1240\n")
        f.write("node       39        2        0           1160\n")
        f.write("node       40        0        1000        1160\n")
        f.write("node       41        0        1300        1140\n")
        f.write("node       42        2        2300        1140\n")
        f.write("node       43        0        1300        1060\n")
        f.write("node       44        2        2300        1060\n")
        f.write("node       45        0        1300        1240\n")
        f.write("node       46        1        2300        1240\n")
        f.write("node       47        0        1300        1160\n")
        f.write("node       48        1        2300        1160\n")
        
        f.write("arc        1         1        2       10.0     10.0       0.11      0.6\n")
        f.write("arc        2         4        3       10.0     10.0       0.11      0.6\n")
        f.write("arc        3         5        6       10.0     10.0       0.11      0.6\n")
        f.write("arc        4         8        7       10.0     10.0       0.11      0.6\n")
        f.write("arc        5         9       10       10.0     10.0       0.11      0.6\n")
        f.write("arc        6        12       11       10.0     10.0       0.11      0.6\n")
        f.write("arc        7        13       14       10.0     10.0       0.11      0.6\n")
        f.write("arc        8        16       15       10.0     10.0       0.11      0.6\n")
        f.write("arc        9        17       18       10.0     10.0       0.11      0.6\n")
        f.write("arc       10        19       20       10.0     10.0       0.11      0.6\n")
        f.write("arc       11        22       21       10.0     10.0       0.11      0.6\n")
        f.write("arc       12        24       23       10.0     10.0       0.11      0.6\n")
        f.write("arc       13        25       26       10.0     10.0       0.11      0.6\n")
        f.write("arc       14        27       28       10.0     10.0       0.11      0.6\n")
        f.write("arc       15        30       29       10.0     10.0       0.11      0.6\n")
        f.write("arc       16        32       31       10.0     10.0       0.11      0.6\n")
        f.write("arc       17        33       34       10.0     10.0       0.11      0.6\n")
        f.write("arc       18        35       36       10.0     10.0       0.11      0.6\n")
        f.write("arc       19        38       37       10.0     10.0       0.11      0.6\n")
        f.write("arc       20        40       39       10.0     10.0       0.11      0.6\n")
        f.write("arc       21        41       42       10.0     10.0       0.11      0.6\n")
        f.write("arc       22        43       44       10.0     10.0       0.11      0.6\n")
        f.write("arc       23        46       45       10.0     10.0       0.11      0.6\n")
        f.write("arc       24        48       47       10.0     10.0       0.11      0.6\n")
        
        f.write("end\n")
