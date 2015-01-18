CC = g++
INC_DIR=-I../dev/pushframework-2.2.1/include
CFLAGS=$(INC_DIR) -DUSE_NET_LIBS -O3
FM_FLAGS=-DUSE_FEMAS_API
# -DFAKE_DEAL  -DLOG_FOR_TRADE -DLOG_FOR_TECH_CALC
PROTO_DIR = ./proto
SRC = ./src
ENTITY_DIR = ./src/entity
OBJ_DIR = ./obj
FS_FM_OBJ_DIR = $(OBJ_DIR)/fs_fm
QS_OBJ_DIR = $(OBJ_DIR)/qs
QS_FM_OBJ_DIR = $(OBJ_DIR)/qs_fm
LIBS = -lPushFramework -lboost_thread -lboost_program_options -lboost_filesystem -lboost_chrono -lboost_date_time -lprotobuf -llog4cpp -lta_lib
CTP_TD_LIBS = -lthosttraderapi
CTP_MD_LIBS = -lthostmduserapi
FM_TD_LIBS = -lUSTPtraderapi
FM_MD_LIBS = -lUSTPmduserapi

_ENTITY_FILES = packet enum quote trade message  
ENTITY_FILES = $(patsubst %, $(ENTITY_DIR)/%.pb.cc, $(_ENTITY_FILES))
ENTITY_OBJS = $(patsubst %, $(OBJ_DIR)/%.pb.o, $(_ENTITY_FILES))

_FS_OBJS = stdafx \
		\
		AvatarClient AvatarFactory ClientProtocol ProtobufPacket ProtobufProtocol \
		RequestedServices TradingServer TradingServerProtocol \
		\
		QuoteAgent QuoteAgentFacade QuoteFetcher QuoteRepositry QuoteStore \
		QuoteUdpAgent ShmQuoteFeed ShmQuoteSubscribe UdpQuoteListener \
		\
		ArbitrageStrategy ArbitrageTrigger ASCTrendStrategy ASCTrendTrigger ChangePositionStrategy \
		ChangePositionTrigger HistSlopeStrategy HistSlopeTrigger LinerRegressionStrategy ManualStrategy \
		RangeTrendStrategy ScalperStrategy ScalperTrigger StrategyOrderCommand TrailingStopTrigger \
		WMATrendStrategy WMATrendTrigger \
		\
		ATRDataSet BollDataSet DiffRecordSet DonchianDataSet HistDataReader HistDataWriter LinerRegressionDataSet \
		MACDDataSet OHLCRecordSet PriceBarDataProxy PriceBarDataSource PriceBarGen TaIndicatorSet \
		TechAnalyStrategy TechDataRepo WATRStopDataSet WillRDataSet WMATrendDataSet \
		\
		LegOrderPlacer ManualOrderPlacer OrderProcessor PortfolioArbitrageOrderPlacer \
		PortfolioOrderHelper PortfolioOrderPlacer PortfolioScalperOrderPlacer PortfolioTrendOrderPlacer \
		\
		AsyncOrderPendingTimer charsetconvert Configuration DoubleCompare FakeDealer \
		FileOperations LogManager orderhelper SettingChangeTrace SymbolInfo \
		SymbolInfoRepositry SymbolTimeUtil TradingTimeSpan \
		\
		Leg Portfolio PortfolioManager Strategy TradeStation Trigger

_FS_CTP_OBJS = InputOrder RtnOrderWrapper TradeAgent 

_FS_FM_OBJS = InputOrder_FM RtnOrderWrapper_FM TradeAgent_FM 

FS_COM_CTP_OBJS = $(patsubst %, $(OBJ_DIR)/%.o, $(_FS_OBJS))
FS_COM_FM_OBJS = $(patsubst %, $(FS_FM_OBJ_DIR)/%.o, $(_FS_OBJS))
FS_CTP_OBJS = $(patsubst %, $(OBJ_DIR)/%.o, $(_FS_CTP_OBJS))
FS_FM_OBJS = $(patsubst %, $(FS_FM_OBJ_DIR)/%.o, $(_FS_FM_OBJS))

_QS_OBJS = stdafx \
		FileOperations MarketLoginWaiter MdSpi QS_Configuration QS_LogManager \
		QuoteAggregator QuoteMain QuoteProxy ShmQuoteFeed ShmQuoteSubscribe
_QS_FM_OBJS = stdafx \
		FileOperations MarketLoginWaiter MdSpi_FM QS_Configuration QS_LogManager \
		QuoteAggregator QuoteMain QuoteProxy_FM ShmQuoteFeed ShmQuoteSubscribe \
		UdpQuoteListener

QS_OBJS = $(patsubst %, $(QS_OBJ_DIR)/%.o, $(_QS_OBJS))
QS_FM_OBJS = $(patsubst %, $(QS_FM_OBJ_DIR)/%.o, $(_QS_FM_OBJS))

pt-station: $(ENTITY_OBJS) $(FS_COM_CTP_OBJS) $(FS_CTP_OBJS)
	$(CC) -o PTStation $^ $(LIBS) $(CTP_TD_LIBS) -Wall

pt-station-fm: $(ENTITY_OBJS) $(FS_COM_FM_OBJS) $(FS_FM_OBJS)
	$(CC) -o PTStationFM $^ $(LIBS) $(FM_TD_LIBS) -Wall

quote-station: $(QS_OBJS)
	$(CC) -o QuoteStation $^ $(LIBS) $(CTP_MD_LIBS) -Wall

quote-station-fm: $(QS_FM_OBJS)
	$(CC) -o QuoteStationFM $^ $(LIBS) $(FM_MD_LIBS) -Wall

# Compile entity
$(OBJ_DIR)/%.pb.o: $(ENTITY_DIR)/%.pb.cc
	$(CC) -c -o $@ $< $(CFLAGS)

# Compile pt station CTP main cpp
$(OBJ_DIR)/%.o: $(SRC)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS) 

# Compile pt station Femas main cpp
$(FS_FM_OBJ_DIR)/%.o: $(SRC)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(FM_FLAGS)

# Compile quote station CTP cpp
$(QS_OBJ_DIR)/%.o: $(SRC)/%.cpp
	$(CC) -c -o $@ $< -O3

# Compile quote station Femas cpp
$(QS_FM_OBJ_DIR)/%.o: $(SRC)/%.cpp
	$(CC) -c -o $@ $< -O3 $(FM_FLAGS)

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
	rm $(FS_FM_OBJ_DIR)/*.*
	rm $(QS_OBJ_DIR)/*.*
	rm $(QS_FM_OBJ_DIR)/*.*
