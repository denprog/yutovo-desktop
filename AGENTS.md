# Agent Instructions for Yutovo Library

## Project Overview

Yutovo library is a documentation system consisting of `.yut` files — a JSON-based document format. The library provides help articles, scientific articles, calculators, examples and other info for the Yutovo calculator app.

## File Format: `.yut`

- `.yut` files are **JSON documents** with a specific schema.
- Top-level fields:
  - `file_guid`: unique UUID per file (must **not** be shared across translations)
  - `config`: document-specific settings
  - `string_formats`: text formatting definitions
  - `paragraph_formats`: paragraph formatting definitions
  - `block_formats`: legacy field, usually an empty array
  - `text`: document body (root element)
  - `caret`: caret state
  - `selection`: selection state

### `config`

Document-specific part of the editor config:

| Field | Meaning |
|-------|---------|
| `language` | `Language` enum value: `English = 1`, `Russian = 2`, `Spanish = 3`, `BrazilianPortuguese = 4` |
| `use_tabs` | Indent with tabs (`true`) or spaces (`false`) |
| `tab_spaces` | Number of spaces per tab |
| `scale` | Document UI scale factor |
| `real_result` | Default real-number result settings (`precision`, `exp`, `default_angle_measure`, `result_angle_measure`, `show_angle_measure`) |
| `integer_result` | Default integer result settings (`result_notation`, `default_notation`, `show_notation`) |
| `rational_result` | Default rational result settings (`fraction_form`) |
| `complex_result` | Default complex result settings |
| `auto_result` | Auto-result settings and the ordered list of result types |
| `include_documents` | List of included documents (`{ file_name: "..." }`) |

### `string_formats`

Each entry defines a reusable character style:

| Field | Meaning |
|-------|---------|
| `id` | UUID referenced by `format_id` in text elements |
| `family` | Font family (e.g. `Arial`, `Courier New`, `FreeMono`) |
| `size` | Font size in points |
| `bold`, `italic`, `underline`, `strikethrough` | Boolean flags |
| `subscript`, `superscript` | Boolean flags |
| `text_color` | Text color as a 32-bit ARGB integer |
| `text_bg_color` | Background color as a 32-bit ARGB integer |
| `text_bg_selection_color` | Selection background color as a 32-bit ARGB integer |

### `paragraph_formats`

Each entry defines a reusable paragraph style. The editor recognizes the canonical English names and translates them on load according to `config.language`, but the JSON stores the localized name.

| Field | Meaning |
|-------|---------|
| `name` | Paragraph format name (must be translated; see table below) |
| `alignment` | `0 = Left`, `1 = Right`, `2 = Center`, `3 = Justify` |
| `word_wrap` | `0 = None`, `1 = Normal` |
| `line_spacing` | Line spacing value |
| `indent_before`, `indent_after` | Paragraph indentation |
| `indent_first_line` | First-line indent |
| `spacing_before`, `spacing_after` | Spacing before/after paragraph |
| `default_string_format` | UUID of the default `string_format` for this paragraph style |

### `text`

The `text` object is the root element of the document tree.

| Field | Meaning |
|-------|---------|
| `id` | Hierarchical element id (e.g. `"0,0"`) |
| `type` | Always `1` (`ElementType::TEXT`) for the root |
| `level` | Superscript/subscript level |
| `elements` | Array of child elements |

## Document Structure

The `text.elements` array contains paragraphs (`type: 2`). Each paragraph has `elements` which can be:

