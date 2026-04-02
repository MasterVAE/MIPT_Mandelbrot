SOURCES_MAIN = main.cpp render.cpp
				
TARGET_MAIN = $(TARGET_DIR)/prog.out

OBJECTS_MAIN = $(addprefix $(OBJ_DIR)/, $(SOURCES_MAIN:.cpp=.o))

$(TARGET_MAIN): $(OBJECTS_MAIN) | $(TARGET_DIR)
	@$(CC) $(CFLAGS) $^ -o $@ -lraylib -lm
	@echo "LINKED MAIN"

main_b: $(TARGET_MAIN)

main: $(TARGET_MAIN)
	@./$(TARGET_MAIN)