from typing import List


def phase4webster(
    arc_demand: List[int] = [400, 400, 200, 200], # 1,4,5,8
    turn_rate: dict = {"r": 0.2, "s": 0.7, "l": 0.1},
    phases=[
        ["s", "r"],
        ["l", "r"],
        ["s", "r"],
        ["l", "r"],
    ],
):
    phase_num = len(arc_demand)
    A = 3  # 黄灯时间
    l = 5  # 每相位损失时间
    allred = 0  # 全红
    L = phase_num * l + allred
    Sb = 1500
    total_flow = sum(arc_demand)
    # y = [i / Sb for i in arc_demand]
    y = [
        max(
            [
                arc_demand[0] * turn_rate[phases[0][0]],
                arc_demand[0] * turn_rate[phases[0][1]],
                arc_demand[1] * turn_rate[phases[0][0]],
                arc_demand[1] * turn_rate[phases[0][1]],
            ]
        )
        / Sb,
        max(
            [
                arc_demand[0] * turn_rate[phases[1][0]],
                arc_demand[0] * turn_rate[phases[1][1]],
                arc_demand[1] * turn_rate[phases[1][0]],
                arc_demand[1] * turn_rate[phases[1][1]],
            ]
        )
        / Sb,
        max(
            [
                arc_demand[2] * turn_rate[phases[2][0]],
                arc_demand[2] * turn_rate[phases[2][1]],
                arc_demand[3] * turn_rate[phases[2][0]],
                arc_demand[3] * turn_rate[phases[2][1]],
            ]
        )
        / Sb,
        max(
            [
                arc_demand[2] * turn_rate[phases[3][0]],
                arc_demand[2] * turn_rate[phases[3][1]],
                arc_demand[3] * turn_rate[phases[3][0]],
                arc_demand[3] * turn_rate[phases[3][1]],
            ]
        )
        / Sb,
    ]
    Y = sum(y)
    print(Y)
    C0 = int((1.5 * L + 5) / (1 - Y))
    Ge = C0 - L
    print(C0)
    gei = [(Ge * y[i] / Y) for i in range(len(y))]
    # duration[i] =
    Gi = [gei[i] - A + l for i in range(len(gei))]
    return [round(_) for _ in Gi]