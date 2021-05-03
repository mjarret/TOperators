import numpy as np
import math
import dyads
import time
import random
from random import shuffle

# Input: matrices A and B
# Output: True if Transpose(A).B is a signed permutation matrix and False otherwise
def areEquivalent(A,B):							
	for row1 in A:								# Computing transpose expensive, just work row by row
		tot = 0 							# Sum of absolute values of terms in a row
		for row2 in B:
			product = dot(row1,row2)						# Dot product of rows, assume this is reduced already
			if product.b != 0 or product.c != 0: return False			# First check to see if either b or c are nonzero, note that this relies on dp being reduced
												# c != 0 shouldn't hit if reduced and b == 0.
			tot += (product.a)**2							# Square of a in {0,1,-1} is in {0,1}
			if tot > 1: return False 						# Maximum 1 non-zero element
		if tot != 1: return False							# Failsafe in case of null sum. Shouldn't be needed.
	return True

# Input: two lists vec1 and vec2
# Output: vec1 . vec2
def dot(vec1,vec2):
	if len(vec1) != len(vec2):	raise Exception("Vectors of unequal length.")		# Removing this check will make things slightly faster and should be guaranteed by data types, doesn't seem to amount to much though
	ret = dyads.Z2(0,0,0)
	for i in range(len(vec1)): ret += vec1[i]*vec2[i]					# Sum elementwise products (don't use sum method, because need to generate a list)
	return ret
	
###### Test methods ########

def rand_Z2():
	return dyads.Z2(random.randrange(10),random.randrange(10),random.randrange(10))
	
def rand_Mat():
	return [[rand_Z2() for i in range(6)] for j in range(6)]
	
def rand_Perm():
	l = [i for i in range(6)]
	random.shuffle(l)
	return [[ dyads.Z2((random.choice([-1,1]))*int(l[i] == j),0,0) for i in range(6)] for j in range(6)]

def print_Mat(A):
	for row in A:
		for val in row:
			print '{:4}'.format(val),
		print

###### Main ########
	
def main():
	A = rand_Perm()
	B = rand_Perm()
	for row in A:
		for val in row:
			print '{:4}'.format(val),
		print
	for row in B:
		for val in row:
			print '{:4}'.format(val),
		print		
  	print(areEquivalent(A,B))
	
	return(0)
if __name__ == "__main__":
	main()	
	
	

