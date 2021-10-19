# -*- coding: utf-8 -*-
#
# Configuration file for the Sphinx documentation builder.
#
# This file does only contain a selection of the most common options. For a
# full list see the documentation:
# http://www.sphinx-doc.org/en/master/config

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'QLiteHtmlBrowser'
copyright = '2021, JK'
author = 'JK'

# The short X.Y version
version = ''
# The full version, including alpha/beta/rc tags
release = ''


# -- General configuration ---------------------------------------------------

# If your documentation needs a minimal Sphinx version, state it here.
#
# needs_sphinx = '1.0'

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = ["sphinxcontrib.needs"
            , 'sphinx.ext.autosectionlabel'
            , 'sphinx.ext.ifconfig'
            , 'breathe'
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# The master toctree document.
master_doc = 'index'
language = 'en'

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

html_theme = 'alabaster'
html_static_path = ['_static']

needs_id_length = 5

doxygen_build_dir = f"{os.getcwd()}/_build/xml"

if os.path.isdir(doxygen_build_dir):
    breathe_projects = { "QLiteHtmlBrowser": doxygen_build_dir }
    breathe_default_project = "QLiteHtmlBrowser"
else:
    raise Exception(f"required {doxygen_build_dir} not generated, first run build process")

needs_types = [
    dict(directive="req", title="Requirement", prefix="R_", color="#BFD8D2", style="node"),
    dict(directive="spec", title="Specification", prefix="S_", color="#FEDCD2", style="node"),
    dict(directive="impl", title="Implementation", prefix="I_", color="#DF744A", style="node"),
    dict(directive="test", title="Test Case", prefix="T_", color="#DCB239", style="node"),
]

needs_types += [
    dict(directive="cr", title="Change Request", prefix="CR_", color="#DCB239", style="node"),
    dict(directive="bug", title="Bug", prefix="BUG_", color="#DCB239", style="node")
]

needs_statuses = [
    dict(name="open", description="Nothing done yet"),
    dict(name="in progress", description="Someone is working on it"),
    dict(name="implemented", description="Work is done and implemented"),
]

needs_default_layout = "complete"

library_name = project
library_widget_name = project

rst_epilog = """
.. |library_name| replace:: %s
.. |library_widget_name| replace:: %s
""" % (library_name, library_name)

#def setup(app):
#    app.add_config_value('library_name', library, 'env')
#    app.add_config_value('library_name_uc', library.upper(), 'env')
#    app.add_config_value('library_name_lc', library.lower(), 'env')
