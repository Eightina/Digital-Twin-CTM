from typing import List


def generate_init_file(
    cell_scale: int = 1,  # in ctm, in update_origin_flow, tick scale is multiplied, so dont add that in
    seconds: int = 1800,
    arc_demand: List[int] = [300, 300, 300, 300],  # up1 down4 left5 right8
    turn_rate: dict = {"r": 0.2, "s": 0.7, "l": 0.1},
    phase_num: int = 4,
    phases: List[List] = [
        [
            [1, 3],
            [9, 19],
            [4, 2],
            [16, 22],
        ],
        [
            [10, 21],
            [9, 19],
            [15, 20],
            [16, 22],
        ],
        [
            [8, 6],
            [23, 12],
            [5, 7],
            [18, 13],
        ],
        [
            [24, 14],
            [23, 12],
            [17, 11],
            [18, 13],
        ],
    ],
    phases_duration: List[int] = [30, 14, 30, 14],
    offset=0,
    filename="Test4",
    # demand_coeff = 0.9,
):
    hours = int(seconds / 3600)
    minutes = int((seconds % 3600) / 60)
    seconds -= 3600 * hours + 60 * minutes

    demand_origin = [
        ["17", "1", "19"],
        ["30", "8", "32"],
        ["33", "9", "35"],
        ["46", "16", "48"],
    ]
    origin_type = [
        ["r", "s", "l"],
        ["l", "s", "r"],
        ["l", "s", "r"],
        ["r", "s", "l"],
    ]
    real_demand = []
    for i in range(4):
        # group_demand = demand_origin[i]
        group_type = origin_type[i]
        total_demand = arc_demand[i]
        for j in range(3):
            real_demand.append(
                "{:.4f}".format(
                    total_demand * turn_rate[group_type[j]] / 3600 / cell_scale / 10
                )
            )

    # print(real_demand)
    with open("Input/init_i_{}.txt".format(filename), "w") as f:
        f.write("traffic" + "\n")
        f.write(
            "time 00:00:00 {:0>2d}:{:0>2d}:{:0>2d}\n".format(hours, minutes, seconds)
        )
        for i in range(4):
            for j in range(3):
                f.write(
                    "demand\t00:00:00\t{}\t{}\n".format(
                        demand_origin[i][j], real_demand[i * 3 + j]
                    )
                )
        f.write("diverge\t9\t19\t1\t1\n")
        f.write("diverge\t1\t3\t1\t0\n")
        f.write("diverge\t10\t21\t1\t2\n")
        f.write("diverge\t16\t22\t1\t1\n")
        f.write("diverge\t4\t2\t1\t0\n")
        f.write("diverge\t15\t20\t1\t2\n")
        f.write("diverge\t18\t13\t1\t1\n")
        f.write("diverge\t5\t7\t1\t0\n")
        f.write("diverge\t17\t11\t1\t2\n")
        f.write("diverge\t23\t12\t1\t1\n")
        f.write("diverge\t8\t6\t1\t0\n")
        f.write("diverge\t24\t14\t1\t2\n")
        f.write("end\n\n")

        f.write("control\n")
        f.write(
            "intersection\t1\t1\t1150\t1150\t15\t60\t0\t{}\t2 4 7 5 10 12 13 15 18 20 22 24 25 27 29 31 34 36 38 40 41 43 45 47\n".format(
                phase_num
            )
        )
        for i in range(phase_num):
            for j in range(len(phases[0])):
                f.write(
                    "phase\t1\t{}\t{}\t{}\n".format(
                        i + 1, phases[i][j][0], phases[i][j][1]
                    )
                )
        for i in range(phase_num):
            f.write("duration\t{}\t{}\n".format(i + 1, phases_duration[i]))
        f.write("offset\t{}\n".format(offset))
        f.write("end\n")
