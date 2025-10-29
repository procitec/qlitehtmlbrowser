#pragma once

#include <litehtml.h>
#include <QtCore/QDebug>

namespace TextHelpers
{

struct TextFragment
{
  std::string            text;
  litehtml::element::ptr element;
  litehtml::position     pos;
  int                    start_char_offset; // Offset in element text
  int                    end_char_offset; // end Offset in element text
  int                    global_start_offset;
  int                    global_end_offset;
  bool                   is_leaf;

  bool hasValidPosition() const { return pos.width > 0 && pos.height > 0; }
};

struct TextFindMatch
{
  std::string               matched_text;
  std::vector<TextFragment> fragments; // may contains multiple elements
  litehtml::position        bounding_box; // bounding box over all elements
};

struct TextPosition
{
  litehtml::element::ptr element;
  int                    char_offset; // Zeichenposition im Element-Text
  litehtml::position     element_pos;

  bool operator<( const TextPosition& other ) const
  {
    // Vergleich basierend auf Position im Dokument
    if ( element_pos.y != other.element_pos.y )
    {
      return element_pos.y < other.element_pos.y;
    }
    return element_pos.x < other.element_pos.x;
  }

  bool isValid() const { return element != nullptr; }
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

class DOMTextManager
{
private:
  std::vector<TextFragment> m_fragments;
  std::string               m_fullText;

public:
  void buildFromDocument( litehtml::document::ptr doc )
  {
    m_fragments.clear();
    m_fullText.clear();

    if ( doc && doc->root() )
    {
      collectTextFragments( doc->root() );

      qDebug() << "\n=== Gesammelte Text-Fragmente ===";
      qDebug() << "Anzahl Fragmente:" << m_fragments.size();
      for ( size_t i = 0; i < m_fragments.size(); ++i )
      {
        const auto& frag = m_fragments[i];
        qDebug() << "Fragment" << i << ":"
                 << "Text:" << QString::fromStdString( frag.text ).left( 30 ) << "| Leaf:" << frag.is_leaf << "| Pos:" << frag.pos.x << frag.pos.y
                 << "| Size:" << frag.pos.width << "x" << frag.pos.height << "| Valid:" << frag.hasValidPosition();
      }
      qDebug() << "================================\n";
    }
  }

  // Findet die Text-Position basierend auf Pixel-Koordinaten
  TextPosition getPositionAtCoordinates( int x, int y )
  {
    TextPosition  result;
    TextFragment* bestMatch    = nullptr;
    int           smallestArea = INT_MAX;

    qDebug() << "\n>>> Suche Element bei:" << x << y;

    // Suche das kleinste Element, das den Punkt enthält (spezifischstes Element)
    for ( auto& fragment : m_fragments )
    {
      // Nur Leaf-Nodes und Elemente mit gültiger Position prüfen
      if ( !fragment.is_leaf || !fragment.hasValidPosition() )
      {
        continue;
      }

      const auto& pos = fragment.pos;

      // Prüfe ob Punkt im Element liegt
      if ( x >= pos.x && x <= pos.x + pos.width && y >= pos.y && y <= pos.y + pos.height )
      {

        int area = pos.width * pos.height;

        qDebug() << "  Kandidat:" << QString::fromStdString( fragment.text ).left( 20 ) << "| Box:" << pos.x << pos.y << pos.width << pos.height
                 << "| Area:" << area;

        // Wähle das kleinste umschließende Element
        if ( area < smallestArea )
        {
          smallestArea = area;
          bestMatch    = &fragment;
        }
      }
    }

    if ( bestMatch )
    {
      result.element     = bestMatch->element;
      result.element_pos = bestMatch->pos;

      // Berechne Zeichen-Offset basierend auf x-Position
      const auto& pos = bestMatch->pos;
      if ( pos.width > 0 && bestMatch->text.length() > 0 )
      {
        float relativeX    = static_cast<float>( x - pos.x ) / pos.width;
        result.char_offset = static_cast<int>( relativeX * bestMatch->text.length() );
        result.char_offset = std::max( 0, std::min( result.char_offset, static_cast<int>( bestMatch->text.length() ) ) );
      }
      else
      {
        result.char_offset = 0;
      }

      qDebug() << ">>> TREFFER:" << QString::fromStdString( bestMatch->text ).left( 30 ) << "| Offset:" << result.char_offset;
    }
    else
    {
      qDebug() << ">>> KEIN TREFFER!";
    }

    return result;
  }

