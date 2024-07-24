import React, { FC, SyntheticEvent } from "react";

export const About: FC<{ onClose: () => void }> = ({ onClose }) => {
  const onClick = (ev: SyntheticEvent) => {
    if (ev.target === ev.currentTarget) {
      onClose();
    }
  };
  return (
    <div className="about" onClick={onClick}>
      <div className="aboutContent">
        <div className="aboutContainer">
          <div className="aboutHeader">About WSesh</div>
          <div className="aboutSpacer" />
          <div className="aboutSection">Copyright (C) kbinani 2024</div>
          <div className="aboutSpacer" />
        </div>
      </div>
    </div>
  );
};
