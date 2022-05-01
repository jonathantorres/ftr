PROG := ftr

# compile program
$(PROG):
	go build -o bin/ftr github.com/jonathantorres/ftr

# Run tests
.PHONY: test
test:
	go test ./internal/conf/

.PHONY: clean
clean:
	go clean
	rm -fr ./bin
	mkdir ./bin && touch ./bin/.gitkeep
