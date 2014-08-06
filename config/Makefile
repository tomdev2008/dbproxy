include ../../lightframe/Makefile_public

DBPROXYSERVER_DIR = $(BASE_DIR)/dbproxy

DBPROXYSERVER_INCLUDE_DIR = $(DBPROXYSERVER_DIR)

DBPROXYSERVER_OBJ_DIR = $(DBPROXYSERVER_DIR)/.objs

SRC = $(wildcard *.cpp)
OBJS = $(addprefix $(DBPROXYSERVER_OBJ_DIR)/, $(subst .cpp,.o,$(SRC)))

INC = -I$(LIGHTFRAME_INCLUDE_DIR) -I$(DBPROXYSERVER_INCLUDE_DIR) -I$(MYSQL_INCLUDE_DIR) -I$(PUBLIC_INCLUDE_DIR)

all : $(OBJS)

$(DBPROXYSERVER_OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJS)
