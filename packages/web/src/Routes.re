module New = {
  [@decco]
  type params = {
    id: option(string),
    initialPhaseState: option(Containers_NoteEditor_New.phase),
  };
};
