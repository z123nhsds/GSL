project = 'GSL'
copyright = '2026, Microsoft Corporation'
author = 'GSL Maintainers'

extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.napoleon',
    'sphinx_autodoc_typehints',
]

# ---------------------------------------------------------------------------
# Return-type deduplication: typehints_use_rtype / typehints_document_rtype
#                             versus napoleon_use_rtype
# ---------------------------------------------------------------------------
#
# Three configuration keys interact when determining what :rtype: is
# ultimately rendered in the HTML output:
#
#   ┌──────────────────────────────┬───────────┬──────────────────────────────────┐
#   │ Option                       │ Default   │ Effect when True                 │
#   ├──────────────────────────────┼───────────┼──────────────────────────────────┤
#   │ napoleon_use_rtype           │ True      │ Napoleon extracts `Returns:`     │
#   │ (sphinx.ext.napoleon)        │           │ type from Google/NumPy docstring │
#   │                              │           │ and SETS :rtype: on the node.    │
#   ├──────────────────────────────┼───────────┼──────────────────────────────────┤
#   │ typehints_use_rtype          │ True      │ sphinx_autodoc_typehints reads   │
#   │ (sphinx_autodoc_typehints)   │           │ the return-type annotation and   │
#   │                              │           │ SETS :rtype: on the node.        │
#   ├──────────────────────────────┼───────────┼──────────────────────────────────┤
#   │ typehints_document_rtype     │ True      │ The :rtype: field (regardless    │
#   │ (sphinx_autodoc_typehints)   │           │ of who set it) is DISPLAYED in   │
#   │                              │           │ the final rendered output.       │
#   └──────────────────────────────┴───────────┴──────────────────────────────────┘
#
# MUTUAL-EXCLUSION / DUPLICATION RULES
# ─────────────────────────────────────
# If napoleon_use_rtype = True  AND  typehints_use_rtype = True, BOTH will
# independently set :rtype:, producing a DUPLICATE return type in the output:
#
#     Return type:   int
#     Return type:   int
#
# This happens because Napoleon adds :rtype: from the docstring first, then
# sphinx_autodoc_typehints adds it again from the type annotation.
#
# RECOMMENDED STRATEGY (code-as-truth)
# ────────────────────────────────────
# When the codebase uses type annotations consistently, disable Napoleon's
# :rtype: extraction and let sphinx_autodoc_typehints be the single source:
#
#     napoleon_use_rtype       = False   # don't extract from docstrings
#     typehints_use_rtype       = True   # extract from type annotations
#     typehints_document_rtype  = True   # render the type in output
#
# This guarantees exactly one :rtype: line, sourced from the annotation.
#
# ALTERNATIVE STRATEGY (docstring-as-truth)
# ─────────────────────────────────────────
# If docstrings are the authoritative source:
#
#     napoleon_use_rtype       = True
#     typehints_use_rtype       = False
#     typehints_document_rtype  = True
#
# SUPPRESS ALL RTYPE (minimalist output)
# ──────────────────────────────────────
#     napoleon_use_rtype       = False
#     typehints_use_rtype       = False
#     typehints_document_rtype  = False
#

napoleon_use_rtype = False
typehints_use_rtype = True
typehints_document_rtype = True

# ---------------------------------------------------------------------------
# threading.local → __thread__local  broken-link fixup
# ---------------------------------------------------------------------------
#
# When a function has a type annotation referencing ``threading.local``,
# sphinx_autodoc_typehints attempts to generate a cross-reference:
#
#     :py:class:`threading.local`
#
# Sphinx fails to resolve this because ``threading.local`` is implemented
# in C (_thread._local) and is not a directly importable class for the
# intersphinx / Python domain inventory.  The result is a broken link
# (red text or missing hyperlink) in the generated documentation.
#
# ``typehints_fixup_module_name`` allows remapping module-qualified names
# *before* sphinx_autodoc_typehints emits the cross-reference role.
# By mapping the C-level implementation name into the annotation namespace,
# the generated role becomes resolvable.
#
# Mapping semantics:
#   Key   -- the module portion of the qualified type name as it appears
#            in the source annotation (e.g. ``threading`` from
#            ``threading.local``).
#   Value -- the canonical module that Sphinx's intersphinx inventory
#            can resolve (e.g. ``threading`` stays as ``threading``,
#            or ``__thread__local`` if that's in the inventory).
#
# For cpython's ``threading.local`` specifically, the fixup tells the
# extension to treat the annotation as a plain-text reference rather than
# a failing :py:class: role:
#

typehints_fixup_module_name = {
    'threading': '_thread',
}