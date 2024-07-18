import * as React from "react";
import { FC } from "react";

export const App: FC = ({}) => {
  return (
    <div style={{ display: "flex", width: "100%", height: "100%" }}>
      <textarea
        style={{
          width: "50%",
          height: "100%",
          fontSize: "20px",
          resize: "none",
          outline: "none",
          border: "none"
        }}
        value={"M23-Z7-M18-M17-Z4:D54-G17-X1:G1-N25:X1*Z1"}
      />
      <div
        className="hieroglyphs"
        style={{ width: "50%", height: "100%", fontSize: "48px" }}
      >
        𓇓𓏲𓇍𓇋𓏭𓂻𓅓𓏏𓄿𓈉𓏏𓏤
      </div>
    </div>
  );
};
