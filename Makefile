.PHONY: all
all: public/eot.ttf public/main.js src/insertions.ts src/static-data.ts src/desktop/Insertions.hpp

public/eot.ttf: deps/font-tools/EgyptianOpenType/font/eot.ttf
	cp $< $@

public/main.js: src/insertions.ts src/static-data.ts src/*.ts src/component/*.tsx src/*.js tsconfig.json package.json
	npm run build

src/insertions.ts: deps/font-tools/EgyptianOpenType/insertions.py script/insertions.ts.sh
	bash script/insertions.ts.sh $< > $@

src/desktop/Insertions.hpp: src/insertions.ts script/insertions.hpp.ts
	tsx script/insertions.hpp.ts > $@

src/static-data.ts: deps/font-tools/EgyptianOpenType/font/eot.ttf node_modules/harfbuzzjs/hb.wasm
	bash script/static-data.ts.sh eot deps/font-tools/EgyptianOpenType/font/eot.ttf harfbuzz node_modules/harfbuzzjs/hb.wasm > $@

clean:
	rm -f public/eot.ttf public/main.js src/insertions.ts src/static-data.ts
