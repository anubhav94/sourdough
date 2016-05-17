import os
import math
import random
def generate_linear_trace():
	with open('../../mahimahi/traces/trace_linear_long.up', 'w') as f:
		with open('../../mahimahi/traces/trace_linear_long.down', 'w') as f2:
			for i in xrange(1400000):
				f.write(str(i) + '\n')
				f2.write(str(i) + '\n')
		f2.close()
	f.close()
	os.system('cp ../../mahimahi/traces/trace_linear_long.* /usr/local/share/mahimahi/traces/')

def generate_sine_trace():
	interval = 0.0
	with open('../../mahimahi/traces/trace_sine_long.up', 'w') as f:
		with open('../../mahimahi/traces/trace_sine_long.down', 'w') as f2:
			for i in xrange(5600000):
				probability = (((math.sin(i/2000)) + 1) / 2) * 10000
				rand_num = random.randint(1,10000)
				if (rand_num <  probability):
					f.write(str(i/2) + '\n')
					f2.write(str(i/2) + '\n')
		f2.close()
	f.close()
	os.system('cp ../../mahimahi/traces/trace_sine_long.* /usr/local/share/mahimahi/traces/')



generate_linear_trace()
