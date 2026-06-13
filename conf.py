import sys
from os.path import dirname, join

sys.path.insert(0, join(dirname(__file__), ".."))

project = "Microsoft.GSL"
copyright = "2024, Microsoft"
author = "Microsoft"
release = "4.2.0"

extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.doctest",
    "sphinx.ext.napoleon",
    "sphinx_autodoc_typehints",
]

# ---------------------------------------------------------------------------
# sphinx-autodoc-typehints: return-type deduplication / rendering control
# ---------------------------------------------------------------------------
#
# The three options that control return-type rendering are mutually related;
# enabling more than one of them will cause Sphinx to emit the return type
# twice (or three times). Here is the exact interaction:
#
#   (1) napoleon_use_rtype (default True)
#       | Controls whether napoleon converts the docstring's ``:rtype:`` /
#       | ``Returns:`` block into a reStructuredText ``:rtype:`` field.
#       | When True, napoleon already produces the visual "Returns: int"
#       | block that autodoc then keeps.
#
#   (2) typehints_use_rtype  (default False — in this extension)
#       | When True, sphinx-autodoc-typehints injects a reStructuredText
#       | ``:rtype:`` field into each signature. autodoc then renders it
#       | as the "Returns" block of the docstring.
#
#   (3) typehints_document_rtype (default True)
#       | When True, sphinx-autodoc-typehints injects the return annotation
#       | directly into the signature line (``func() -> int``). This is the
#       | modern, PEP-484-style rendering.
#
# The two common "valid" combinations (non-duplicated output) are:
#
#   A) Signature-only style (recommended for modern projects)
#       typehints_document_rtype = True    # keep "-> T" in signature line
#       typehints_use_rtype      = False   # DO NOT also emit :rtype:
#       napoleon_use_rtype       = False   # DO NOT emit :rtype: from docstring
#
#   B) Docstring-only style
#       typehints_document_rtype = False
#       typehints_use_rtype      = True    # emit ":rtype: T" in docstring
#       napoleon_use_rtype       = False   # but not a second one from napoleon
#
# Any other combination produces a duplicate "Returns" block:
#     typehints_use_rtype = True AND typehints_document_rtype = True
#     => return type shown in signature AND in a separate "Returns" block.
#     napoleon_use_rtype = True AND typehints_use_rtype = True
#     => two separate "Returns" blocks emitted from different sources.
#
# We pick style (A) because it matches PEP-484's `-> T` rendering and keeps
# the docstring clean.
typehints_document_rtype = True
typehints_use_rtype = False
napoleon_use_rtype = False

# ---------------------------------------------------------------------------
# typehints_fixup_module_name — fix broken ``threading.local`` link
# ---------------------------------------------------------------------------
#
# `threading.local` is an alias for the real class `_thread._local`.
# sphinx-autodoc-typehints normalises annotations to their canonical module
# path via ``inspect.getmodule()``, which returns ``_thread`` rather than
# ``threading``. As a result the generated cross-reference becomes
# :class:`_thread._local`, which is not a documented public API and does
# not have an intersphinx mapping. The rendered link therefore points to a
# non-existent target.
#
# ``typehints_fixup_module_name`` is a mapping of
#     ``(qualified_name, ) -> replacement_qualified_name``
# that is applied *after* the automatic module-name normalisation. We use it
# here to rewrite ``_thread._local`` back to ``threading.local`` so the
# autodoc / intersphinx reference points to the documented public API.
typehints_fixup_module_name = {
    "_thread._local": "threading.local",
}

# napoleon also supports Google-style docstrings (kept for completeness).
napoleon_google_docstring = True
napoleon_numpy_docstring = False

autodoc_member_order = "bysource"
autodoc_typehints = "description"
