export function logCanvas(canvas: HTMLCanvasElement) {
  const width = canvas.width;
  const height = canvas.height;
  const b64 = canvas.toDataURL("image/png");
  console.log(
    `%c `,
    `padding: ${height / 2}px ${width / 2}px; background: url("${b64}") no-repeat;`,
  );
}
