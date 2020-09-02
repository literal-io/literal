type tag = {
  id: option(string),
  text: string,
};
type tagState = {
  commits: array(tag),
  partial: string,
  filterResults: array(tag),
};

type value = {
  text: string,
  tags: array(tag),
};

