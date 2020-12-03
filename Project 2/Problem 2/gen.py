import itertools
# x = [range(48,58) + range(65,91) + range(97,123)]
x = list(range(35,39))								# Characters - #, $, %, & 
x += list(range(48,58))								# Characters - 0 to 9
x += list(range(64,91))								# Characters - @ to Z
x += list(range(97,123))							# Characters - a to z
chars = list(map(lambda x: chr(x), x))
print(chars)
print(len(chars))
for i in range(1,9):
	f = open("table" + str(i) + ".txt", "w")
	for p in itertools.product(chars, repeat=i):
		phr = "".join(p)
		f.write(phr+ "\n")