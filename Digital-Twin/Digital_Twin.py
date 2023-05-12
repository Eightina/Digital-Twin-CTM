import CTMCoreImprove as ctm
from generate_init_file import generate_init_file
from generate_cons_file import generate_cons_file

# how to run javagui: java -jar "view.jar" Test4

# nsim = ctm.simulation("i_Test")
cell_scale = 2

generate_cons_file(
    cell_scale=cell_scale,
    file_name="Test4",
)

nsim = ctm.simulation("i_Test4")

generate_init_file(
    cell_scale=cell_scale,
    # seconds=cur_cycle_len * sim_cycle_num,
    offset=0,
    filename="Test4",
    # arc_demand=arc_demand,
    # phases_duration=cur_duration,
)
print("simulation geo defined")

# nsim.initialize("i_Test")
nsim.initialize("i_Test4")
print("simulation specs defined")

delay0 = nsim.execute()
print("test counting: 0 " + str(delay0))
nsim.output_result()
nsim.stepend()
print("test counting: 0 done")


# nsim.initialize("i_Test4")
# print('simulation specs defined')

# delay1 = nsim.execute()
# print('test counting: 1 '+ str(delay1))
# nsim.output_result()
# nsim.stepend()
# print('test counting: 1 done')


print("This is PYTHON")
