CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -g
LDFLAGS =

SRC_DIR_Q1 = src/q1
SRC_DIR_Q2 = src/q2
SRC_DIR_COMMON = src/common

OBJ_DIR = build

OBJS_Q1 = $(OBJ_DIR)/q1_main.o \
          $(OBJ_DIR)/memory_analyzer.o \
          $(OBJ_DIR)/proc_parser.o \
          $(OBJ_DIR)/leak_detector.o \
          $(OBJ_DIR)/utils.o

OBJS_Q2 = $(OBJ_DIR)/q2_main.o \
          $(OBJ_DIR)/security_checker.o \
          $(OBJ_DIR)/code_parser.o \
          $(OBJ_DIR)/recommender.o \
          $(OBJ_DIR)/utils.o

all: dirs app1 app2

dirs:
	mkdir -p $(OBJ_DIR)

# ---- APP1 (q1) ----
app1: q1
	@cp q1 app1
	@echo "Created app1 (copy of q1)"

q1: $(OBJS_Q1)
	$(CC) $(CFLAGS) -o q1 $(OBJS_Q1) $(LDFLAGS)

# ---- APP2 (q2) ----
app2: q2
	@cp q2 app2
	@echo "Created app2 (copy of q2)"

q2: $(OBJS_Q2)
	$(CC) $(CFLAGS) -o q2 $(OBJS_Q2) $(LDFLAGS)

# ---- Object rules ----

$(OBJ_DIR)/q1_main.o: $(SRC_DIR_Q1)/main.c $(SRC_DIR_Q1)/memory_analyzer.h $(SRC_DIR_Q1)/proc_parser.h $(SRC_DIR_Q1)/leak_detector.h src/common/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/memory_analyzer.o: $(SRC_DIR_Q1)/memory_analyzer.c $(SRC_DIR_Q1)/memory_analyzer.h $(SRC_DIR_Q1)/proc_parser.h src/common/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/proc_parser.o: $(SRC_DIR_Q1)/proc_parser.c $(SRC_DIR_Q1)/proc_parser.h src/common/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/leak_detector.o: $(SRC_DIR_Q1)/leak_detector.c $(SRC_DIR_Q1)/leak_detector.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/q2_main.o: $(SRC_DIR_Q2)/main.c $(SRC_DIR_Q2)/security_checker.h $(SRC_DIR_Q2)/code_parser.h $(SRC_DIR_Q2)/recommender.h src/common/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/security_checker.o: $(SRC_DIR_Q2)/security_checker.c $(SRC_DIR_Q2)/security_checker.h $(SRC_DIR_Q2)/code_parser.h src/common/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/code_parser.o: $(SRC_DIR_Q2)/code_parser.c $(SRC_DIR_Q2)/code_parser.h src/common/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/recommender.o: $(SRC_DIR_Q2)/recommender.c $(SRC_DIR_Q2)/recommender.h src/common/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/utils.o: $(SRC_DIR_COMMON)/utils.c src/common/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

# ---- Cleaning ----
clean:
	rm -rf $(OBJ_DIR) q1 q2 app1 app2

.PHONY: all clean dirs app1 app2
