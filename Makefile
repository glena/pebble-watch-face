clean:
	pebble clean

build:
	pebble build

install:
	pebble install --phone $(IP)

log:
	pebble logs --phone $(IP)

dev: clean build install log
dev-build: clean build
dev-install: install log