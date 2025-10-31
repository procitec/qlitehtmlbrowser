#include "TextManager.h"
#include <litehtml.h>
#include <QtCore/QDebug>

void TextManager::buildFromDocument( litehtml::document::ptr doc )
{
  m_fragments.clear();
  m_fullText.clear();

  if ( doc && doc->root() )
  {
    collectTextFragments( doc->root() );
    if ( !m_fullText.empty() && m_fullText.back() == ' ' )
    {
      m_fullText.pop_back();
    }

    // qDebug() << "\n=== DOM Text Manager initialisiert ===";
    // qDebug() << "Fragmente gesammelt:" << m_fragments.size();
    // qDebug() << "Gesamttext-Länge:" << m_fullText.length();

    // qDebug() << "\n=== Gesammelte Text-Fragmente ===";
    // qDebug() << "Anzahl Fragmente:" << m_fragments.size();
    // for ( size_t i = 0; i < m_fragments.size(); ++i )
    // {
    //   const auto& frag = m_fragments[i];
    //   qDebug() << "Fragment" << i << ":"
    //            << "Text:" << QString::fromStdString( frag.text ).left( 30 ) << "| Leaf:" << frag.is_leaf << "| Pos:" << frag.pos.x << frag.pos.y
    //            << "| Size:" << frag.pos.width << "x" << frag.pos.height << "| Valid:" << frag.hasValidPosition();
    // }
    // qDebug() << "================================\n";
  }
}

TextManager::TextPosition TextManager::getPositionAtCoordinates( int x, int y )
{
  TextPosition  result;
  TextFragment* bestMatch    = nullptr;
  int           smallestArea = INT_MAX;

  for ( auto& fragment : m_fragments )
  {
    if ( !fragment.is_leaf || !fragment.hasValidPosition() )
    {
      continue;
    }

    const auto& pos = fragment.pos;

    if ( x >= pos.x && x <= pos.x + pos.width && y >= pos.y && y <= pos.y + pos.height )
    {

      int area = pos.width * pos.height;

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
  }

  return result;
}

// Extrahiert Text und Fragmente zwischen zwei Positionen
TextManager::SelectionRange TextManager::getSelectionBetween( const TextManager::TextPosition& start, const TextManager::TextPosition& end )
{
  SelectionRange selection;
  selection.start = start;
  selection.end   = end;

  if ( !start.isValid() || !end.isValid() )
  {
    return selection;
  }

  TextPosition actualStart = start;
  TextPosition actualEnd   = end;
  if ( end < start )
  {
    std::swap( actualStart, actualEnd );
  }

  bool inSelection = false;

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

      // WICHTIG: Position anpassen
      litehtml::position adjustedPos = fragment.pos;
      if ( fragment.text.length() > 0 && fragment.pos.width > 0 )
      {
        float charWidth   = static_cast<float>( fragment.pos.width ) / static_cast<float>( fragment.text.length() );
        adjustedPos.x     = fragment.pos.x + static_cast<int>( charWidth * startOffset );
        adjustedPos.width = static_cast<int>( charWidth * ( endOffset - startOffset ) );
      }
      selectedFragment.pos = adjustedPos;

      selection.fragments.push_back( selectedFragment );

      if ( isEndFragment )
      {
        break;
      }
    }
  }

  return selection;
}

std::vector<TextManager::TextFindMatch> TextManager::findText( const std::string& search_term, bool case_sensitive )
{
  std::vector<TextFindMatch> results;
  if ( search_term.empty() || m_fullText.empty() )
  {
    return results;
  }

  std::string searchText = m_fullText;
  std::string searchFor  = search_term;

  if ( !case_sensitive )
  {
    std::transform( searchText.begin(), searchText.end(), searchText.begin(), ::tolower );
    std::transform( searchFor.begin(), searchFor.end(), searchFor.begin(), ::tolower );
  }

  qDebug() << "\n=== Suche ===";
  qDebug() << "Suchbegriff:" << QString::fromStdString( searchFor );
  qDebug() << "Durchsuche m_fullText (RAW):" << QString::fromStdString( searchText ).left( 100 );

  // Alle Vorkommen finden
  size_t pos = 0;
  while ( ( pos = searchText.find( searchFor, pos ) ) != std::string::npos )
  {
    TextFindMatch result;
    result.matched_text = m_fullText.substr( pos, searchFor.length() ); // Jetzt korrekt!

    int searchStart = static_cast<int>( pos );
    int searchEnd   = static_cast<int>( pos + searchFor.length() );

    qDebug() << "Treffer bei:" << searchStart << "-" << searchEnd << "| Text:" << QString::fromStdString( result.matched_text );

    // Finde Fragmente in diesem Bereich (jetzt mit korrekten Positionen)
    result.bounding_box = calculateBoundingBox( searchStart, searchEnd, result.fragments );

    results.push_back( result );
    pos += searchFor.length();
  }

  qDebug() << "Treffer gesamt:" << results.size() << "\n";
  return results;
}

