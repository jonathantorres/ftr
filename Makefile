PROG := ftr
PREFIX := /usr/local/ftr
PKG := github.com/jonathantorres/ftr

# compile program
$(PROG):
	go build -o bin/$(PROG) -ldflags="-X '$(PKG)/internal/server.Prefix=$(PREFIX)'" $(PKG)

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
test:
	go test ./internal/conf/

.PHONY: clean
clean:
	go clean
	rm -fr ./bin
	mkdir ./bin && touch ./bin/.gitkeep
