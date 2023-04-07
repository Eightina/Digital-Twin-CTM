import CTMCoreImprove as ctm

# how to run javagui: java -jar "view.jar" Test4

nsim = ctm.simulation("i_Test")
#nsim = ctm.simulation("i_Test4")
print('simulation geo defined')

nsim.initialize("i_Test")
#nsim.initialize("i_Test4")
print('simulation specs defined')

delay0 = nsim.execute()
print('test counting: 0 '+ str(delay0))
nsim.output_result()
nsim.stepend()
print('test counting: 0 done')


nsim.initialize("i_Test")
#nsim.initialize("i_Test4")
print('simulation specs defined')

delay1 = nsim.execute()
print('test counting: 1 '+ str(delay1))
nsim.output_result()
nsim.stepend()
print('test counting: 1 done')


print('This is PYTHON')
