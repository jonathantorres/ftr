PROG := ftr
PKG := github.com/jonathantorres/ftr
PREFIX := $(shell pwd)
LDFLAGS := -X '$(PKG)/internal/server.Prefix=$(PREFIX)'

# Add another variable (if necessary)
# LDFLAGS += -X 'main.foo=bar'

# compile program
$(PROG):
	go build -o bin/$(PROG) -ldflags="$(LDFLAGS)" $(PKG)

# create release version
.PHONY: release
release:
	go build -o bin/$(PROG) -ldflags="-s -w $(LDFLAGS)" $(PKG)

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
