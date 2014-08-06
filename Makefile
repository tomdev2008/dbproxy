include ../lightframe/Makefile_public

DBPROXYSERVER_DIR = $(BASE_DIR)/dbproxy

DBPROXYSERVER_INCLUDE_DIR = $(DBPROXYSERVER_DIR)

BLL_DIR = bll
DEF_DIR = def
CONFIG_DIR = config
DAL_DIR  = dal
MEMCACHE_DIR = memcache_dbproxy

DBPROXYSERVER_OBJ_DIR = $(DBPROXYSERVER_DIR)/.objs

DBPROXYSERVER_LDFLAGS = $(LDFLAGS) -ldl -lpthread -lmemcached

TARGET	= $(DEBUG_TARGET)

DEBUG_TARGET = $(BIN_DIR)/vdc_dbproxyserver$(BIN_SUFFIX)

DBPROXYSERVER_SRC = $(wildcard *.cpp)
DEF_SRC = $(wildcard $(DEF_DIR)/*.cpp)
BLL_SRC = $(wildcard $(BLL_DIR)/*.cpp)
CONFIG_SRC = $(wildcard $(CONFIG_DIR)/*.cpp)
DAL_SRC = $(wildcard $(DAL_DIR)/*.cpp)
MEMCACHE_SRC = $(wildcard $(MEMCACHE_DIR)/*.cpp)

DBPROXYSERVER_OBJS = $(addprefix $(DBPROXYSERVER_OBJ_DIR)/, $(subst .cpp,.o,$(DBPROXYSERVER_SRC)))

OBJS = $(wildcard $(DBPROXYSERVER_OBJ_DIR)/*.o)

INC = -I$(LIGHTFRAME_INCLUDE_DIR) -I$(DBPROXYSERVER_INCLUDE_DIR) -I$(MYSQL_INCLUDE_DIR) -I$(PUBLIC_INCLUDE_DIR)

all : $(TARGET)

$(TARGET) : $(DBPROXYSERVER_OBJS) COMMON LIGHTFRAME DEF BLL CONFIG DAL MEMCACHE_DBPROXY
	$(CXX)  -o $@ $(OBJS) $(DBPROXYSERVER_LDFLAGS)
$(DBPROXYSERVER_OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@
COMMON:
	cd $(COMMON_DIR); make
DEF:
	cd $(DEF_DIR); make
BLL:
	cd $(BLL_DIR); make
CONFIG:
	cd $(CONFIG_DIR); make
DAL:
	cd $(DAL_DIR); make
MEMCACHE_DBPROXY:
	cd $(MEMCACHE_DIR); make
LIGHTFRAME:
	cd $(LIGHTFRAME_DIR); make

clean: 
	cd $(COMMON_DIR); make clean;
	cd $(LIGHTFRAME_DIR); make clean;
	cd $(CONFIG_DIR); make clean;
	cd $(DEF_DIR); make clean;
	cd $(BLL_DIR); make clean;
	cd $(DAL_DIR); make clean;
	cd $(MEMCACHE_DIR); make clean;
	rm -f $(OBJS) $(TARGET)


