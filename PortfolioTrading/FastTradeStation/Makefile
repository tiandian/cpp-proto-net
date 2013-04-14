CC = g++
CFLAGS=#-Wall
PROTO_DIR = ./proto
SRC = ./src
ENTITY_DIR = ./src/entity
AP_DIR = ./src/ap_invoke
OBJ_DIR = ./obj
LIBS = -lboost_thread -lboost_program_options -lboost_filesystem -lboost_chrono \
 -lboost_date_time -lprotobuf -llog4cpp -lthostmduserapi -lthosttraderapi

_ENTITY_FILES = packet enum quote trade message  
ENTITY_FILES = $(patsubst %, $(ENTITY_DIR)/%.pb.cc, $(_ENTITY_FILES))
ENTITY_OBJS = $(patsubst %, $(OBJ_DIR)/%.pb.o, $(_ENTITY_FILES))

_AP_INVOKE_OBJS = server.o apsessionmanager.o
AP_INVOKE_OBJS = $(patsubst %, $(OBJ_DIR)/%, $(_AP_INVOKE_OBJS))

_FS_OBJS = ArbitrageStrategy AsyncScalperEventFirer ChangePositionStrategy \
	   charsetconvert ClientAgent ClientManager ClientMessagePump \
	   Configuration FileOperations InputOrder LogManager \
	   orderhelper OrderProcessor2 Portfolio PortfolioManager \
	   PortfolioOrderHelper PortfolioOrderPlacer PortfolioScalperOrderPlacer \
	   QuoteAgent QuoteFetcher QuoteRepositry QuoteStore \
	   ScalperStrategy ScalperStrategy2 Strategy \
	   StrategyFactory SymbolInfo SymbolInfoRepositry TradeAgent \
	   TradeStationMain
FS_OBJS = $(patsubst %, $(OBJ_DIR)/%.o, $(_FS_OBJS))

fast-trade-stion: $(ENTITY_OBJS) $(AP_INVOKE_OBJS) $(FS_OBJS)
	$(CC) -o FastTradeStation $^ $(LIBS) -Wall

# Compile entity
$(OBJ_DIR)/%.pb.o: $(ENTITY_DIR)/%.pb.cc
	$(CC) -c -o $@ $< $(CFLAGS)

# Compile ap_invoke
$(OBJ_DIR)/server.o: $(AP_DIR)/server.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ_DIR)/apsessionmanager.o: $(AP_DIR)/apsessionmanager.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

# Compile fast trade station main cpp
$(OBJ_DIR)/%.o: $(SRC)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS) 

# Generate proto .h and .cc
$(ENTITY_DIR)/packet.pb.cc: $(PROTO_DIR)/packet.proto
	protoc -I=$(PROTO_DIR) --cpp_out=$(ENTITY_DIR) $<

$(ENTITY_DIR)/enum.pb.cc: $(PROTO_DIR)/enum.proto
	protoc -I=$(PROTO_DIR) --cpp_out=$(ENTITY_DIR) $<

$(ENTITY_DIR)/message.pb.cc: $(PROTO_DIR)/message.proto
	protoc -I=$(PROTO_DIR) --cpp_out=$(ENTITY_DIR) $<

$(ENTITY_DIR)/quote.pb.cc: $(PROTO_DIR)/quote.proto
	protoc -I=$(PROTO_DIR) --cpp_out=$(ENTITY_DIR) $<

$(ENTITY_DIR)/trade.pb.cc: $(PROTO_DIR)/trade.proto
	protoc -I=$(PROTO_DIR) --cpp_out=$(ENTITY_DIR) $<


.PHONY : clean
clean: 
	rm $(OBJ_DIR)/* 
