# Grupo 03
# Bruno Freitas 51478 
# Marcelo Mouta 51110 
# Pedro Marques 51021

#############################
####      VARIABLES      ####
#############################

# pastas
OBJ_DIR = object
BIN_DIR = binary

# todos os testes
TESTES = test_data test_entry test_list test_table test_serialization

# testes
TEST_DATA = data.o test_data.o
TEST_ENTRY = data.o entry.o test_entry.o
TEST_LIST = data.o entry.o list.o test_list.o
TEST_TABLE = data.o entry.o list.o table.o test_table.o
TEST_SERIALIZATION = data.o entry.o  serialization.o test_serialization.o

# variaveis de configuracao
CC = gcc
CFLAGS = -Wall -I include -g

vpath $(OBJ_DIR)/*.o

#############################
#### MAKING BINARY FILES ####
#############################

projeto: $(addprefix $(BIN_DIR)/,$(TESTES))

./binary/test_data: $(TEST_DATA)
	$(CC) $(addprefix $(OBJ_DIR)/,$^) -o $@

./binary/test_entry: $(TEST_ENTRY)
	$(CC) $(addprefix $(OBJ_DIR)/,$^) -o $@

./binary/test_list: $(TEST_LIST)
	$(CC) $(addprefix $(OBJ_DIR)/,$^) -o $@

./binary/test_table: $(TEST_TABLE)
	$(CC) $(addprefix $(OBJ_DIR)/,$^) -o $@

./binary/test_serialization: $(TEST_SERIALIZATION)
	$(CC) $(addprefix $(OBJ_DIR)/,$^) -o $@

#############################
#### MAKING OBJECT FILES ####
#############################

%.o: source/%.c
	$(CC) $(CFLAGS) -c $< -o $(OBJ_DIR)/$@

#############################
####  EXECUTE  VALGRIND  ####
#############################

valgrind: 
	valgrind --leak-check=full ./binary/${TEST}

#############################
####     RUN  TESTS      ####
#############################

test: $(addprefix $(BIN_DIR)/,$(TESTES))
	@for i in $^; do \
            ./$$i; \
			echo "\n"; \
        done

#############################
####        CLEAN        ####
#############################

clean: 
	@rm -f $(OBJ_DIR)/*
	@rm -f binary/*
