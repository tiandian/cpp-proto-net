CC = g++
INC_DIR=-I../dev/pushframework-2.2/include
CFLAGS=$(INC_DIR) -DUSE_NET_LIBS -O3
PROTO_DIR = ./proto
SRC = ./src
ENTITY_DIR = ./src/entity
OBJ_DIR = ./obj
QS_OBJ_DIR = $(OBJ_DIR)/qs
LIBS = -lPushFramework -lboost_thread -lboost_program_options -lboost_filesystem -lboost_chrono \
 -lboost_date_time -lprotobuf -llog4cpp -lta_lib -lthostmduserapi_13 -lthosttraderapi_13

_ENTITY_FILES = packet enum quote trade message  
ENTITY_FILES = $(patsubst %, $(ENTITY_DIR)/%.pb.cc, $(_ENTITY_FILES))
ENTITY_OBJS = $(patsubst %, $(OBJ_DIR)/%.pb.o, $(_ENTITY_FILES))

_FS_OBJS = stdafx \
		ArbitrageStrategy ArbitrageTrigger AsyncOrderPendingTimer AvatarClient AvatarFactory \
		ChangePositionStrategy ChangePositionTrigger charsetconvert ClientProtocol Configuration \
	    DoubleCompare FakeDealer FileOperations HistDataReader HistDataWriter \
		HistSlopeStrategy HistSlopeTrigger InputOrder Leg LegOrderPlacer \
		LogManager MACDDataSet OHLCRecordSet orderhelper OrderProcessor \
		Portfolio PortfolioManager PortfolioOrderHelper PortfolioOrderPlacer PortfolioScalperOrderPlacer \
		PortfolioTrendOrderPlacer PriceBarDataProxy PriceBarDataSource PriceBarGen ProtobufPacket \
		ProtobufProtocol QuoteAgent QuoteFetcher QuoteRepositry QuoteStore \
		RequestedServices RtnOrderWrapper ScalperStrategy ScalperTrigger SettingChangeTrace \
		ShmQuoteFeed ShmQuoteSubscribe Strategy SymbolInfo SymbolInfoRepositry \
		TaIndicatorSet TechAnalyStrategy TechDataRepo TradeAgent TradeStation \
		TradingServer TradingServerProtocol TradingTimeSpan Trigger

FS_OBJS = $(patsubst %, $(OBJ_DIR)/%.o, $(_FS_OBJS))

_QS_OBJS = stdafx \
		FileOperations MarketLoginWaiter MdSpi QS_Configuration QS_LogManager \
		QuoteAggregator QuoteMain QuoteProxy ShmQuoteFeed ShmQuoteSubscribe
QS_OBJS = $(patsubst %, $(QS_OBJ_DIR)/%.o, $(_QS_OBJS))

pt-station: $(ENTITY_OBJS) $(FS_OBJS)
	$(CC) -o PTStation $^ $(LIBS) -Wall

quote-station: $(QS_OBJS)
	$(CC) -o QuoteStation $^ $(LIBS) -Wall

# Compile entity
$(OBJ_DIR)/%.pb.o: $(ENTITY_DIR)/%.pb.cc
	$(CC) -c -o $@ $< $(CFLAGS)

# Compile pt station main cpp
$(OBJ_DIR)/%.o: $(SRC)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS) 

# Compile quote station cpp
$(QS_OBJ_DIR)/%.o: $(SRC)/%.cpp
	$(CC) -c -o $@ $< -O3

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
	rm $(OBJ_DIR)/*.* 
