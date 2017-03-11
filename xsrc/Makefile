CC = g++
AR = ar
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib

all: $(LIB_DIR)/libCXRenderer.a

SRC = \
CXLibRenderer2D.cpp \
CXLibPixelRenderer.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-std=c++11 \
-I../xinclude \
-I../include \
-I../../CXLib/include \
-I../../CTimer/include \
-I../../CImageLib/include \
-I../../CFont/include \
-I../../CFile/include \
-I../../CMath/include \
-I../../CStrUtil/include \
-I../../COS/include \
-I../../CUtil/include \

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCXRenderer.a

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

.SUFFIXES: .cpp

$(LIB_DIR)/libCXRenderer.a: $(OBJS)
	$(AR) crv $(LIB_DIR)/libCXRenderer.a $(OBJS)
