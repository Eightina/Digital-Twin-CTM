from typing import List
import CTMCoreImprove as ctm
from generate_init_file import generate_init_file
from generate_cons_file import generate_cons_file
import numpy as np

def phase4webster(
    arc_demand: List[int] = [400, 400, 200, 200],  # 1,4,5,8
    turn_rate: dict = {"r": 0.2, "s": 0.7, "l": 0.1},
    phases=[
        ["s", "r"],
        ["l", "r"],
        ["s", "r"],
        ["l", "r"],
    ],
    fix=None,
    num=1,  # 奇数
):
    phase_num = len(arc_demand)
    A = 3  # 黄灯时间
    l = 4  # 每相位损失时间
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
    # print(Y)
    C0 = int((1.5 * L + 5) / (1 - Y))
    Ge = C0 - L
    if fix:
        Ge = fix
    # print(C0)
    if num > 1:
        unit = max(1, Ge // 30)
        halflen = num // 2
        backup_Ge = [Ge]
        backup_Ge.extend([Ge - i * unit for i in range(1, halflen + 1)])
        backup_Ge.extend([Ge + i * unit for i in range(1, halflen + 1)])
        backup_gei = [[(Ge * y[i] / Y) for i in range(len(y))] for Ge in backup_Ge]
        backup_Gi = [[gei[i] - A + l for i in range(len(gei))] for gei in backup_gei]
        return [[round(_) for _ in Gi] for Gi in backup_Gi]

    gei = [(Ge * y[i] / Y) for i in range(len(y))]
    # duration[i] =
    Gi = [gei[i] - A + l for i in range(len(gei))]

    return [round(_) for _ in Gi]


def best_duration(
    step_veh,
    arc_demand: List[int] = [400, 400, 200, 200],  # 1,4,5,8
    turn_rate: dict = {"r": 0.2, "s": 0.7, "l": 0.1},
    phases=[
        ["s", "r"],
        ["l", "r"],
        ["s", "r"],
        ["l", "r"],
    ],
    fix=None,
    num=1,
    cell_scale=1,
    ctm_coeff=44,
    sim_cycle_num = 1
):
    phase_num = len(phases)
    durations = phase4webster(arc_demand, turn_rate, phases, None, num)
    print(durations)
    res_delay = np.inf
    res_duration = []
    for cur_duration in durations:
        cur_cycle_len = round(sum(cur_duration) + phase_num * 4)
        generate_cons_file(
            cell_scale=cell_scale,
            file_name="Test4",
        )
        sim0 = ctm.simulation("i_Test4")
        generate_init_file(
            cell_scale=cell_scale,
            seconds=cur_cycle_len * sim_cycle_num,
            offset=0,
            filename="Test4",
            arc_demand=arc_demand,
            phases_duration=cur_duration,
        )
        sim0.initialize_with_occu("i_Test4", step_veh)
        delay0 = sim0.execute() * 10 / ctm_coeff
        sim0.output_result()
        sim0.stepend()
        if delay0 < res_delay:
            res_delay = delay0
            res_duration = cur_duration
    print(res_duration)
    return res_delay, res_duration
