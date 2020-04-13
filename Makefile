INC_DIR:= ./inc
SRC_DIR:= ./src
BIN_DIR:= ./bin
SRCS:=$(wildcard $(SRC_DIR)/*.cc)
OBJS:=$(patsubst %.cc, %.o, $(SRCS))
LIBS:= -lpthread

CXX:=clang++

CXXFLAGS:= -W -O3 -g -std=c++1z $(addprefix -I, $(INC_RID)) $(LIBS)

EXE:= $(BIN_DIR)/Servo

$(EXE):$(OBJS)
	$(CXX) -o $(EXE) $(OBJS) $(CXXFLAGS)
%.o:%.c
	$(CXX) -c $< -o $@ $(CXXFLAGS)
clean:
	rm -rf $(EXE)
	rm -rf $(OBJS)
