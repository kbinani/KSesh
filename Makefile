.PHONY: all
all: web src/desktop/Insertions.hpp

.PHONY: web
web: public/eot.ttf public/main.js src/web/insertions.ts src/web/static-data.ts

public/eot.ttf: deps/font-tools/EgyptianOpenType/font/eot.ttf
	cp $< $@

public/main.js: src/web/insertions.ts src/web/static-data.ts src/web/*.ts src/web/component/*.tsx src/web/*.js tsconfig.json package.json
	npm run build

src/web/insertions.ts: deps/font-tools/EgyptianOpenType/insertions.py script/insertions.ts.sh
	bash script/insertions.ts.sh $< > $@

src/desktop/Insertions.hpp: src/web/insertions.ts script/insertions.hpp.ts
	tsx script/insertions.hpp.ts > $@

src/web/static-data.ts: deps/font-tools/EgyptianOpenType/font/eot.ttf node_modules/harfbuzzjs/hb.wasm
	bash script/static-data.ts.sh eot deps/font-tools/EgyptianOpenType/font/eot.ttf harfbuzz node_modules/harfbuzzjs/hb.wasm > $@

.PHONY: clean
clean:
	rm -f public/eot.ttf public/main.js src/web/insertions.ts src/web/static-data.ts
