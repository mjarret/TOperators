import numpy as np
import math
import dyads
import time
import random
from random import shuffle


# Input: matrices A and B
# Output: True if Transpose(A).B is a signed permutation matrix and False otherwise
def areEquivalent(A,B):
	for row1 in A:
		tot = 0 							# Sum of absolute values of terms in a row
		for row2 in B:
			product = dot(row1,row2)						# Dot product of rows, assume this is reduced already
			print(product)
			if product.b != 0 or product.c != 0: return False			# First check to see if either b or c are non zero, note that this relies on dp being reduced
												# c != 0 shouldn't hit if reduced and b == 0. Should only need to check if b==0.
			tot += (product.a)**2							# Square of a in {0,1,-1} is in {0,1}
			if tot > 1: return False 						# Maximum 1 non-zero element
		if tot != 1: return False							# Failsafe in case of null sum
	return True

# Input: two lists vec1 and vec2
# Output: vec1 . vec2
def dot(vec1,vec2):
	if len(vec1) != len(vec2):	raise Exception("Vectors of unequal length.")		# Removing this check will make things slightly faster and should be guaranteed by data types
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
	return l

def print_Mat(A):
	for row in A:
		for val in row:
			print '{:4}'.format(val),
		print
	return(0)


###### Main ########
	
def main():
	A = rand_Mat()
	for row in A:
		for val in row:
			print '{:4}'.format(val),
		print
	print(A)
	print(rand_Perm())
	print(areEquivalent(rand_Mat(),rand_Mat()))
	return(0)
	a = dyads.Z2(-40,2,6)
	print(a)
	b = dyads.Z2(3,10,3)
	c = dyads.Z2(3,10,3)
	start = time.time()
	for i in range(1000): a+b
	print(time.time()-start)
	print(c == b)

if __name__ == "__main__":
	main()	
	
	

