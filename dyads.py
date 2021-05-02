class Z2:

	# Store everything as a 3-tuple (a,b,c) = (sign(a) sqrt{2}^{|a|} + sign(b) \sqrt{2}^{|b|})/\sqrt{2}^c where a is even, b is odd, c is an integer
	def __init__(self, intPart, rootPart, denom):
		self.a = intPart
		self.b = rootPart
		self.c = denom
		
	def __add__(self, other):
		a,b,c = scale(self.a,self.b,self.c,other.c)				# Scaling both terms is the same runtime as scaling a single term
		a1,b1,c = scale(other.a,other.b,other.c,self.c)
		a,b,c = reduce(a+a1,b+b1,c)						# Reduction step doubles runtime, but maybe helps in unseen ways, can be removed
		return(Z2(a,b,c))
		
	def __sub__(self, other):
		a,b,c = scale(self.a,self.b,self.c,other.c)				# Scaling both terms is the same runtime as scaling a single term
		a1,b1,c = scale(other.a,other.b,other.c,self.c)
		a,b,c = reduce(a-a1,b-b1,c)						# Reduction step doubles runtime, but maybe helps in unseen ways, can be removed
		return(Z2(a,b,c))

	def __neg__(self):
		return Z2(-self.a,-self.b,self.c)
		
	def __mul__(self, other):							# Multiplication is straightforard
		a = self.a*other.a + 2*self.b*other.b
		b = self.a*other.b + self.b*other.a
		c = self.c + other.c
		a,b,c = reduce(a,b,c)
		return(Z2(a,b,c))
	
	def __eq__(self, other):
		if isinstance(other, Z2):
			return (self.a == other.a and self.b == other.b and self.c == other.c)
		else if isinstance(other, list):
			return (self.a == list[0] and self.b == list[1] and self.c == list[2])
		return False
	
	def __str__(self):
		return '[%g, %g, %g]' % (self.a, self.b, self.c)
		
def scale(a,b,c,k):			
	while c < k: 			# Doing things as a loop works faster than computing how many times to do it
		b = a
		a = 2*b
		c += 1
	return(a,b,c)
	
def reduce(a,b,c):
	while a % 2 == 0 and c > 0: 
		a,b,c = b,a/2,c-1
	return(a,b,c)
