
import dyads
# import dyads2
import time
import random
from random import shuffle

# Input: matrices A and B
# Output: True if Transpose(A).B is a signed permutation matrix and False otherwise
def areEquivalent(A,B):							
	for row1 in A:														# Computing transpose expensive, just work row by row
		tot = 0 														# Sum of absolute values of terms in a row
		for row2 in B:
			product = dot(row1,row2)									# Dot product of rows, assume this is reduced already
			if product.b != 0 or product.c != 0: return False			# First check to see if either b or c are nonzero, note that this relies on dp being reduced
																		# c != 0 shouldn't hit if reduced and b == 0.
			tot += (product.a)**2										# Square of a in {0,1,-1} is in {0,1}
			if tot > 1: return False 									# Maximum 1 non-zero element
		if tot != 1: return False										# Failsafe in case of null sum. Shouldn't be needed.
	return True

# Input: matrices A and B
# Output: A.B
def matrix_multiply(A,B):
	rowsA = len(A)
	colsA = len(A[0])
	rowsB = len(B)
	colsB = len(B[0])
	
	C = [[dyads.Z2(0,0,0) for i in range(rowsA)] for j in range(colsB)]
	for i in range(rowsA):
		for j in range(colsB):
			for k in range(colsA):
				C[i][j] += A[i][k]*B[k][j]
	return C

# Input: two lists vec1 and vec2
# Output: vec1 . vec2
def dot(vec1,vec2):
	if len(vec1) != len(vec2):	raise Exception("Vectors of unequal length.")		# Removing this check will make things slightly faster and should be guaranteed by data types, doesn't seem to amount to much though
	ret = dyads.Z2(0,0,0)
	for i in range(len(vec1)): ret += vec1[i]*vec2[i]								# Sum elementwise products (don't use sum method, because need to generate a list)
	return ret

#############################
#		Test methods		#
#############################

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
		out = ''
		for x in row:
			out += str(x) + ' '
		print(out)
	return(0)

#############################
#			Main			#
#############################	
def main():
	a = dyads.Z2(14098400,42341098,8321)
	b = dyads.Z2(39481920,49276910,8340)
	start = time.time()
	for i in range(10000):	a+b
	t= time.time()-start
	print(t*1000)
	

if __name__ == "__main__":
	main()	
	
	

