import os
import math
import random
def generate_linear_trace():
	with open('../../mahimahi/traces/trace_linear.up', 'w') as f:
		with open('../../mahimahi/traces/trace_linear.down', 'w') as f2:
			for i in xrange(140000):
				f.write(str(i) + '\n')
				f2.write(str(i) + '\n')
		f2.close()
	f.close()
	os.system('cp ../../mahimahi/traces/trace_linear.* /usr/local/share/mahimahi/traces/')

def generate_sine_trace():
	interval = 0.0
	with open('../../mahimahi/traces/trace_sine.up', 'w') as f:
		with open('../../mahimahi/traces/trace_sine.down', 'w') as f2:
			for i in xrange(560000):
				probability = (((math.sin(i/2000)) + 1) / 2) * 10000
				rand_num = random.randint(1,10000)
				if (rand_num <  probability):
					f.write(str(i/2) + '\n')
					f2.write(str(i/2) + '\n')
		f2.close()
	f.close()
	os.system('cp ../../mahimahi/traces/trace_sine.* /usr/local/share/mahimahi/traces/')



generate_sine_trace()