import webdriver from "webdriverio";

async () => {
  const opts = {
    capabilities: {
      platformName: "Android",
      platformVersion: "10",
      deviceName: "Android Emulator",
      browserName: "Chrome"
    }
  };

  const client = webdriver.remote(opts);

  const result = client.execute(
    function(a, b) {
      return a + b;
    },
    1,
    2
  );

  console.log("roundtrip: " + result);

  await client.deleteSession();
};
