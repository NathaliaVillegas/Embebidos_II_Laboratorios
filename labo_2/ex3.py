import random
import time

while True:
	temp = random.uniform(0, 35)
	print(f"tempratura actual: {temp:.2f}")
	
	if temp<12:
		print("calentador on")
		print("ventitador off")
		
	elif temp>20:
		print("calentador off")
		print("ventitador on")
	
	else:
	 	print("calentador off")
		print("ventitador off")
		
time.sleep(3)
	
