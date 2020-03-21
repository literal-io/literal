import { devices, firefox, BrowserContext } from "playwright";
import {
  SelectionAnnotation,
  browserInject,
  DOMAIN
} from "../../browser-inject";
import { Driver } from "../types";

export class PlaywrightDriver implements Driver {
  context: BrowserContext;

  initializeContext = async ({
    browser,
    device = "Pixel 2"
  }: {
    browser: string;
    device: string;
  }): Promise<void> => {
    const d = devices[device];
    const browserInst = await (async function() {
      switch (browser) {
        case "firefox":
          return firefox.launch();
      }
    })();
    this.context = await browserInst.newContext({
      viewport: d.viewport,
      userAgent: d.userAgent
    });
  };

  getScreenshot = async ({
    href,
    outputPath,
    domain
  }: {
    href: string;
    outputPath: string;
    domain: DOMAIN;
  }): Promise<SelectionAnnotation[]> => {
    if (!this.context) {
      throw new Error("Driver uninitialized");
    }

    const page = await this.context.newPage();
    await page.goto(href);

    const annotations = await browserInject(domain, page.evaluate);

    await page.screenshot({ path: outputPath });
    await page.close();

    return annotations;
  };

  cleanup = async () => {
    if (this.context) {
      await this.context.close();
    }
  };
}