| Type | Enum name | Meaning |
|------|-----------|---------|
| `type: 1` | `TEXT` | Document root element |
| `type: 2` | `PARAGRAPH` | Paragraph; has `format_name` (and optionally `format_alignment`, `marker`, `marker_format_id`) |
| `type: 3` | `ROW` | Inline group / span; may have `format_name` |
| `type: 4` | `STRING` | Plain text — `elements` is a **single string** (not an array of characters); this is the only text that should be translated |
| `type: 5` | `CODE_BLOCK` | Block of code paragraphs |
| `type: 6` | `CODE_PARAGRAPH` | A line inside a code block |
| `type: 7` | `CODE_ROW` | Row inside a code paragraph |
| `type: 8` | `CODE_STRING` | Literal characters inside code |
| `type: 10` | `SHAPE` | Empty placeholder / shape element |
| `type: 11` | `PLUS` | Plus operator |
| `type: 12` | `MINUS` | Minus operator |
| `type: 13` | `MULTIPLY` | Multiplication operator |
| `type: 14` | `DIVISION` | Division / fraction |
| `type: 15` | `POWER` | Superscript (power) |
| `type: 16` | `SQUARE_ROOT` | Square root |
| `type: 17` | `NTH_ROOT` | N-th root |
| `type: 18` | `EQUATION` | Equation / result container |
| `type: 19` | `OPEN_ROUND_BRACKET` | Opening round bracket `(` |
| `type: 20` | `CLOSE_ROUND_BRACKET` | Closing round bracket `)` |
| `type: 25` | `AUTO_RESULT` | Auto result marker (`=`) |
| `type: 27` | `ASSIGNMENT` | Assignment operator |
| `type: 28` | `SUBSCRIPT` | Subscript |
| `type: 35` | `SUM` | Summation |
| `type: 36` | `PRODUCT` | Product (∏) |
| `type: 37` | `UNIT` | Unit element |
| `type: 38` | `COMMA` | Comma separator |
| `type: 39` | `LINK` | Hyperlink (`url` field contains the target path) |
| `type: 40` | `OPEN_SQUARE_BRACKET` | Opening square bracket `[` |
| `type: 41` | `CLOSE_SQUARE_BRACKET` | Closing square bracket `]` |
| `type: 43` | `GRAPH_LINE` | Graph line element |
| `type: 44` | `CODE_PARAGRAPHS_BLOCK` | Multi-paragraph code block |
| `type: 45` | `SYMBOLIC_REAL_RESULT` | Symbolic real result |
| `type: 46` | `SYMBOLIC_RATIONAL_RESULT` | Symbolic rational result |
| `type: 47` | `NOT` | Logical NOT |
| `type: 49` | `DEFINITE_INTEGRAL` | Definite integral |
| `type: 50` | `INDEFINITE_INTEGRAL` | Indefinite integral |

### Formula element child counts

When building formula elements by hand, use these child layouts:

- `DIVISION` (14): `[CODE_ROW numerator, SHAPE, CODE_ROW denominator]`
- `POWER` (15): `[CODE_ROW base, SHAPE, CODE_ROW exponent]`
- `SQUARE_ROOT` (16): `[SHAPE, CODE_ROW radicand]`
- `SUBSCRIPT` (28): `[CODE_ROW base, SHAPE, CODE_ROW subscript]`
- `SUM` (35): `[ASSIGNMENT lower, SHAPE, CODE_ROW upper, CODE_ROW body]`
- `PRODUCT` (36): same layout as `SUM`
- `DEFINITE_INTEGRAL` (49): `[CODE_ROW lower, SHAPE, CODE_ROW upper, CODE_ROW integrand, CODE_STRING "d", CODE_ROW variable]`
- `INDEFINITE_INTEGRAL` (50): `[SHAPE, CODE_ROW integrand, CODE_STRING "d", CODE_ROW variable]`

`PLUS`/`MINUS`/`MULTIPLY`/`COMMA` and bracket elements contain arrays of `SHAPE` placeholders plus a `symbol` field.

`CODE_STRING` (`type: 8`) must always be placed inside a `CODE_BLOCK` (`type: 5`) hierarchy so the editor can resolve its code context.

## Hand-authoring `.yut` documents

When creating or editing `.yut` files by hand (e.g., for library articles), follow these rules. The editor and desktop app are strict; deviations usually cause `document not parsed` or a crash.

### Top-level structure

```json
{
  "file_guid": "<new-uuid>",
  "config": { "language": 2, ... },
  "string_formats": [...],
  "paragraph_formats": [...],
  "block_formats": [],
  "text": { "id": "0", "type": 1, "elements": [...] },
  "caret": { "id": "0,0,0,0" },
  "selection": []
}
```

- `file_guid` must be unique per file. Never reuse it across translations.
- `config.language` must match the folder language (`English = 1`, `Russian = 2`, `Spanish = 3`, `BrazilianPortuguese = 4`).
- `block_formats` is legacy. Keep it as an empty array `[]` or omit it entirely.
- The root `text` element must have `type: 1` and must **not** have a `level` field.

### Element IDs

Use hierarchical IDs: `"0"`, `"0,0"`, `"0,0,0"`, etc. They must reflect the actual tree position. After regenerating a tree, reassign all IDs from the root. `caret.id` must point to an existing element.

Paragraph indices start at `0`. Exception: documents with `config.include_documents` — the included documents' paragraphs are prepended to the document, so the numbering of the document's own paragraphs starts at the total paragraph count of all included documents (e.g. a document that includes one 10-paragraph document numbers its own paragraphs `0,10`, `0,11`, …). Only the second id component (the paragraph index) is affected; deeper components are numbered within the paragraph as usual.

### String and code elements

- `type: 4` (`STRING`) `elements` must be a single string, not an array of characters.
- `type: 39` (`LINK`) `elements` must be a single string; `url` is a string.
- `type: 8` (`CODE_STRING`) `elements` must be a single string and must live inside a `CODE_BLOCK` (`type: 5`) hierarchy.

