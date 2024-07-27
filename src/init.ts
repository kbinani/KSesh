import { loadHarfbuzz } from "./harfbuzz";
import { staticData } from "./static-data";
import { FontData } from "./font-data";
import { bufferFromBase64 } from "./base64";

export async function init(): Promise<FontData> {
  const promises: Promise<void>[] = [];
  promises.push(
    loadHarfbuzz("data:application/wasm;base64," + staticData.harfbuzz),
  );
  if (typeof document === "object") {
    promises.push(
      new Promise((res, rej) =>
        document.fonts
          .load("48px 'Egyptian Text Proto'", "𓏞𓏜")
          .then(() => res())
          .catch(rej),
      ),
    );
  }
  await Promise.all(promises);
  return new FontData(bufferFromBase64(staticData.eot));
}
