CC=g++
ODIR=obj
LDIR =-L/usr/lib -L/usr/local/lib
LIBS=-lprotobuf -lboost_thread
VPATH=./ap_invoke

_OBJECTS=TradeStationMain.o apsessionmanager.o packet.pb.o server.o
OBJECTS=$(patsubst %, $(ODIR)/%, $(_OBJECTS))

FastTrade:$(OBJECTS)
	$(CC) -o $@ $^ $(LIBS) $(LDIR)

$(ODIR)/TradeStationMain.o:TradeStationMain.cpp
	$(CC) -c -o $@ $<

$(ODIR)/packet.pb.o:packet.pb.h
	$(CC) -c -o $@ $<
$(ODIR)/server.o:server.h connection.h
	$(CC) -c -o $@ $<
$(ODIR)/apsessionmanager.o:apsessionmanager.h session.h msgtype.h connection.h \
							server.h packet.pb.h
	$(CC) -c -o $@ $< -D__USE_W32_SOCKETS

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o 