### Paragraph formats

- `paragraph_formats` must contain every format name referenced in `text.elements`.
- Names must be localized to match `config.language` (e.g., Russian names for `language: 2`). The canonical English names are recognized by the editor, but storing localized names is safer and avoids ambiguity.
- For code/formula blocks, include a `Код` / `Code` / `Código` / `Código` paragraph format and reference a monospace `string_format`.

### String formats and references

- Every `format_id` in `type: 4` and `type: 8` elements must match an existing UUID in `string_formats`.
- Every `default_string_format` in `paragraph_formats` must match an existing UUID in `string_formats`.

### Code blocks and formulas

- `CODE_BLOCK` (`type: 5`) must have a unique `code_id` integer per block.
- `CODE_BLOCK` must be placed inside a `ROW` (`type: 3`) inside a `PARAGRAPH` (`type: 2`).
- `CODE_PARAGRAPH` (`type: 6`) must have `format_name` pointing to the code paragraph format.
- `CODE_ROW` (`type: 7`) contains `CODE_STRING` and/or formula elements.
- Do not mix `type: 4` (`STRING`) and code/formula elements in the same `ROW`.

### Equations with results

- `EQUATION` (`type: 18`) must have `result_type`.
- Structure: `[CODE_ROW left, SHAPE, CODE_ROW right]`.
- The right `CODE_ROW` must contain a result element (`REAL_RESULT`, `INTEGER_RESULT`, `RATIONAL_RESULT`, `COMPLEX_RESULT`, `AUTO_RESULT`, etc.).
- `EQUATION` must be inside a `CODE_BLOCK` so the result element can resolve its `code_id`.
- If you only need to display a formula (without solving), use a `CODE_BLOCK` with `CODE_ROW` directly instead of `EQUATION`.

### Validation workflow

1. Write the JSON.
2. Validate it with any JSON parser.
3. Open it with `yutovo-desktop` (or `yutovo-editor`) to confirm it loads without `document not parsed`.
4. Check the log for `Parser exception` — those are formula/solver errors, not loading errors.

### Storage

- The editor saves `.yut` as gzip-compressed JSON by default.
- Files in this repository are stored as plain JSON for git diffability. Both formats load successfully.

## Library Directory Structure

```
library/
├── ru/          # Russian (language=2)
├── en/          # English (language=1)
├── es/          # Spanish (language=3)
└── pt_BR/       # Portuguese (Brazil) (language=4)
```

Each language directory mirrors the same logical structure:
- `Help/Calculations/` — help articles about calculations
- `Calculators/` — calculator documents
- `Others/` — misc documents like "First page"

Directory and file names are localized, e.g. Russian `Справка/` corresponds to English `Help/`.

## `.order` Files

Each directory may contain a `.order` file listing folder/document names in display order:
- Folder names are listed without extension
- Document names are listed **with** `.yut` extension
- Order matters — it defines the navigation sequence in the app
- Folder names inside `.order` must use the localized names of the target language

## Translation Rules

1. **Translate only human-readable text.**
   - `type: 4` (`STRING`) `elements` is a single string containing the actual text to translate.
   - `type: 39` (`LINK`) `elements` is also a single string and must be translated.
2. **Preserve JSON structure, IDs, formats, and all numeric values exactly.**
3. **Update `config.language`** to match the target language code.
4. **Generate a new `file_guid`** for every translated file — never reuse the source UUID.
   - Place the translated file into the corresponding language directory (`en/`, `ru/`, `es/`, `pt_BR/`) and update `config.language` accordingly.
5. **Translate navigation paths:** type 39 `url` fields must point to the translated document names.
   - Leave external URLs (starting with `http://`, `https://`) unchanged; only translate their link text.
6. **Translate `paragraph_formats` names** and corresponding `format_name` values in `text.elements` to the target language.
   - The canonical English names are translated by the editor on load, but the JSON must contain the localized name for the target `config.language`.
7. **Keep `format_id` values unchanged.** They reference `string_formats` entries by UUID.
8. **Preserve `caret` and `selection` fields** as-is or reset them to a simple empty state if the source has active selections.
9. **Use the correct imaginary unit symbol.** In Russian documents the imaginary unit is written as `j`; in non-Russian documents (`en`, `es`, `pt_BR`) it must be changed to `i`. Update any literal `j` in `CODE_STRING` or text content that represents the imaginary unit when translating.

## Navigation Style

Navigation links are `type: 39` elements appended at the end of `text.elements` (after an empty paragraph):

- **Within-section links**: use `./Document.yut` or `./Folder/Document.yut`
- **Cross-section links**: use `../Section/Document.yut`

