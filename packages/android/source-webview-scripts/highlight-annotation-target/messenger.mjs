export class Messenger {
  constructor() {
    this.handlers = new Map();

    window.addEventListener("message", (ev) => {
      if (ev.ports && ev.ports.length > 0 && !globalThis.literalMessagePort) {
        globalThis.literalMessagePort = ev.ports[0];
      }

      this._handleMessage(ev);
    });
  }

  _handleMessage(ev) {
    try {
      const data = JSON.parse(ev.data);

      if (this.handlers.has(data.type)) {
        this.handlers.get(data.type).forEach((handler) => handler(data));
      }
    } catch (e) {
      console.error("[Literal] Unable to parse message", e);
    }
  }

  postMessage(ev) {
    if (!globalThis.literalMessagePort) {
      console.error("[Literal] Unable to dispatch: has not initialized");
      return;
    }
    globalThis.literalMessagePort.postMessage(JSON.stringify(ev));
  }

  on(type, handler) {
    if (!this.handlers.has(type)) {
      this.handlers.set(type, [handler]);
    } else {
      this.handlers.get(type).push(handler);
    }
  }
}
