import numpy as np
import math
import dyads
import time


# Input: matrices A and B
# Output: True if Transpose(A).B is a signed permutation matrix and False otherwise
def areEquivalent(A,B):
	for row1 in A:
		sum = 0 							# Sum of absolute values of terms in a row
		for row2 in B:
			dp = dot(row1,row2)					# Dot product of rows
			if dp.b != 0 or dp.c != 0: return False		# First check to see if either b or c are non zero, note that this relies on dp being reduced
			if dp.a not in [0,1,-1]: return False			# If other checks passed, this shouldn't hit, can probably remove with testing
			sum += a*a
			if sum > 1: return False 				# Maximum 1 non-zero element
		if sum != 1: return False					# Failsafe in case of null sum
	return True

def dot(a,b):
	if len(a) != len(b):	raise Exception("Vectors of unequal length.")		# Removing this check will make things faster and should be guaranteed by data types
	ret = dyads.Z2(0,0,0)
	for i in range(len(a)): ret += a[i]*b[i]					# Sum elementwise products (don't use sum method, because need to generate a list)
	return ret

def main():
	a = dyads.Z2(40,2,6)
	b = dyads.Z2(3,10,3)
	c = dyads.Z2(3,10,3)
	start = time.time()
	for i in range(100000): a-b
	print(time.time()-start)

if __name__ == "__main__":
	main()	
