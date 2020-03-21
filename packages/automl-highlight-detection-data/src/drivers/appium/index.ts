import { writeFileSync } from "fs";
import { remote as webdriver } from "webdriverio";
import {
  SelectionAnnotation,
  browserInject,
  DOMAIN
} from "../../browser-inject";
import { Driver } from "../types";

export class AppiumDriver implements Driver {
  context: WebdriverIOAsync.BrowserObject;

  initializeContext = async ({
    browser,
    device
  }: {
    browser: string;
    device: string;
  }) => {
    const opts = {
      port: 4723,
      capabilities: {
        platformName: "Android",
        platformVersion: "10",
        deviceName: device,
        browserName: browser
      }
    };

    this.context = await webdriver(opts);
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

    await this.context.navigateTo(href);

    const annotations = await browserInject(domain, (fn: any, arg: any) =>
      this.context.execute(fn, arg)
    );

    const screenshotData: string = await this.context.executeScript(
      "mobile:shell",
      [
        {
          command: "stty raw; screencap -p 2>/dev/null | base64 -w 0"
        }
      ]
    );

    writeFileSync(outputPath, screenshotData, { encoding: "base64" });

    return annotations;
  };

  cleanup = async () => {
    if (this.context) {
      return this.context.deleteSession();
    }
  };
}
