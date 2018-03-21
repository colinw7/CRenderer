CC = g++
AR = ar
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib

all: $(LIB_DIR)/libCRenderer.a

SRC = \
CGUIPixelRenderer.cpp \
CImagePixelRenderer.cpp \
CImageRenderer2D.cpp \
CPath2D.cpp \
CPixelClip.cpp \
CPixelRenderer.cpp \
CRenderer2D.cpp \
CRendererRegion2D.cpp \
CSymbol2D.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-std=c++14 \
-I. -I$(INC_DIR) \
-I../../CImageLib/include \
-I../../CFont/include \
-I../../CConfig/include \
-I../../CUtil/include \
-I../../CFile/include \
-I../../CMath/include \
-I../../COS/include \
-I../../CStrUtil/include \

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCRenderer.a

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

.SUFFIXES: .cpp

$(LIB_DIR)/libCRenderer.a: $(OBJS)
	$(AR) crv $(LIB_DIR)/libCRenderer.a $(OBJS)
