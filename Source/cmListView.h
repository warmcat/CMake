/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <iterator>
#include <string>

#include <cm/string_view>

/**
 * A lightweight view of a CMake list string.
 *
 * This class provides an iterator for iterating over the elements of a
 * semicolon-separated CMake list string. It handles escaped semicolons and
 * bracket arguments correctly. It avoids allocating a std::vector of strings,
 * but may allocate temporary strings for elements that require un-escaping.
 */
class cmListView
{
public:
  cmListView(cm::string_view str);

  class const_iterator
  {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = cm::string_view;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type*;
    using reference = const value_type&;

    reference operator*() const { return this->CurrentElement; }
    pointer operator->() const { return &this->CurrentElement; }

    const_iterator& operator++();
    const_iterator operator++(int);

    bool operator==(const const_iterator& other) const;
    bool operator!=(const const_iterator& other) const;

  private:
    friend class cmListView;
    const_iterator() = default;
    const_iterator(cm::string_view str);

    void FindNext();

    cm::string_view String;
    cm::string_view::const_iterator Pos = nullptr;
    value_type CurrentElement;
    std::string UnescapedElement;
  };

  const_iterator begin() const;
  const_iterator end() const;

private:
  cm::string_view String;
};
