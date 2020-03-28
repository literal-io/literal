import { SelectionAnnotation, DOMAIN } from "../browser-inject";

export interface Driver {
  initializeContext(arg: { browser: string; device: string }): Promise<void>;
  getScreenshot(arg: {
    outputPath: string;
    domain: DOMAIN;
  }): Promise<SelectionAnnotation[]>;
  cleanup(): Promise<void>;
}
