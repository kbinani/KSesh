.PHONY: all
all: public/eot.ttf public/main.js

public/eot.ttf: deps/font-tools/EgyptianOpenType/font/eot.ttf
	cp $< $@

public/main.js: src/*.ts src/insertions.ts src/component/*.tsx src/*.js tsconfig.json package.json
	npm run build

src/insertions.ts: deps/font-tools/EgyptianOpenType/insertions.py
	(echo 'import { Insertion } from "./insertion";'; cat $< | sed 's/\(#.*\)//g' | sed 's/insertions/export const insertions: {[key: string]: Insertion}/g') > $@
