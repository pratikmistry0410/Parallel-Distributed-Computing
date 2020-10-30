
TASK_CXX=../tasksys.cpp
TASK_LIB=-lpthread
TASK_OBJ=objectfiles/tasksys.o

CXX=clang++
CXXFLAGS+=-Iobjectfiles/ -O2
CC=clang
CCFLAGS+=-Iobjectfiles/ -O2

LIBS=-lm $(TASK_LIB) -lstdc++
ISPC=ispc
ISPC_FLAGS+=-O2
ISPC_HEADER=objectfiles/$(ISPC_SRC_FILE:.ispc=_ispc.h)

ARCH:=$(shell uname -m | sed -e s/x86_64/x86/ -e s/i686/x86/ -e s/arm.*/arm/ -e s/sa110/arm/)

ifeq ($(ARCH),x86)
  ISPC_OBJS=$(addprefix objectfiles/, $(ISPC_SRC_FILE:.ispc=)_ispc.o)
  COMMA=,
  ifneq (,$(findstring $(COMMA),$(ISPC_IA_TARGETS)))
    #$(info multi-target detected: $(ISPC_IA_TARGETS))
    ifneq (,$(findstring sse2,$(ISPC_IA_TARGETS)))
      ISPC_OBJS+=$(addprefix objectfiles/, $(ISPC_SRC_FILE:.ispc=)_ispc_sse2.o)
    endif
    ifneq (,$(findstring sse4,$(ISPC_IA_TARGETS)))
      ISPC_OBJS+=$(addprefix objectfiles/, $(ISPC_SRC_FILE:.ispc=)_ispc_sse4.o)
    endif
    ifneq (,$(findstring avx1-,$(ISPC_IA_TARGETS)))
      ISPC_OBJS+=$(addprefix objectfiles/, $(ISPC_SRC_FILE:.ispc=)_ispc_avx.o)
    endif
    ifneq (,$(findstring avx1.1,$(ISPC_IA_TARGETS)))
      ISPC_OBJS+=$(addprefix objectfiles/, $(ISPC_SRC_FILE:.ispc=)_ispc_avx11.o)
    endif
    ifneq (,$(findstring avx2,$(ISPC_IA_TARGETS)))
      ISPC_OBJS+=$(addprefix objectfiles/, $(ISPC_SRC_FILE:.ispc=)_ispc_avx2.o)
    endif
    ifneq (,$(findstring avx512knl,$(ISPC_IA_TARGETS)))
      ISPC_OBJS+=$(addprefix objectfiles/, $(ISPC_SRC_FILE:.ispc=)_ispc_avx512knl.o)
    endif
    ifneq (,$(findstring avx512skx,$(ISPC_IA_TARGETS)))
      ISPC_OBJS+=$(addprefix objectfiles/, $(ISPC_SRC_FILE:.ispc=)_ispc_avx512skx.o)
    endif
  endif
  ISPC_TARGETS=$(ISPC_IA_TARGETS)
  ARCH_BIT:=$(shell getconf LONG_BIT)
  ifeq ($(ARCH_BIT),32)
    ISPC_FLAGS += --arch=x86
    CXXFLAGS += -m32
    CCFLAGS += -m32
  else
    ISPC_FLAGS += --arch=x86-64
    CXXFLAGS += -m64
    CCFLAGS += -m64
  endif
else ifeq ($(ARCH),arm)
  ISPC_OBJS=$(addprefix objectfiles/, $(ISPC_SRC_FILE:.ispc=_ispc.o))
  ISPC_TARGETS=$(ISPC_ARM_TARGETS)
else
  $(error Unknown architecture $(ARCH) from uname -m)
endif

CPP_OBJS=$(addprefix objectfiles/, $(CPP_SRC:.cpp=.o))
CC_OBJS=$(addprefix objectfiles/, $(CC_SRC:.c=.o))
OBJS=$(CPP_OBJS) $(CC_OBJS) $(TASK_OBJ) $(ISPC_OBJS)

default: $(OUTPUT)

all: $(OUTPUT) $(OUTPUT)-sse4 $(OUTPUT)-generic16 $(OUTPUT)-scalar

.PHONY: dirs clean

dirs:
	/bin/mkdir -p objectfiles/

objectfiles/%.cpp objectfiles/%.o objectfiles/%.h: dirs

clean:
	/bin/rm -rf objs *~ $(OUTPUT) $(OUTPUT)-sse4 $(OUTPUT)-generic16 ref test

$(OUTPUT): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

objectfiles/%.o: %.cpp dirs $(ISPC_HEADER)
	$(CXX) $< $(CXXFLAGS) -c -o $@

objectfiles/%.o: %.c dirs $(ISPC_HEADER)
	$(CC) $< $(CCFLAGS) -c -o $@

objectfiles/%.o: ../%.cpp dirs
	$(CXX) $< $(CXXFLAGS) -c -o $@

objectfiles/$(OUTPUT).o: objectfiles/$(OUTPUT)_ispc.h dirs

objectfiles/%_ispc.h objectfiles/%_ispc.o objectfiles/%_ispc_sse2.o objectfiles/%_ispc_sse4.o objectfiles/%_ispc_avx.o objectfiles/%_ispc_avx11.o objectfiles/%_ispc_avx2.o objectfiles/%_ispc_avx512knl.o objectfiles/%_ispc_avx512skx.o : %.ispc dirs
	$(ISPC) $(ISPC_FLAGS) --target=$(ISPC_TARGETS) $< -o objectfiles/$*_ispc.o -h objectfiles/$*_ispc.h

objectfiles/$(ISPC_SRC_FILE:.ispc=)_sse4.cpp: $(ISPC_SRC_FILE)
	$(ISPC) $(ISPC_FLAGS) $< -o $@ --target=generic-4 --emit-c++ --c++-include-file=sse4.h

objectfiles/$(ISPC_SRC_FILE:.ispc=)_sse4.o: objectfiles/$(ISPC_SRC_FILE:.ispc=)_sse4.cpp
	$(CXX) -I../intrinsics -msse4.2 $< $(CXXFLAGS) -c -o $@

$(OUTPUT)-sse4: $(CPP_OBJS) objectfiles/$(ISPC_SRC_FILE:.ispc=)_sse4.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

objectfiles/$(ISPC_SRC_FILE:.ispc=)_generic16.cpp: $(ISPC_SRC_FILE)
	$(ISPC) $(ISPC_FLAGS) $< -o $@ --target=generic-16 --emit-c++ --c++-include-file=generic-16.h

objectfiles/$(ISPC_SRC_FILE:.ispc=)_generic16.o: objectfiles/$(ISPC_SRC_FILE:.ispc=)_generic16.cpp
	$(CXX) -I../intrinsics $< $(CXXFLAGS) -c -o $@

$(OUTPUT)-generic16: $(CPP_OBJS) objectfiles/$(ISPC_SRC_FILE:.ispc=)_generic16.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

objectfiles/$(ISPC_SRC_FILE:.ispc=)_scalar.o: $(ISPC_SRC_FILE)
	$(ISPC) $(ISPC_FLAGS) $< -o $@ --target=generic-1

$(OUTPUT)-scalar: $(CPP_OBJS) objectfiles/$(ISPC_SRC_FILE:.ispc=)_scalar.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
