export class Messenger {
  constructor() {
    this.handlers = new Map();
    this.eventQueue = [];

    window.addEventListener("message", (ev) => {
      if (ev.ports && ev.ports.length > 0 && !globalThis.literalMessagePort) {
        globalThis.literalMessagePort = ev.ports[0];
        this.eventQueue.forEach(this.postMessage);
        this.eventQueue = [];
      }

      this._handleMessage(ev);
    });
  }

  _handleMessage(ev) {
    try {
      const data = JSON.parse(ev.data);

      console.log("[Literal] Receieved message", ev.data);
      if (this.handlers.has(data.type)) {
        this.handlers.get(data.type).forEach((handler) => handler(data));
      }
    } catch (e) {
      console.error("[Literal] Unable to parse message", e, ev.data);
    }
  }

  postMessage(ev) {
    if (!globalThis.literalMessagePort) {
      this.eventQueue.push(ev);
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
