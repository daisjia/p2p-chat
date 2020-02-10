INC += -I./
LIB +=  -ljson -levent -lpthread
OBJ_DIR = ./obj
OBJ =  $(OBJ_DIR)/client.o  $(OBJ_DIR)/main.o $(OBJ_DIR)/mytime.o 
OBJ_EXT = .o
CXX_EXT = .cpp


main : $(OBJ)
	@echo
	@echo "Linking..."
	g++ -o main $(OBJ) $(LIB)
	@echo

$(OBJ_DIR)/%$(OBJ_EXT) : %$(CXX_EXT)
	@echo
	@echo "Compiling $< ==> $@..."
	@-mkdir -p $(OBJ_DIR)
	g++ -c $< -o $@

cc:
	@echo
	@echo "Removeing..."
	-rm -rf $(OBJ_DIR) ./bin main
	@echo