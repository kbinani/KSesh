import { FC } from "react";
import { Content } from "../content";

export const ContentComponent: FC<{ content: Content }> = ({ content }) => {
  return content.result;
};
