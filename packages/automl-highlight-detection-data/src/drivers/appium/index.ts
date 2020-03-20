import { remote as webdriver } from "webdriverio";

(async () => {
  const opts = {
    port: 4723,
    capabilities: {
      platformName: "Android",
      platformVersion: "10",
      deviceName: "Android Emulator",
      browserName: "Chrome"
    }
  };

  const client = await webdriver(opts);

  const result = client.execute(
    function(a, b) {
      return a + b;
    },
    1,
    2
  );

  console.log("roundtrip: " + result);

  await client.deleteSession();
})();