void TextManager::collectTextFragments( litehtml::element::ptr el )
{
  if ( !el )
    return;

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

    if ( pos.width > 0 && pos.height > 0 )
    {
      TextFragment fragment;
      fragment.text                = text; // Original-Text
      fragment.parent_element      = el->parent();
      fragment.element             = el;
      fragment.pos                 = pos;
      fragment.start_char_offset   = 0;
      fragment.end_char_offset     = text.length();
      fragment.global_start_offset = m_fullText.length(); // Start VOR dem Text
      fragment.is_leaf             = true;

      // Text zum Volltext hinzufügen
      m_fullText += text;
      fragment.global_end_offset = m_fullText.length(); // Ende NACH dem Text
      fragment.element_tag       = getElementTag( el );
      fragment.is_block_element  = isBlockLevelElement( fragment.element_tag );

      m_fragments.push_back( fragment );

      // WICHTIG: Leerzeichen NACH dem Speichern des Fragments hinzufügen
      // So zeigt global_end_offset auf das Ende des Textes, nicht auf das Leerzeichen
      // m_fullText += " ";

      // qDebug() << "Leaf-Fragment:" << QString::fromStdString( fragment.text ).left( 30 ) << "| Pos:" << pos.x << pos.y << "| Size:" << pos.width
      //          << "x" << pos.height;
    }
  }

  // Rekursiv durch alle Kindelemente
  for ( auto it = el->children().begin(); it != el->children().end(); ++it )
  {
    collectTextFragments( *it );
  }
}

litehtml::position TextManager::calculateBoundingBox( int searchStart, int searchEnd, std::vector<TextFragment>& matchedFragments )
{
  litehtml::position boundingBox   = { 0, 0, 0, 0 };
  bool               firstFragment = true;

  qDebug() << "\n=== Berechne BBox ===";
  qDebug() << "Suchbereich im m_fullText:" << searchStart << "-" << searchEnd;
  qDebug() << "Gesuchter Text:"
           << QString::fromStdString( m_fullText.substr( std::min( (size_t)searchStart, m_fullText.length() ),
                                                         std::min( (size_t)( searchEnd - searchStart ), m_fullText.length() - searchStart ) ) );

  for ( const auto& fragment : m_fragments )
  {
    if ( !fragment.is_leaf || !fragment.hasValidPosition() )
    {
      continue;
    }

    // qDebug() << "Prüfe Fragment:" << QString::fromStdString( fragment.text ).left( 20 ) << "| Global:" << fragment.global_start_offset << "-"
    //          << fragment.global_end_offset;

    // Überlappung zwischen Suchbereich und Fragment
    // WICHTIG: Fragment geht von global_start_offset bis global_end_offset
    // Das Leerzeichen danach ist NICHT Teil des Fragments!
    int overlapStart = std::max( searchStart, fragment.global_start_offset );
    int overlapEnd   = std::min( searchEnd, fragment.global_end_offset );

    if ( overlapStart < overlapEnd )
    {
      qDebug() << "  → Überlappung:" << overlapStart << "-" << overlapEnd;

      TextFragment matchedFragment = fragment;

      // Lokale Offsets im Fragment-Text berechnen
      int localStart = overlapStart - fragment.global_start_offset;
      int localEnd   = overlapEnd - fragment.global_start_offset;

      // Bounds checking
      int textLen = static_cast<int>( fragment.text.length() );
      localStart  = std::clamp( localStart, 0, textLen );
      localEnd    = std::clamp( localEnd, 0, textLen );

      if ( localEnd <= localStart )
      {
        qDebug() << "  → FEHLER: Ungültige lokale Offsets!";
        continue;
      }

      matchedFragment.start_char_offset = localStart;
      matchedFragment.end_char_offset   = localEnd;

      qDebug() << "  → Lokale Offsets:" << localStart << "-" << localEnd
               << "| Text:" << QString::fromStdString( fragment.text.substr( localStart, localEnd - localStart ) );

      // Position für diesen Teil des Fragments berechnen
      litehtml::position adjustedPos = fragment.pos;

      if ( textLen > 0 && fragment.pos.width > 0 )
      {
        float charWidth = static_cast<float>( fragment.pos.width ) / static_cast<float>( textLen );

        // X-Position anpassen
        adjustedPos.x = fragment.pos.x + static_cast<int>( charWidth * localStart );

        // Breite anpassen
        adjustedPos.width = static_cast<int>( charWidth * ( localEnd - localStart ) );

        qDebug() << "  → Angepasste Position:" << adjustedPos.x << adjustedPos.y << "| Größe:" << adjustedPos.width << "x" << adjustedPos.height
                 << "| CharWidth:" << charWidth;
      }

      // Speichere angepasste Position im Fragment
      matchedFragment.pos = adjustedPos;
      matchedFragments.push_back( matchedFragment );

      // Bounding Box erweitern/initialisieren
      if ( firstFragment )
      {
        boundingBox   = adjustedPos;
        firstFragment = false;
        qDebug() << "  → Initiale BBox:" << boundingBox.x << boundingBox.y << boundingBox.width << "x" << boundingBox.height;
      }
      else
      {
        int minX = std::min( boundingBox.x, adjustedPos.x );
        int minY = std::min( boundingBox.y, adjustedPos.y );
        int maxX = std::max( boundingBox.x + boundingBox.width, adjustedPos.x + adjustedPos.width );
        int maxY = std::max( boundingBox.y + boundingBox.height, adjustedPos.y + adjustedPos.height );

        boundingBox.x      = minX;
        boundingBox.y      = minY;
        boundingBox.width  = maxX - minX;
        boundingBox.height = maxY - minY;

        qDebug() << "  → Erweiterte BBox:" << boundingBox.x << boundingBox.y << boundingBox.width << "x" << boundingBox.height;
      }
    }
  }

  qDebug() << "Finale BBox:" << boundingBox.x << boundingBox.y << boundingBox.width << "x" << boundingBox.height;
  qDebug() << "Anzahl Fragmente:" << matchedFragments.size();
  qDebug() << "===================\n";
  return boundingBox;
}

