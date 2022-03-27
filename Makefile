SERVER_PROG := ftrd
CLIENT_PROG := ftr

# compile server program
$(SERVER_PROG):
	go build -o bin/ftrd

# compile client program
$(CLIENT_PROG):
	go build -o bin/ftr

# Run tests
.PHONY: test
test:
	@echo "running tests..."

.PHONY: clean
clean:
	go clean
	rm -fr ./bin
	mkdir ./bin && touch ./bin/.gitkeep