  // Extrahiert Text und Fragmente zwischen zwei Positionen
  SelectionRange getSelectionBetween( const TextPosition& start, const TextPosition& end )
  {
    SelectionRange selection;
    selection.start = start;
    selection.end   = end;

    if ( !start.isValid() || !end.isValid() )
    {
      return selection;
    }

    // Sicherstellen dass start vor end liegt
    TextPosition actualStart = start;
    TextPosition actualEnd   = end;
    if ( end < start )
    {
      std::swap( actualStart, actualEnd );
    }

    bool inSelection = false;

    // Nur durch Leaf-Nodes iterieren
    for ( const auto& fragment : m_fragments )
    {
      if ( !fragment.is_leaf || !fragment.hasValidPosition() )
      {
        continue;
      }

      bool isStartFragment = ( fragment.element == actualStart.element );
      bool isEndFragment   = ( fragment.element == actualEnd.element );

      if ( isStartFragment )
      {
        inSelection = true;
      }

      if ( inSelection )
      {
        TextFragment selectedFragment = fragment;
        int          startOffset      = 0;
        int          endOffset        = fragment.text.length();

        if ( isStartFragment )
        {
          startOffset = actualStart.char_offset;
        }

        if ( isEndFragment )
        {
          endOffset = actualEnd.char_offset;
        }

        selectedFragment.start_char_offset = startOffset;
        selectedFragment.end_char_offset   = endOffset;

        selection.fragments.push_back( selectedFragment );

        if ( isEndFragment )
        {
          break;
        }
      }
    }

    qDebug() << "Selektion umfasst" << selection.fragments.size() << "Fragmente";

    return selection;
  }

  const std::vector<TextFragment>& getFragments() const { return m_fragments; }
private:
  std::string normalizeWhitespace( const std::string& text )
  {
    std::string result;
    bool        lastWasSpace = false;

    for ( char c : text )
    {
      if ( std::isspace( static_cast<unsigned char>( c ) ) )
      {
        if ( !lastWasSpace && !result.empty() )
        {
          result += ' ';
          lastWasSpace = true;
        }
      }
      else
      {
        result += c;
        lastWasSpace = false;
      }
    }

    return result;
  }

  void collectTextFragments( litehtml::element::ptr el )
  {
    if (!el) return;

    // Prüfe ob Element Kinder hat
    bool hasChildren = !el->children().empty();

    // Text aus dem Element holen
    std::string text;
    el->get_text( text );

    // NUR Leaf-Nodes (Elemente ohne Kinder) mit Text speichern
    // Dadurch vermeiden wir, dass Parent-Elemente den gesamten
    // aggregierten Text ihrer Kinder zurückgeben
    if ( !hasChildren && !text.empty() )
    {
      litehtml::position pos = el->get_placement();

      // Nur wenn Element eine gültige Position hat
      if ( pos.width > 0 && pos.height > 0 )
      {
        TextFragment fragment;
        fragment.text                = normalizeWhitespace( text );
        fragment.element             = el;
        fragment.pos                 = pos;
        fragment.start_char_offset   = 0;
        fragment.end_char_offset     = fragment.text.length();
        fragment.global_start_offset = m_fullText.length();
        fragment.is_leaf             = true;

        m_fullText += fragment.text + " ";
        fragment.global_end_offset = m_fullText.length();

        m_fragments.push_back( fragment );

        qDebug() << "Leaf-Fragment:" << QString::fromStdString( fragment.text ).left( 30 ) << "| Pos:" << pos.x << pos.y << "| Size:" << pos.width
                 << "x" << pos.height;
      }
    }

    // Rekursiv durch alle Kindelemente
    for ( auto it = el->children().begin(); it != el->children().end(); ++it )
    {
      collectTextFragments( *it );
    }
  }
};
}
