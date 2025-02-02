import React, {
  DetailedHTMLProps,
  FC,
  HTMLAttributes,
  SyntheticEvent,
  useEffect,
} from "react";

export const Advertisement: FC<{ onClose: () => void }> = ({ onClose }) => {
  const onClick = (ev: SyntheticEvent) => {
    if (ev.target === ev.currentTarget) {
      onClose();
    }
  };
  const onKeyDown = (e: KeyboardEvent) => {
    if (e.key === "Escape") {
      onClose();
    }
  };
  useEffect(() => {
    window.addEventListener("keydown", onKeyDown);
    return () => {
      window.removeEventListener("keydown", onKeyDown);
    };
  }, []);
  return (
    <div className="about" onClick={onClick}>
      <div className="aboutContent">
        <div
          className="aboutContainer"
          style={{ fontSize: "20px", lineHeight: "32px" }}
        >
          <div>
            This web application is a sample application that lets you easily
            try out some of KSesh’s features.
          </div>
          <div>
            To access the full range of KSesh’s capabilities, please download
            the desktop version from the Microsoft Store or the Apple App Store.
          </div>
          <div
            style={{
              display: "grid",
              gridTemplateRows: "1fr 1fr",
              gridTemplateColumns: "auto 1fr",
              alignItems: "center",
              margin: 30,
            }}
          >
            <div>Microsoft Store: </div>
            <div>
              <a
                href="https://apps.microsoft.com/detail/9ppdnn5s6z36?mode=direct"
                style={{ display: "inline-block" }}
              >
                <img
                  src="https://get.microsoft.com/images/en-us%20dark.svg"
                  style={{
                    width: 200,
                    verticalAlign: "middle",
                    objectFit: "contain",
                  }}
                />
              </a>
            </div>
            <div> Apple App Store:</div>
            <div>
              <a
                href="https://apps.apple.com/us/app/ksesh/id6596728393?mt=12&itscg=30200&itsct=apps_box_badge&mttnsubad=6596728393"
                style={{ display: "inline-block" }}
              >
                <img
                  src="https://toolbox.marketingtools.apple.com/api/v2/badges/download-on-the-app-store/black/en-us?releaseDate=1723420800"
                  alt="Download on the App Store"
                  style={{
                    width: 200,
                    height: 82,
                    verticalAlign: "middle",
                    objectFit: "contain",
                  }}
                />
              </a>
            </div>
          </div>
        </div>
      </div>
      <div className="aboutCloseButton" onClick={onClose} />
    </div>
  );
};
