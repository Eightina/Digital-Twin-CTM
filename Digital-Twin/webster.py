from typing import List
import CTMCoreImprove as ctm
from generate_init_file import generate_init_file
from generate_cons_file import generate_cons_file
import numpy as np
from sko.GA import GA
import optuna


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
        Gi = []
        unit = max(1, Ge // 30)
        halflen = num // 2
        backup_Ge = [Ge]
        backup_Ge.extend([Ge - i * unit for i in range(1, halflen + 1)])
        backup_Ge.extend([Ge + i * unit for i in range(1, halflen + 1)])
        for ns in [0.8 , 0.9, 1.0, 1.1, 1.2]:
            for we in [0.8 , 0.9, 1.0, 1.1, 1.2]:
                temp_y = y.copy()
                ns_ration = temp_y[0] / temp_y[1] * ns
                we_ration = temp_y[2] / temp_y[3] * we
                temp_y = [
                    1 / (1 / ns_ration + 1) * (temp_y[0] + temp_y[1]),
                    1 / (ns_ration + 1) * (temp_y[0] + temp_y[1]),
                    1 / (1 / we_ration + 1) * (temp_y[2] + temp_y[3]),
                    1 / (we_ration + 1) * (temp_y[2] + temp_y[3]),
                ]
                backup_gei = [[(Ge * temp_y[i] / Y) for i in range(len(temp_y))] for Ge in backup_Ge]
                backup_Gi = [[gei[i] - A + l for i in range(len(gei))] for gei in backup_gei]
                Gi.extend([[round(_) for _ in Gi] for Gi in backup_Gi])
        # return [[round(_) for _ in Gi] for Gi in backup_Gi]
        return Gi

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
    # ctm_coeff=44,
    sim_cycle_num=1,
    file_name = "Test4"
):
    merge_scale = cell_scale * 2
    cell_length = merge_scale * 10
    phase_num = len(phases)
    durations = phase4webster(arc_demand, turn_rate, phases, None, num)
    # print(durations)
    res_delay = np.inf
    res_duration = []
    for cur_duration in durations:
        cur_cycle_len = round(sum(cur_duration) + phase_num * 4)
        generate_cons_file(
            cell_scale=cell_scale,
            file_name=file_name,
        )
        sim0 = ctm.simulation("i_{}".format(file_name))
        generate_init_file(
            cell_scale=cell_scale,
            seconds=cur_cycle_len * sim_cycle_num,
            offset=0,
            filename=file_name,
            arc_demand=arc_demand,
            phases_duration=cur_duration,
        )
        sim0.initialize_with_occu("i_{}".format(file_name), step_veh)
        # delay0 = sim0.execute() * 10 / ctm_coeff
        delay0 = sim0.execute()
        delay0 = (
            delay0
            * 10
            # )
            / (
                (sum(arc_demand) * cur_cycle_len * sim_cycle_num / 3600)
                + (sum(step_veh) * cell_length)
            )
        )
        print(cur_duration)
        print(delay0)
        sim0.output_result()
        sim0.stepend()
        if delay0 < res_delay:
            res_delay = delay0
            res_duration = cur_duration
    print(res_duration)
    # res_delay = (
    #     res_delay
    #     * 10
    #     # )
    #     / (
    #         (sum(arc_demand) * cur_cycle_len * sim_cycle_num / 3600)
    #         + (sum(step_veh) * cell_length)
    #     )
    # )
    print("ga_best_duration:", res_duration)
    return res_delay, res_duration


class blackbox_func:
    def __init__(
        self,
        step_veh,
        arc_demand: List[int] = [400, 400, 200, 200],  # 1,4,5,8
        cell_scale=1,
        # ctm_coeff=44,
        sim_cycle_num=1,
        file_name="Test4",
    ):
        self.step_veh = step_veh
        self.arc_demand = arc_demand
        self.A = 3  # 黄灯时间
        self.l = 4  # 每相位损失时间
        self.allred = 0  # 全红
        self.phase_num = len(arc_demand)
        self.L = self.phase_num * self.l + self.allred
        self.cell_scale = cell_scale
        self.sim_cycle_num = sim_cycle_num
        self.file_name = file_name
        # self.ctm_coeff = ctm_coeff

    def __call__(self, p):
        Ge = round(p[0])
        y = p[1:]
        Y = sum(y)

        gei = [(Ge * y[i] / Y) for i in range(len(y))]
        Gi = [gei[i] - self.A + self.l for i in range(len(gei))]
        duration = [round(_) for _ in Gi]
        phase_num = len(duration)
        cur_cycle_len = round(sum(duration) + phase_num * 4)
        cell_length = self.cell_scale * 2 * 10

        generate_cons_file(
            cell_scale=self.cell_scale,
            file_name=self.file_name,
        )
        sim0 = ctm.simulation("i_{}".format(self.file_name))
        generate_init_file(
            cell_scale=self.cell_scale,
            seconds=cur_cycle_len * self.sim_cycle_num,
            offset=0,
            filename=self.file_name,
            arc_demand=self.arc_demand,
            phases_duration=duration,
        )
        sim0.initialize_with_occu("i_{}".format(self.file_name), self.step_veh)
        delay0 = sim0.execute()
        # print(delay0)

        delay0 = (
            delay0
            * 10
            # )
            / (
                (sum(self.arc_demand) * cur_cycle_len * self.sim_cycle_num / 3600)
                + (sum(self.step_veh) * cell_length)
            )
        )
        sim0.output_result()
        sim0.stepend()
        del sim0
        print(duration)
        return delay0