Navigation text format:
- Previous doc: `<- Document name` or `<- Section/Document name`
- Next doc: `Document name ->` or `Section/Document name ->`

### Navigation rules

1. **Link order must follow `.order`.** The previous link must point to the document listed immediately before the current document in the containing directory's `.order` file, and the next link must point to the document listed immediately after it.
2. **Middle documents must have two navigation links** (previous and next). Only the first and the last document in a section may have a single link — the first has only "next", the last has only "previous".
3. **All help documents must be reachable through navigation links.** Every help article must link to its predecessor and successor (except section boundaries), forming a connected navigation chain through the whole help tree. There must be no gaps in the chain.
4. **Preserve formatting when editing navigation links.** When updating navigation link text or `url`, keep the original `format_id`, `level`, `format_name`, paragraph alignment, and the exact spacing (whitespace) between links. Change only the `elements` text and the `url` of the `type: 39` link elements.
5. **Special order consistency.** If `.order` lists `Terms of use` before `Privacy policy` (or their localized equivalents), the navigation links between them must reflect that order: `Terms of use` is the previous document and `Privacy policy` is the next.
6. **Middle documents always have two links.** A document that is neither the first nor the last in its section (for example, `Calculator block`) must have both a previous and a next navigation link.
7. **Internal links must target ordered documents.** Any internal `type: 39` link (not starting with `http://` or `https://`) should point to a document that is listed in the `.order` file of the target directory. Documents that are not listed in `.order` should be removed or added to `.order` after review.

### Paragraph Format Translations

| Russian | English | Spanish | Portuguese (pt_BR) |
|---------|---------|---------|-------------------|
| Основной текст | Text body | Cuerpo de texto | Corpo do texto |
| Заголовок 1 | Header 1 | Encabezado 1 | Cabeçalho 1 |
| Заголовок 2 | Header 2 | Encabezado 2 | Cabeçalho 2 |
| Заголовок 3 | Header 3 | Encabezado 3 | Cabeçalho 3 |
| Заголовок 4 | Header 4 | Encabezado 4 | Cabeçalho 4 |
| Пример | Example | Ejemplo | Exemplo |
| Моноширинный | Monospace | Monoespaciado | Monoespaçado |
| Код | Code | Código | Código |

## `.in` Preprocessor Files

When merging documents from the `web` branch into `library`, use `.yut.in` files to keep both variants in a single file. A `.yut.in` file is a plain-text JSON document with C-style preprocessor directives (`#ifdef WEB`, `#else`, `#endif`). The consumer chooses one branch at build/load time and removes the other.

### Rules for creating `.yut.in` files

1. **Base the file on the `library` version.**
   - `file_guid` comes from `library` (differences in `file_guid` are ignored).
   - `config`, `string_formats`, `paragraph_formats`, `caret`, and `selection` come from `library`.
2. **Wrap only paragraph-level differences.**
   - Compare `text.elements` paragraph by paragraph (ignoring element `id` values).
   - For each paragraph that differs, replace it with:
     ```json
     #ifdef WEB
     { ... paragraph as it appears in web ... }
     #else
     { ... paragraph as it appears in library ... }
     #endif
     ```
   - Paragraphs that are identical (ignoring `id`) are kept unchanged from `library`.
3. **Different paragraph counts.**
   - If `web` and `library` have a different number of paragraphs in `text.elements`, wrap the whole `text.elements` array as a single block instead of individual paragraphs.
4. **Broken JSON in `web`.**
   - If a file in `web` is not valid JSON (e.g. duplicated lines), repair it before generating the `.in` file.
5. **Replace the original `.yut`.**
   - The original `File.yut` is removed and replaced by `File.yut.in`.

### Preprocessor semantics

- `#ifdef WEB` keeps the block that follows when building for `web`.
- `#else` keeps the block that follows for any non-web build (i.e. `library`).
- `#endif` ends the conditional block.
- The resulting file after preprocessing must be valid JSON.

## Workflow Rules

- **Never run `git commit`, `git push`, `git reset`, `git rebase` or any git mutations unless explicitly asked to do so.** Always ask for confirmation before committing.
- **After modifying documents, verify all internal links.** Any `type: 39` (`LINK`) element whose `url` does not start with `http://` or `https://` must point to an existing `.yut` file (or `.yut.in` file) relative to the document's location. Report or fix broken links before finishing.
- **Internal links must target documents that are listed in the corresponding `.order` file.** When adding or updating a link, ensure the destination document appears in the `.order` file of its containing directory. If a referenced document is missing from `.order`, report it.

## Language Codes Reference

| Language | Code |
|----------|------|
| English (en) | 1 |
| Russian (ru) | 2 |
| Spanish (es) | 3 |
| Portuguese Brazil (pt_BR) | 4 |
