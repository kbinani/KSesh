.PHONY: all
all: public/eot.ttf public/main.js

public/eot.ttf: deps/font-tools/EgyptianOpenType/font/eot.ttf
	cp $< $@

public/main.js: src/**/*.ts src/**/*.tsx src/**/*.js tsconfig.json package.json
	npm run build
