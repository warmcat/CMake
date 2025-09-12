/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#include "cmListView.h"

#include <utility>

cmListView::cmListView(cm::string_view str)
  : String(str)
{
}

cmListView::const_iterator::const_iterator(cm::string_view str)
  : String(str)
  , Pos(String.begin())
{
  this->FindNext();
}

cmListView::const_iterator& cmListView::const_iterator::operator++()
{
  this->FindNext();
  return *this;
}

cmListView::const_iterator cmListView::const_iterator::operator++(int)
{
  const_iterator tmp = *this;
  this->FindNext();
  return tmp;
}

bool cmListView::const_iterator::operator==(const const_iterator& other) const
{
  return this->Pos == other.Pos && this->String.data() == other.String.data();
}

bool cmListView::const_iterator::operator!=(const const_iterator& other) const
{
  return !(*this == other);
}

void cmListView::const_iterator::FindNext()
{
  if (this->Pos > this->String.end()) {
    return;
  }
  if (this->Pos == this->String.end()) {
    this->Pos = this->String.end() + 1; // sentinel
    this->CurrentElement = cm::string_view();
    return;
  }

  auto start = this->Pos;
  int squareNesting = 0;
  bool needsUnescape = false;
  auto end_of_element = this->String.end();

  for (auto it = this->Pos; it != this->String.end(); ++it) {
    switch (*it) {
      case '\\': {
        auto cnext = it + 1;
        if (cnext != this->String.end() && *cnext == ';') {
          needsUnescape = true;
          ++it;
        }
      } break;
      case '[': {
        ++squareNesting;
      } break;
      case ']': {
        --squareNesting;
      } break;
      case ';': {
        if (squareNesting == 0) {
          end_of_element = it;
          goto loop_end;
        }
      } break;
      default:
        break;
    }
  }
loop_end:;

  cm::string_view element_view(start, end_of_element - start);

  if (needsUnescape) {
    this->UnescapedElement.clear();
    this->UnescapedElement.reserve(element_view.length());
    for (auto it = element_view.begin(); it != element_view.end(); ++it) {
      if (*it == '\\' && (it + 1) != element_view.end() && *(it + 1) == ';') {
        this->UnescapedElement += ';';
        ++it;
      } else {
        this->UnescapedElement += *it;
      }
    }
    this->CurrentElement = this->UnescapedElement;
  } else {
    this->CurrentElement = element_view;
  }

  if (end_of_element == this->String.end()) {
    this->Pos = this->String.end();
  } else {
    this->Pos = end_of_element + 1;
  }
}

cmListView::const_iterator cmListView::begin() const
{
  if (this->String.empty()) {
    return this->end();
  }
  return const_iterator(this->String);
}

cmListView::const_iterator cmListView::end() const
{
  const_iterator it;
  it.String = this->String;
  it.Pos = this->String.end() + 1; // sentinel
  return it;
}
