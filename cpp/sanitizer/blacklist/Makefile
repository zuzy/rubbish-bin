all:
	@clang++ -fsanitize=address -fsanitize-blacklist=suppress.txt foo.cpp
test: all
	./a.out