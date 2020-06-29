
TARGET:=test_mqtt
src += ./test-payloads.cpp
objs := $(addprefix $(./),$(src:.cpp=.o))
CXXFLAGS += -std=gnu++11
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SYSTEM_INC) $(INCLUDES) $(DEFINES) -MMD -MF $(basename $@).d -MP -o $@ -c $<
	# @g++ -std=gnu++11 -o $@ -c $<

$(TARGET) : $(objs)
	$(CXX) $(objs) -o $@ -lpthread
	@-rm *.o *.d

.phony:clean test
test: $(TARGET)
	# valgrind --leak-check=full ./$(TARGET)
	valgrind --leak-check=full ./$(TARGET)
clean:
	@-rm *.o *.d ${TARGET}


# objects:= addnum.o
# target:= addnum
# cc:=gcc

# # $(objects):
# all: $(objects)
# 	$(cc) -o $(target) $(objects) -lpthread
# 	@echo "make success"

# .PHONY: clean
# clean: 
# 	@-rm *.o $(target)