# cpp/trans/dynamic/option-ignore-strict-aliasing
option := option
option_ignore-strict-aliasing := option-ignore-strict-aliasing


.PHONY: clean
all: dynamic dynamic_ignore-strict-aliasing dynamic_union

dynamic:dynamic.cc
	-@$(CXX) $(shell cat $(option)) $? -o $@

dynamic_ignore-strict-aliasing:dynamic.cc
	@$(CXX) $(shell cat $(option_ignore-strict-aliasing)) $? -o $@

dynamic_union:dynamic.cc
	@$(CXX) $(shell cat $(option)) -Duse_union $? -o $@

clean:
	@rm -rf *.o
	@rm -rf dynamic dynamic_ignore-strict-aliasing dynamic_union
	@rm -rf *.gc*
	@rm -rf *.out

