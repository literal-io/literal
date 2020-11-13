import { writeFileSync } from "fs";
import { WebDriverLogTypes } from "webdriver";
import { remote as webdriver } from "webdriverio";
import {
  SelectionAnnotation,
  browserInject,
  DOMAIN,
  parsers,
} from "../../browser-inject";
import { Driver } from "../types";

enum Orientation {
  LANDSCAPE = "LANDSCAPE",
  PORTRAIT = "PORTRAIT",
}

enum AppiumContext {
  NATIVE_APP = "NATIVE_APP",
  CHROMIUM = "CHROMIUM",
}

interface SystemBar {
  visible: boolean;
  height: number;
  width: number;
}
interface AndroidSystemBars {
  statusBar: SystemBar;
  navigationBar: SystemBar;
}

type AndroidCapabilities = WebDriver.DesiredCapabilities & {
  pixelRatio: number;
  viewportRect: {
    width: number;
    height: number;
    top: number;
    left: number;
  };
};

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
      port: parseInt(process.env.APPIUM_PORT),
      hostname: process.env.APPIUM_HOSTNAME,
      capabilities: {
        platformName: "Android",
        platformVersion: "11",
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
    const rect = this.getCapabilities().viewportRect;

    return orientation === Orientation.PORTRAIT
      ? rect
      : {
          ...rect,
          width: rect.height + rect.top,
          height: rect.width - rect.top,
        };
  };

  getCapabilities = () => {
    return this.context.capabilities as AndroidCapabilities;
  };

  getScreenshot = async ({
    href,
    domain,
    outputPath,
    forceNavigate,
  }: {
    href: string;
    outputPath: string;
    domain: DOMAIN;
    forceNavigate: boolean;
  }): Promise<SelectionAnnotation[]> => {
    if (!this.context) {
      throw new Error("Driver uninitialized");
    }

    const orientation =
      Math.random() > 0.66 ? Orientation.LANDSCAPE : Orientation.PORTRAIT;
    const currentOrientation = await this.context.getOrientation();

    if (currentOrientation !== orientation) {
      await this.context.setOrientation(orientation);
    }

    await this.context.switchContext(AppiumContext.CHROMIUM);
    const currentHref = await this.context.execute(() => window.location.href);
    if (forceNavigate || !parsers[domain].isUrlEqual(currentHref, href)) {
      await this.context.navigateTo(href);

      await this.context.switchContext(AppiumContext.NATIVE_APP);
      await this.context
        .findElement("id", "com.android.chrome:id/infobar_close_button")
        .then((id) => {
          if (id) {
            return this.context.$(id).then((el) => el.click());
          }
        });
      await this.context.switchContext(AppiumContext.CHROMIUM);
    }

    const { annotations, size } = await browserInject(
      domain,
      (fn: any, arg: any) => this.context.execute(fn, arg)
    );

    if (annotations.length === 0) {
      console.error("[error] annotations.length === 0, exiting early.");
      console.debug(
        "[debug] ",
        JSON.stringify({ annotations, size, orientation })
      );
      return [];
    }

    await this.context.switchContext(AppiumContext.NATIVE_APP);

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

    const pixelRatio = this.getCapabilities().pixelRatio;
    const viewportRect = await this.getViewportRect();
    const systemBars: AndroidSystemBars = (await this.context.getSystemBars()) as any;

    const statusBarHeight = systemBars.statusBar.visible
      ? systemBars.statusBar.height
      : 0;

    /**
     * Creating a selection within JS doesn't trigger Chrome's text selection
     * action menu. For realism, we want the normal UI that comes up to
     * appear. Using the highlight coords, tap in the center and wait a
     * short period to have chrome display the UI.
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
    const tapX = Math.min(
      xMin + (xMax - xMin) / 2,
      viewportRect.left + viewportRect.width - 1
    );
    const tapY = Math.min(
      yMin + (yMax - yMin) / 2 + chromeToolbarHeight + statusBarHeight,
      viewportRect.top + viewportRect.height - 1
    );
    await this.context.touchAction({
      action: "tap",
      x: tapX,
      y: tapY,
    });
    await new Promise((resolve) => setTimeout(resolve, 500));

    /** after clicking, make sure the selection still exists. */
    await this.context.switchContext(AppiumContext.CHROMIUM);
    const selectionExists = await this.context.execute(function() {
      return !window.getSelection().isCollapsed;
    });
    if (!selectionExists) {
      console.error("[error] Selection cleared, exiting early.");
      console.debug(
        "[debug] ",
        JSON.stringify({
          annotations,
          size,
          orientation,
          viewportRect,
          xRelativeMin,
          yRelativeMin,
          yRelativeMax,
        })
      );
      return [];
    }
    await this.context.switchContext(AppiumContext.NATIVE_APP);

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
            (orientation === Orientation.LANDSCAPE
              ? systemBars.navigationBar.width
              : 0),
          xRelativeMax * size.width * pixelRatio +
            (orientation === Orientation.LANDSCAPE
              ? systemBars.navigationBar.width
              : 0),
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
          (orientation === Orientation.PORTRAIT
            ? systemBars.navigationBar.height
            : 0);
        const viewportWidth =
          viewportRect.width +
          (orientation === Orientation.LANDSCAPE
            ? systemBars.navigationBar.width
            : 0);

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

    const screenshotData = await this.context.takeScreenshot();

    writeFileSync(outputPath, screenshotData, { encoding: "base64" });

    await this.context.switchContext(AppiumContext.CHROMIUM);

    console.log("getScreenshot complete", outputPath);
    return reframedBoundingBoxes;
  };

  cleanup = async () => {
    if (this.context) {
      return this.context.deleteSession();
    }
  };
}
