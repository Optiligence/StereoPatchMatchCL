class functor: 
	def __call__(self, a, b):
		return cmp(b,a);
array = [5,8,3,7,6,11,100];
print(array);
myOrdering = functor();
array.sort(myOrdering);
print(array);