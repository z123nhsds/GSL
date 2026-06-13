project = "GSL"
copyright = "Microsoft Corporation. All rights reserved."
author = "Microsoft"

release = "4.2.0"
version = "4.2.0"

extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.napoleon",
    "sphinx_autodoc_typehints",
]

templates_path = ["_templates"]
exclude_patterns = ["_build"]

html_theme = "sphinx_rtd_theme"

# ── sphinx_autodoc_typehints ──────────────────────────────────────────
#
# typehints_use_rtype (default: True)
#   When True, sphinx-autodoc-typehints reads the return annotation from
#   the function signature and may inject an ":rtype:" field into the
#   description.  When False, the extension ignores the return annotation
#   entirely and leaves return-type documentation to other extensions
#   (e.g. napoleon).
#
# typehints_document_rtype (default: True)
#   When True, the extension emits an ":rtype:" directive in the
#   description body for every function that has a return annotation.
#   When False, the return type is suppressed from the description even
#   if the annotation exists.
#
# napoleon_use_rtype (default: True)
#   When True, napoleon parses the "Returns:" section of a Google-style
#   docstring and emits an ":rtype:" field.  When False, napoleon omits
#   the ":rtype:" and only renders the return description text.
#
# ── Mutual exclusion / duplication problem ────────────────────────────
#
# If both napoleon and sphinx-autodoc-typehints are active and all three
# options are True (the defaults), the return type appears TWICE:
#   1. napoleon emits ":rtype:" from the docstring "Returns:" section.
#   2. sphinx-autodoc-typehints emits another ":rtype:" from the
#      annotation.
#
# The fix is to let exactly ONE source own the return type:
#
#   Option A – napoleon owns the return type:
#     typehints_use_rtype      = False
#     typehints_document_rtype = False
#     napoleon_use_rtype       = True
#     Rationale: napoleon renders the rich "Returns:" description from
#     the docstring which often includes semantic context; the raw
#     annotation is redundant.
#
#   Option B – autodoc-typehints owns the return type:
#     typehints_use_rtype      = True
#     typehints_document_rtype = True
#     napoleon_use_rtype       = False
#     Rationale: type annotations are the single source of truth and
#     are always in sync with the code; docstring return types can
#     drift.
#
# We choose Option B so that the type annotation (which the compiler
# validates) is the authoritative source.

typehints_use_rtype = True
typehints_document_rtype = True
napoleon_use_rtype = False

# ── typehints_fixup_module_name ───────────────────────────────────────
#
# CPython's threading.local is implemented in C as _thread._local.
# Sphinx resolves the qualified name via obj.__module__ which yields
# "_thread" for the C implementation, causing intersphinx to link
# threading.local to _thread._local instead of the public
# threading.local API.
#
# typehints_fixup_module_name remaps the internal module name to the
# public one so that :class:`threading.local` links correctly.

typehints_fixup_module_name = {
    "_thread": "threading",
}

autodoc_default_options = {
    "members": True,
    "undoc-members": True,
    "show-inheritance": True,
}
