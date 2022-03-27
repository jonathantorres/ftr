SERVER_PROG := ftrd
CLIENT_PROG := ftr

# compile server program
$(SERVER_PROG):
	go build -o bin/ftrd github.com/jonathantorres/ftr/cmd/ftrd

# compile client program
$(CLIENT_PROG):
	go build -o bin/ftr github.com/jonathantorres/ftr/cmd/ftr

# Run tests
.PHONY: test
test:
	@echo "running tests..."

.PHONY: clean
clean:
	go clean
	rm -fr ./bin
	mkdir ./bin && touch ./bin/.gitkeep
