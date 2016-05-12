import os
def generate_trace():
	with open('../../mahimahi/traces/trace_linear.up', 'w') as f:
		with open('../../mahimahi/traces/trace_linear.down', 'w') as f2:
			for i in xrange(140000):
				f.write(str(i) + '\n')
				f2.write(str(i) + '\n')
		f2.close()
	f.close()
	os.system('cp ../../mahimahi/traces/trace_linear.* /usr/local/share/mahimahi/traces/')

generate_trace()