def ga_best_duration(
    step_veh,
    arc_demand: List[int] = [400, 400, 200, 200],  # 1,4,5,8
    turn_rate: dict = {"r": 0.2, "s": 0.7, "l": 0.1},
    phases=[
        ["s", "r"],
        ["l", "r"],
        ["s", "r"],
        ["l", "r"],
    ],
    cell_scale=1,
    # ctm_coeff=44,
    sim_cycle_num=1,
    file_name="Test4",
    size_pop=10,
    max_iter=20,
):
    cell_length = cell_scale * 2 * 10
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
    C0 = int((1.5 * L + 5) / (1 - Y))
    Ge = C0 - L

    func = blackbox_func(step_veh, arc_demand, cell_scale, sim_cycle_num, file_name)
    upper_bound = [1.5 * Ge]
    upper_bound.extend([min(1, 1.5 * _) for _ in y])
    lower_bound = [Ge / 1.5]
    lower_bound.extend([_ / 1.5 for _ in y])
    print(Ge)
    print(lower_bound)
    print(upper_bound)
    ga = GA(
        func=func,
        n_dim=5,
        size_pop=size_pop,
        max_iter=max_iter,
        prob_mut=0.005,
        lb=lower_bound,
        ub=upper_bound,
        precision=[1, 1e-2, 1e-2, 1e-2, 1e-2],
    )
    res, delay0 = ga.run()
    print(res)

    res_Ge = int(res[0])
    res_y = list(res[1:])
    res_Y = sum(res_y)
    res_gei = [(res_Ge * res_y[i] / res_Y) for i in range(len(res_y))]
    res_Gi = [res_gei[i] - A + l for i in range(len(res_gei))]
    res_duration = [round(_) for _ in res_Gi]

    # cur_cycle_len = round(sum(res_duration) + phase_num * 4)
    # delay0 = (
    #     float(delay0)
    #     # / 10
    #     * (
    #         (sum(arc_demand) * cur_cycle_len * sim_cycle_num / 3600)
    #         + (sum(step_veh) * cell_length)
    #     )
    # )

    print("ga_best_duration:", res_duration)
    return delay0, res_duration


def bo_best_duration(
    step_veh,
    arc_demand: List[int] = [400, 400, 200, 200],  # 1,4,5,8
    turn_rate: dict = {"r": 0.2, "s": 0.7, "l": 0.1},
    phases=[
        ["s", "r"],
        ["l", "r"],
        ["s", "r"],
        ["l", "r"],
    ],
    cell_scale=1,
    # ctm_coeff=44,
    sim_cycle_num=1,
    file_name="Test4",
    max_iter=50,
    
):
    cell_length = cell_scale * 2 * 10
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
    C0 = int((1.5 * L + 5) / (1 - Y))
    Ge = C0 - L
    func = blackbox_func(step_veh, arc_demand, cell_scale, sim_cycle_num, file_name)
    upper_bound = [1.5 * Ge]
    upper_bound.extend([min(1, 1.5 * _) for _ in y])
    lower_bound = [Ge / 1.5]
    lower_bound.extend([_ / 1.5 for _ in y])

    def objective(trial):
        x = trial.suggest_int("x", int(lower_bound[0]), int(upper_bound[0]))
        y0 = trial.suggest_uniform("y0", lower_bound[1], upper_bound[1])
        y1 = trial.suggest_uniform("y1", lower_bound[2], upper_bound[2])
        y2 = trial.suggest_uniform("y2", lower_bound[3], upper_bound[3])
        y3 = trial.suggest_uniform("y3", lower_bound[4], upper_bound[4])
        return func([x, y0, y1, y2, y3])

    study = optuna.create_study(direction="minimize")
    study.optimize(objective, n_trials=max_iter)

    # print(study.best_params)
    # print(study.best_value)
    res = study.best_params
    delay0 = study.best_value
    # print(res)

    res_Ge = int(res['x'])
    res_y = [res['y0'], res['y1'], res['y2'], res['y3']]
    # res_y = res[1:]
    res_Y = sum(res_y)
    res_gei = [(res_Ge * res_y[i] / res_Y) for i in range(len(res_y))]
    res_Gi = [res_gei[i] - A + l for i in range(len(res_gei))]
    res_duration = [round(_) for _ in res_Gi]

    # cur_cycle_len = round(sum(res_duration) + phase_num * 4)
    # delay0 = (
    #     float(delay0)
    #     # / 10
    #     * (
    #         (sum(arc_demand) * cur_cycle_len * sim_cycle_num / 3600)
    #         + (sum(step_veh) * cell_length)
    #     )
    # )
    
    print("ga_best_duration:", res_duration)
    return study.best_value, res_duration