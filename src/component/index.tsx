import * as React from "react";
import { App } from "./app";
import { createRoot } from "react-dom/client";

document.addEventListener("DOMContentLoaded", () => {
  const container = document.getElementById("app");
  const root = createRoot(container!);
  root.render(<App />);
});
