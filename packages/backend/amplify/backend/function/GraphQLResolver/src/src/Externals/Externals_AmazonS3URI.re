type t = {
  region: string,
  bucket: string,
  key: string
};

[@bs.module] external make: string => t = "amazon-s3-uri"
