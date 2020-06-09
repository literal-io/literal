## Tags

- [x] delete highlight tags mutation when highlight is deleted
- [] update new note from share editor (consolidate editor implementations?)
- [] dynamodb stream on highlight, highlightTag deletion to clean up associations
- [] ensure highlight tags are unique on (tagId, highlightId, owner) and tags are unique on (owner, text)
- [] `recent` tag becomes set on dynamodb stream, with an expiry
- [] notes filters on tag, recent by default
- [] tags link to notes filter
- [] scroll up / down to see other tags
