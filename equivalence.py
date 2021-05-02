import numpy as np
import math
import dyads
import time

# Input: matrices A and B
# Output: True if Transpose(A).B is a signed permutation matrix and False otherwise
def areEquivalent(A,B):
	for row1 in A:
		sum = 0 	# Sum of absolute values of terms in a row
		count = 0 	# Count number of non-zero elements		
		for row2 in B:
			dp = dot(row1,row2)				# Dot product of rows
			if dp !=0: 
				if dp not in [1,-1]: return False	# Reject anything that isn't in {0,1,-1}
				count += 1 				# Increase count
			if count > 1: return False 			# Maximum 1 non-zero element
	return True 

def dot(a,b):
	if len(a) != len(b):	raise Exception("Vectors of unequal length.")	
	ret = dyads.Z2(0,0,0)
	for i in range(len(a)):
		ret += a[i]*b[i]
	return ret

def recTime(s):
	start = time.time()
	s
	return time.time()-start

def main():
	a = dyads.Z2(40,2,6)
	b = dyads.Z2(3,10,3)
	start = time.time()
	for i in range(100000): a*b
	print(time.time()-start)
	
if __name__ == "__main__":
	main()	
