VPATH = src tests bin
CPPFLAGS = g++ -std=gnu++11 -Wall -Wextra -Isrc
PROG := ftr
PREFIX := /usr/local/$(PROG)
PROGS := server conf log session
TESTS := $(addsuffix _test,$(PROGS))
OBJS := $(addsuffix .o,$(PROGS))

# compile main program
$(PROG): $(PROG).cpp $(OBJS)
	$(CPPFLAGS) src/$(PROG).cpp $(OBJS) -o bin/$(PROG)

# compile objects
$(OBJS):%.o: %.cpp %.hpp
	$(CPPFLAGS) -c $^

# compile tests
$(TESTS):%: %.cpp $(OBJS)
	$(CPPFLAGS) $^ -o bin/$@
	./bin/$@

# install binary and server files
.PHONY: install
install:
	rm -rf $(PREFIX)
	mkdir -p $(PREFIX)
	mkdir -p $(PREFIX)/bin
	cp ./bin/$(PROG) $(PREFIX)/bin/$(PROG)
	cp ftr.conf $(PREFIX)

# Run tests
.PHONY: test
test: $(TESTS)
	@echo "TODO"

.PHONY: clean
clean:
	rm -f ./*.o src/*.h.gch src/*.hpp.gch
	rm -fr ./bin
	mkdir ./bin && touch ./bin/.gitkeep
