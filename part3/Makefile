# Grupo 03
# Bruno Freitas 51468 
# Marcelo Mouta 51110 
# Pedro Marques 51021

# Directories
SRC_DIR = source
INC_DIR = include
OBJ_DIR = object
BIN_DIR = binary
LIB_DIR = lib
PROTOC_DIR = /usr/local/

CC = gcc
CFLAGS = -Wall -g -pthread -I $(INC_DIR)
LDFLAGS =${PROTOC_DIR}lib/libprotobuf-c.a
TARGET = table-server client-lib.o table-client
SERVER_OBJS = data.o entry.o list.o table.o sdmessage.pb-c.o message.o table_skel.o network_server.o table_server.o 
CLIENT_OBJS =  client-lib.o table_client.o
LIB_OBJS = data.o entry.o sdmessage.pb-c.o message.o network_client.o client_stub.o
COMPILE  = $(CC) $(CFLAGS) -c $< -o $(OBJ_DIR)/$@ 


ALL:$(TARGET)

client-lib.o: $(LIB_OBJS)
	ld -r $(addprefix $(OBJ_DIR)/,$(LIB_OBJS)) -o $(LIB_DIR)/client-lib.o

table-client: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $(LIB_DIR)/client-lib.o $(OBJ_DIR)/table_client.o $(LDFLAGS) -o $(BIN_DIR)/$@ 

table-server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(addprefix $(OBJ_DIR)/,$(SERVER_OBJS)) $(LDFLAGS) -o $(BIN_DIR)/$@

%.o: $(SRC_DIR)/%.c
	$(COMPILE)

clean:
	@rm -f $(OBJ_DIR)/*
	@rm -f $(BIN_DIR)/*
	@rm -f $(LIB_DIR)/*