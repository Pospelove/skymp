import { mpClientPlugin, PacketType, on as spOn } from "@skymp/skyrim-platform";

type Handler = (messageOrError: Record<string, unknown> | string) => void;
const handlersMap = new Map<PacketType, Handler[]>();
let lastHostname = "";
let lastPort = 0;

spOn("tick", () => {
  mpClientPlugin.tick((packetType, jsonContent, error) => {
    const handlers = handlersMap.get(packetType) || [];
    handlers.forEach((handler) => {
      const parse = () => {
        try {
          return JSON.parse(jsonContent);
        } catch (e) {
          throw new Error(`JSON ${jsonContent} failed to parse: ${e}`);
        }
      };
      handler(jsonContent.length ? parse() : error);
    });
  });
});

export const connect = (hostname: string, port: number): void => {
  lastHostname = hostname;
  lastPort = port;
  mpClientPlugin.createClient(hostname, port);
};

export const close = (): void => {
  mpClientPlugin.destroyClient();
};

export const on = (packetType: PacketType, handler: Handler): void => {
  let arr = handlersMap.get(packetType);
  arr = (arr ? arr : []).concat([handler]);
  handlersMap.set(packetType, arr);
};

export const send = (msg: Record<string, unknown>, reliable: boolean): void => {
  mpClientPlugin.send(JSON.stringify(msg), reliable);
};

// Reconnect automatically
const reconnect = () => mpClientPlugin.createClient(lastHostname, lastPort);
on("connectionFailed", reconnect);
on("connectionDenied", reconnect);
on("disconnect", reconnect);
