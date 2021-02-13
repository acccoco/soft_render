CPP_FLAGS     = -ggdb
C_FLAGS =
LD_FLAGS      =
LIBS         = -lm

TARGET  = main

# 文件列表
EXCLUDE_CPPS := our_gl.cpp test.cpp
ALL_CPPS := $(filter-out $(EXCLUDE_CPPS), $(wildcard *.cpp))
ALL_OBJS := $(patsubst %.cpp, %.o, $(ALL_CPPS))


# 编译链接
# =============================================================================
# 链接为可执行文件
all: $(ALL_OBJS)
	g++ -Wall $(LD_FLAGS) -o $(TARGET) $(ALL_OBJS) $(LIBS)


# 编译所有的 .cpp 文件为 .o 文件
# <target>: <target-pattern>: <req-pattern>
$(ALL_OBJS): %.o: %.cpp
	g++ -Wall $(CPP_FLAGS) -c $(C_FLAGS) $< -o $@


# 测试
# =============================================================================
test:
	g++ -Wall test.cpp -o test.exe
	./test.exe


# 清理
# =============================================================================
clean:
	-rm -f *.o
	-rm -f *.exe
	-rm -f *.tga

.PHONY: all clean test
