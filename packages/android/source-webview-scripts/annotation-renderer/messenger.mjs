export class Messenger {
  constructor() {
    this.handlers = new Map();
    this.eventQueue = [];

    window.addEventListener("message", (ev) => {
      if (ev.ports && ev.ports.length > 0 && !window.literalMessagePort) {
        window.literalMessagePort = ev.ports[0];
        this.eventQueue.forEach(this.postMessage);
        this.eventQueue = [];
      }

      this._handleMessage(ev);
    });

    if (!window.literalMessagePort && window.literalWebview) {
      window.literalWebview.sendMessagePort();
    }
  }

  _handleMessage(ev) {
    try {
      const data = JSON.parse(ev.data);

      console.log("[Literal] Receieved message", ev.data);
      if (this.handlers.has(data.type)) {
        this.handlers.get(data.type).forEach((handler) => handler(data));
      }
    } catch (e) {
      console.log("[Literal] Unable to parse message", ev.data);
    }
  }

  postMessage(ev) {
    if (!window.literalMessagePort) {
      this.eventQueue.push(ev);
      return;
    }
    window.literalMessagePort.postMessage(JSON.stringify(ev));
  }

  on(type, handler) {
    if (!this.handlers.has(type)) {
      this.handlers.set(type, [handler]);
    } else {
      this.handlers.get(type).push(handler);
    }
  }
}
