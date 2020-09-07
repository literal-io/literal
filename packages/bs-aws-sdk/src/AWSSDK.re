module Config = {
  type t;

  [@bs.send] external update: (t, {. "region": string}) => unit = "update";
  [@bs.set]
  external mediaconvert: (t, {. "endpoint": string}) => unit = "mediaconvert";
};

module CloudFront = {
  module Signer = {
    type t;

    /** keyPairId, privateKey */
    [@bs.new] [@bs.scope "CloudFront"] [@bs.module "aws-sdk"]
    external make: (string, string) => t = "Signer";

    module SignedCookie = {
      [@bs.deriving abstract]
      type t = {
        [@bs.as "CloudFront-Policy"]
        policy: string,
        [@bs.as "CloudFront-Signature"]
        signature: string,
        [@bs.as "CloudFront-Key-Pair-Id"]
        keyPairId: string,
      };
    };

    [@bs.send]
    external getSignedCookie:
      (
        t,
        {
          .
          "expires": option(float),
          "policy": option(string),
          "url": option(string),
        }
      ) =>
      SignedCookie.t =
      "getSignedCookie";
  };
};

module Request = {
  type t('a);
  [@bs.send] external promise: t('a) => Js.Promise.t('a) = "promise";
};

module MediaConvert = {
  type t;
  type jobResponse;

  module Job = {
    [@bs.deriving abstract]
    type fileGroupSettings = {
      [@bs.as "Destination"]
      destination: string,
    };

    [@bs.deriving abstract]
    type outputGroupSettings = {
      [@bs.as "FileGroupSettings"]
      fileGroupSettings,
    };
    [@bs.deriving abstract]
    type containerSettings = {
      [@bs.as "Container"]
      container: string,
    };

    [@bs.deriving abstract]
    type output = {
      [@bs.as "ContainerSettings"]
      containerSettings,
    };

    [@bs.deriving abstract]
    type outputGroup = {
      [@bs.as "OutputGroupSettings"]
      outputGroupSettings,
      [@bs.as "Outputs"]
      outputs: array(output),
    };

    [@bs.deriving abstract]
    type settings = {
      [@bs.as "OutputGroups"]
      outputGroups: array(outputGroup),
    };

    [@bs.deriving abstract]
    type job = {
      [@bs.as "Id"]
      id: string,
      [@bs.as "Settings"]
      settings,
    };

    [@bs.deriving abstract]
    type t = {
      [@bs.as "Job"]
      job,
    };
  };

  [@bs.deriving abstract]
  type makeParams = {
    apiVersion: string,
    region: string,
    endpoint: string,
  };

  [@bs.new] [@bs.module "aws-sdk"]
  external make: makeParams => t = "MediaConvert";

  [@bs.send]
  external createJob: (t, Js.t({..})) => Request.t(Job.t) = "createJob";

  [@bs.deriving abstract]
  type getJobParams = {
    [@bs.as "Id"]
    id: string,
  };
  [@bs.send]
  external getJob: (t, getJobParams) => Request.t(Job.t) = "getJob";
};

[@bs.deriving abstract]
type t = {config: Config.t};

[@bs.module] external inst: t = "aws-sdk";