// Ermittle HTML-Tag des Elements oder seiner Eltern
std::string TextManager::getElementTag( litehtml::element::ptr element ) const
{
  if ( !element )
    return "";

  // Gehe zum Parent-Element, da Textknoten selbst kein Tag haben
  auto parent = element->parent();
  if ( !parent )
    return "";

  // Hole Tag-Name
  const char* tag = parent->get_tagName();
  if ( tag )
  {
    std::string tagName( tag );
    std::transform( tagName.begin(), tagName.end(), tagName.begin(), ::tolower );
    return tagName;
  }

  return "";
}

// Prüfe ob Element ein Block-Level-Element ist
bool TextManager::isBlockLevelElement( const std::string& tag ) const
{
  // Liste der gängigen Block-Level-Elemente
  static const std::vector<std::string> blockElements = {
    "address", "article",  "aside", "blockquote", "canvas", "dd",      "div",   "dl",    "dt",     "fieldset", "figcaption", "figure",
    "footer",  "form",     "h1",    "h2",         "h3",     "h4",      "h5",    "h6",    "header", "hr",       "li",         "main",
    "nav",     "noscript", "ol",    "p",          "pre",    "section", "table", "tfoot", "ul",     "video",
    "br" // BR ist speziell: inline aber erzeugt Zeilenumbruch
  };

  return std::find( blockElements.begin(), blockElements.end(), tag ) != blockElements.end();
}

bool TextManager::shouldInsertNewline( const TextFragment& prev, const TextFragment& current ) const
{
  // WICHTIG: Prüfe ob die Fragmente das GLEICHE Parent-Element haben
  if ( prev.parent_element == current.parent_element )
  {
    // Gleicher Parent = kein Newline (z.B. mehrere Textknoten in einem <p>)
    return false;
  }

  // Unterschiedliche Parents: Prüfe ob eines davon ein Block-Element ist
  if ( prev.is_block_element || current.is_block_element )
  {
    return true;
  }

  return false;
}

std::string TextManager::selectedText( const SelectionRange& selection, TextFormatMode mode ) const
{
  if ( selection.isEmpty() )
  {
    return {};
  }

  std::string result;

  if ( mode == TextFormatMode::Structured )
  {
    for ( size_t i = 0; i < selection.fragments.size(); ++i )
    {
      const auto& fragment = selection.fragments[i];

      if ( fragment.end_char_offset > fragment.start_char_offset )
      {
        std::string part = fragment.text.substr( fragment.start_char_offset, fragment.end_char_offset - fragment.start_char_offset );

        if ( i > 0 )
        {
          const auto& prevFragment = selection.fragments[i - 1];

          // WICHTIG: Prüfe ob gleicher Parent
          if ( prevFragment.parent_element != fragment.parent_element )
          {
            // Unterschiedliches Parent-Element
            if ( prevFragment.element_tag == "p" && fragment.element_tag == "p" )
            {
              // Zwischen zwei <p>-Elementen: Doppelter Newline
              result += "\n\n";
            }
            else if ( prevFragment.is_block_element || fragment.is_block_element )
            {
              // Anderes Block-Element: Einfacher Newline
              result += "\n";
            }
            else
            {
              if ( prevFragment.element_tag != fragment.element_tag )
              {
                // Inline-Elemente mit unterschiedlichem Parent: Space
                result += ' ';
              }
            }
          }
          else
          {
            // Gleiches Parent-Element: Nur Space            result += ' ';
          }
        }

        result += part;
      }
    }
  }
  else // Plain mode
  {
    bool first = true;
    for ( const auto& fragment : selection.fragments )
    {
      if ( fragment.end_char_offset > fragment.start_char_offset )
      {
        std::string part = fragment.text.substr( fragment.start_char_offset, fragment.end_char_offset - fragment.start_char_offset );

        if ( !first )
        {
          result += ' ';
        }
        result += part;
        first = false;
      }
    }
  }

  // Trimme Whitespace
  auto start = result.find_first_not_of( " \t\n\r" );
  auto end   = result.find_last_not_of( " \t\n\r" );

  if ( start == std::string::npos )
  {
    return "";
  }

  return result.substr( start, end - start + 1 );
}
