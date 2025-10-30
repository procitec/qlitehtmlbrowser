#pragma once

#include <litehtml.h>
#include <QtCore/QDebug>

class TextManager
{
public:
  struct TextFragment
  {
    std::string            text;
    litehtml::element::ptr element;
    litehtml::position     pos;
    int                    start_char_offset; // Offset in element text
    int                    end_char_offset;   // end Offset in element text
    int                    global_start_offset;
    int                    global_end_offset;
    bool                   is_leaf;
    litehtml::element::ptr parent_element;

    std::string element_tag;      // z.B. "p", "div", "span", "br"
    bool        is_block_element; // true für Block-Level-Elemente

    bool hasValidPosition() const { return pos.width > 0 && pos.height > 0; }
  };

  struct TextFindMatch
  {
    std::string               matched_text;
    std::vector<TextFragment> fragments;
    litehtml::position        bounding_box;

    bool isEmpty() const { return fragments.empty(); }
  };

  struct TextPosition
  {
    litehtml::element::ptr element;
    int                    char_offset = 0;
    litehtml::position     element_pos;

    bool operator<( const TextPosition& other ) const
    {
      if ( element_pos.y != other.element_pos.y )
        return element_pos.y < other.element_pos.y;
      if ( element_pos.x != other.element_pos.x )
        return element_pos.x < other.element_pos.x;
      return char_offset < other.char_offset;
    }
    bool isValid() const { return element && element_pos.width > 0 && element_pos.height > 0; }
  };

  struct SelectionRange
  {
    TextPosition              start;
    TextPosition              end;
    std::vector<TextFragment> fragments; // Alle Textfragmente in der Selektion

    bool isEmpty() const { return !start.isValid() || !end.isValid(); }

    void clear()
    {
      start = TextPosition();
      end   = TextPosition();
      fragments.clear();
    }
  };

  enum class TextFormatMode
  {
    Plain,      // Nur Leerzeichen zwischen Fragmenten
    Structured, // Kontextsensitiv: Berücksichtigt Block-Elemente
    Html        // Behält HTML-Struktur bei (für zukünftige Erweiterung)
  };

private:
  std::vector<TextFragment> m_fragments;
  std::string               m_fullText;

public:
  void                       buildFromDocument( litehtml::document::ptr doc );
  TextPosition               getPositionAtCoordinates( int x, int y );
  SelectionRange             getSelectionBetween( const TextPosition& start, const TextPosition& end );
  std::vector<TextFindMatch> findText( const std::string& search_term, bool case_sensitive = true );
  std::string                selectedText( const SelectionRange& selection, TextFormatMode mode = TextFormatMode::Structured ) const;

private:
  void               collectTextFragments( litehtml::element::ptr el );
  litehtml::position calculateBoundingBox( int searchStart, int searchEnd, std::vector<TextFragment>& matchedFragments );
  bool               isBlockLevelElement( const std::string& tag ) const;
  std::string        getElementTag( litehtml::element::ptr element ) const;
  bool               shouldInsertNewline( const TextFragment& prev, const TextFragment& current ) const;
};
