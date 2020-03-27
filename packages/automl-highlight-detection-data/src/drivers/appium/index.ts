import { writeFileSync } from "fs";
import { WebDriverLogTypes } from "webdriver";
import { remote as webdriver } from "webdriverio";
import {
  SelectionAnnotation,
  browserInject,
  DOMAIN,
} from "../../browser-inject";
import { Driver } from "../types";

export class AppiumDriver implements Driver {
  context: WebdriverIOAsync.BrowserObject;

  initializeContext = async ({
    browser,
    device,
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
        browserName: browser,
      },
      logLevel: "warn" as WebDriverLogTypes,
    };

    this.context = await webdriver(opts);
  };

  getViewportRect = async () => {
    if (!this.context) {
      throw new Error("Driver uninitialized");
    }

    const orientation = await this.context.getOrientation();
    const rect = (this.context.capabilities as any).viewportRect;

    return orientation === "PORTRAIT"
      ? rect
      : {
          ...rect,
          width: rect.height + rect.top,
          height: rect.width - rect.top,
        };
  };

  getScreenshot = async ({
    href,
    outputPath,
    domain,
  }: {
    href: string;
    outputPath: string;
    domain: DOMAIN;
  }): Promise<SelectionAnnotation[]> => {
    if (!this.context) {
      throw new Error("Driver uninitialized");
    }

    const orientation = Math.random() > 0.66 ? "LANDSCAPE" : "PORTRAIT";
    const currentOrientation = await this.context.getOrientation();
    if (currentOrientation !== orientation) {
      await this.context.setOrientation(orientation);
    }

    await this.context.switchContext("CHROMIUM");

    await this.context.navigateTo(href);

    const { annotations, size } = await browserInject(
      domain,
      (fn: any, arg: any) => this.context.execute(fn, arg)
    );

    if (annotations.length === 0) {
      return [];
    }

    await this.context.switchContext("NATIVE_APP");

    const chromeToolbarHeight = await this.context
      .findElement("id", "com.android.chrome:id/toolbar")
      .then((id) => {
        if (!id) {
          return 0;
        }
        return this.context
          .$(id)
          .then((el) => el.getSize())
          .then((s) => s.height);
      })
      .catch((_err) => 0);

    const pixelRatio = (this.context.capabilities as any).pixelRatio;
    const viewportRect = await this.getViewportRect();
    const systemBars: any = await this.context.getSystemBars();

    const statusBarHeight = systemBars.statusBar.visible
      ? systemBars.statusBar.height
      : 0;

    /**
     * Creating a selection within JS doesn't trigger Chrome's text selection action
     * menu. For realism, we want the normal UI that comes up to appear. Using the
     * highlight coords, tap in the center and wait a short period to have chrome
     * display the UI.
     */
    const {
      boundingBox: { xRelativeMin, xRelativeMax, yRelativeMin, yRelativeMax },
    } = annotations.find(({ label }) => label === "highlight");
    const [xMin, xMax, yMin, yMax] = [
      xRelativeMin * size.width * pixelRatio,
      xRelativeMax * size.width * pixelRatio,
      yRelativeMin * size.height * pixelRatio,
      yRelativeMax * size.height * pixelRatio,
    ];
    await this.context.touchAction({
      action: "tap",
      x: Math.min(
        xMin + (xMax - xMin) / 2,
        viewportRect.left + viewportRect.width - 1
      ),
      y: Math.min(
        yMin + (yMax - yMin) / 2 + chromeToolbarHeight + statusBarHeight,
        viewportRect.top + viewportRect.height - 1
      ),
    });
    await new Promise((resolve) => setTimeout(resolve, 500));

    /** after clicking, make sure the selection still exists. */
    await this.context.switchContext("CHROMIUM");
    const selectionExists = await this.context.execute(function() {
      return !window.getSelection().isCollapsed;
    });
    if (!selectionExists) {
      return [];
    }
    await this.context.switchContext("NATIVE_APP");

    /**
     * Bounding boxes were calculated relative to DOM viewport, adjust to overall
     * device viewport.
     */
    const reframedBoundingBoxes = annotations.map(
      ({
        boundingBox: { xRelativeMin, xRelativeMax, yRelativeMin, yRelativeMax },
        label,
      }) => {
        const [xMin, xMax, yMin, yMax] = [
          xRelativeMin * size.width * pixelRatio +
            (orientation === "LANDSCAPE" ? systemBars.navigationBar.width : 0),
          xRelativeMax * size.width * pixelRatio +
            (orientation === "LANDSCAPE" ? systemBars.navigationBar.width : 0),
          yRelativeMin * size.height * pixelRatio +
            chromeToolbarHeight +
            statusBarHeight,
          yRelativeMax * size.height * pixelRatio +
            chromeToolbarHeight +
            statusBarHeight,
        ];
        const viewportHeight =
          viewportRect.height +
          statusBarHeight +
          // @ts-ignore
          (orientation === "PORTRAIT" ? systemBars.navigationBar.height : 0);
        const viewportWidth =
          viewportRect.width +
          (orientation === "LANDSCAPE" ? systemBars.navigationBar.width : 0);

        return {
          label,
          boundingBox: {
            xRelativeMin: Math.max(xMin / viewportWidth, 0),
            xRelativeMax: Math.min(xMax / viewportWidth, 1.0),
            yRelativeMin: Math.max(yMin / viewportHeight, 0),
            yRelativeMax: Math.min(yMax / viewportHeight, 1.0),
          },
        };
      }
    );

    const screenshotData: string = await this.context.executeScript(
      "mobile:shell",
      [
        {
          command: "stty raw; screencap -p 2>/dev/null | base64 -w 0",
        },
      ]
    );

    writeFileSync(outputPath, screenshotData, { encoding: "base64" });

    await this.context.switchContext("CHROMIUM");

    return reframedBoundingBoxes;
  };

  cleanup = async () => {
    if (this.context) {
      return this.context.deleteSession();
    }
  };
}
