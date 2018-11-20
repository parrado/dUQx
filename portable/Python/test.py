 # dUQx Python, test file
 #
 # This file is part of dUQx.
 #
 # Copyright (c) 2018 Alexander Lopez Parrado
 #
 # dUQx is free software: you can redistribute it and/or modify
 # it under the terms of the GNU Lesser General Public License as published by
 # the Free Software Foundation, either version 3 of the License, or
 # (at your option) any later version.
 #
 # dUQx is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU Lesser General Public License for more details.
 #
 # You should have received a copy of the GNU Lesser General Public License
 # along with dUQx.  If not, see <https://www.gnu.org/licenses/>.

import duqx
import time
import matplotlib.pyplot as plt

#Main methd
def main():
	#Inits dUQx
	duqx.init(24)
	#Digital port as input
	duqx.configure_digital(0)
	#Pull-ups enable
	duqx.write_digital(1023)
	#Vref estimation
	vref=duqx.calibrate_analog()

	print(vref)

	ch=14
	n=1000
	
	#Tests buffer mode runtime
	start_time = time.time()
	data=duqx.read_analog_buffer(ch,vref,n)
	end_time=time.time()
	sr=len(data)/(end_time-start_time)


	print("dUQx achieved a sample rate of "+str(sr/1000)+str(" kHz")+" when reading "+str(len(data))+" samples in buffer mode.")



	#Tests single sample runtime
	start_time = time.time()
	for i in range(n):
		v=duqx.read_analog_single(ch,vref)
	end_time=time.time()
	sr=n/(end_time-start_time)

	print("dUQx achieved a sample rate of "+str(sr/1000)+str(" kHz")+" when reading "+str(n)+" samples in single sample mode.")

	#Test output analog channels	
	duqx.write_analog(1.23,vref,0)
	duqx.write_analog(2.45,vref,1)

	#Closes dUQx
	duqx.end()
	
	#Plot data
	plt.plot(data)
	plt.show()


if __name__== "__main__":
	main()
