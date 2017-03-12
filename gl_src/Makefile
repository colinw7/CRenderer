CC = g++
AR = ar
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../gl_include
OBJ_DIR = ../obj
LIB_DIR = ../lib

all: $(LIB_DIR)/libCGLRenderer.a

SRC = \
CGLRenderer3D.cpp \
CGLRenderer2D.cpp \
CGLPixelRenderer.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-std=c++11 \
-I$(INC_DIR) \
-I../include \
-I../../CGL/app_include \
-I../../CGeometry3D/include \
-I../../CImageLib/include \
-I../../CFont/include \
-I../../CConfig/include \
-I../../CUtil/include \
-I../../CFile/include \
-I../../CFileUtil/include \
-I../../CMath/include \
-I../../COS/include \
-I../../CStrUtil/include \

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCGLRenderer.a

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

.SUFFIXES: .cpp

$(LIB_DIR)/libCGLRenderer.a: $(OBJS)
	$(AR) crv $(LIB_DIR)/libCGLRenderer.a $(OBJS)
