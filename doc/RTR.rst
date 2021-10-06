============
Requirements
============

This documents describes the requirements of the QLiteHtmlBrowser Library.

.. req:: Render HTML with litehtml
    :id: R_0001

    The Library should be used to display HTML Code with the `litehtml <https://github.com/litehtml/litehtml/>`_
    library. The litehtml library should be linked to the library. The feature set of the litehtml
    library should be fully supported by the library, this requires implementing all
    interfaces of the `litehtml document container <https://github.com/litehtml/litehtml/wiki/document_container/>`_


.. req:: QWidget embed
    :id: R_0002

    The Library should provide a class derived from QWidget to
    embed the HTML View into a Qt Application / Widget. The widget
    muss follow the principal design patterns for Qt (i.e. Parenting,
    Signal/Slots).

.. req:: Display HTML code
    :id: R_0003

    The main QLiteHtmlBrowser Widget must provide a interface to set HTML code as an string.
    The code should be rendered, embedded content should be displayed as possible

.. req:: Display Content from URL
    :id: R_0004

    The main QLiteHtmlBrowser Widget must provide a interface to set an URL.
    The URL should be resolved, loaded and content should be displayed as possible.
    URLs to local content (e.g. "file://") or to local embedded content (e.g. local
    ".css" stylesheet files) should be resolved by the implementation as possible.
    
.. req:: Current URL provision
    :id: R_0005

    The main QLiteHtmlBrowser must provide the current URL if changed e.g. by following
    a link or an anchor. The current url must be provided by sending an Qt signal.
    The current URL should also provided by the QLiteHtmlBrowser main widgets interface.

.. req:: Resource Loading Hierarchy
    :id: R_0006


    The main QLiteHtmlBrowser Widget must provide an interface to load resources from
    other locations than local files. The main widget should load resources as possible
    and must call the interface if no resources could get loaded to give the embedding widget
    a possiblity to load the content from the given resource. The Interface must provide
    a full URL. The content must be returned as bytes
    
    * Textual Resources are expected to be UTF-8 encoded
    * Image Resources are expected to be Pixmaps.

.. req:: Scrolling
    :id: R_0007


    The main QLiteHtmlBrowser should provide scrollbars to view the complete content.
    The scrollbar handling must be implemented in the main QLiteHtmlBrowser Widget.
    
.. req:: Zoom In/Out
    :id: R_0008


    The main QLiteHtmlBrowser should provide a possiblity to Zoom the content.
    This includes the complete content, not only parts of the content like only the text.
    The Zoom should be possible via shortcuts and mouse wheel like in Firefox.
    
    This includes:
    
    * <STRG>+MouseWheel
    * <STRG>+0 to undo zoom and set the default (100%) zoom level

    The zoom is limited, the main QLiteHtmlBrowser should provide an interface to set the limits.

.. req:: Anchor Scrolling
    :id: R_0009
    :links: R_0004
    
    If the URL provided contains an Anchor (e.g. like '#AnchorName'), the Widget should resolve
    the anchor in the HTML code and scroll to the content to the anchor.

.. req:: Link support in HTML
    :id: R_0010
    :links: R_0004, R_0005

    If the HTML code contains a link, the link should be activated by a mouse click. The URL of
    the link should be loaded and displayed if possible. The actived link should be provided.


