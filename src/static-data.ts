// @ts-ignore
import _hbWASMBinary from "../node_modules/harfbuzzjs/hb.wasm";

// @ts-ignore
import _eotTTFBinary from "../public/eot.ttf";

export const staticData = {
  harfbuzz: _hbWASMBinary as Uint8Array,
  eot: _eotTTFBinary as Uint8Array,
};
