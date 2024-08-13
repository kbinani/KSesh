export function base64FromBuffer(buffer: Uint8Array): string {
  return btoa(
    buffer.reduce((data, byte) => data + String.fromCharCode(byte), ""),
  );
}

export function bufferFromBase64(s: string): Uint8Array {
  const raw = atob(s);
  const bytes: number[] = [];
  for (const ch of raw) {
    bytes.push(ch.charCodeAt(0));
  }
  return Uint8Array.from(bytes);
}
