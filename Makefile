.PHONY: all
all: public/eot.ttf public/main.js

public/eot.ttf: deps/font-tools/EgyptianOpenType/font/eot.ttf
	cp $< $@

public/main.js: src/insertions.ts src/static-data.ts src/*.ts src/component/*.tsx src/*.js tsconfig.json package.json
	npm run build

src/insertions.ts: deps/font-tools/EgyptianOpenType/insertions.py
	(echo 'import { Insertion } from "./insertion";'; cat $< | sed 's/\(#.*\)//g' | sed 's/insertions/export const insertions: {[key: string]: Insertion}/g') > $@

src/static-data.ts: deps/font-tools/EgyptianOpenType/font/eot.ttf node_modules/harfbuzzjs/hb.wasm
	bash script/static-data.sh eot deps/font-tools/EgyptianOpenType/font/eot.ttf harfbuzz node_modules/harfbuzzjs/hb.wasm > $@
