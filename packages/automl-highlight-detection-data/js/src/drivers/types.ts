import { SelectionAnnotation, DOMAIN } from "../browser-inject";

export interface Driver {
  initializeContext(arg: { browser: string; device: string }): Promise<void>;
  getScreenshot(arg: {
    outputPath: string;
    domain: DOMAIN;
    href: string;
    forceNavigate: boolean
  }): Promise<SelectionAnnotation[]>;
  cleanup(): Promise<void>;
}
