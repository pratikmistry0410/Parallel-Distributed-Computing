import itertools
# x = [range(48,58) + range(65,91) + range(97,123)]
x = list(range(48,58))
x += list(range(65,91))
x += list(range(97,123))
chars = list(map(lambda x: chr(x), x))
print(chars)
print(len(chars))
for i in range(1,5):
	f = open("table" + str(i) + ".txt", "w")
	for p in itertools.product(chars, repeat=i):
		phr = "".join(p)
		f.write(phr+ "\n")