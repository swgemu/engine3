/******************************************************************************/
/*  Copyright (c) 2010-2011, Tim Day <timday@timday.com>                      */
/*                                                                            */
/*  Permission to use, copy, modify, and/or distribute this software for any  */
/*  purpose with or without fee is hereby granted, provided that the above    */
/*  copyright notice and this permission notice appear in all copies.         */
/*                                                                            */
/*  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES  */
/*  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF          */
/*  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR   */
/*  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    */
/*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN     */
/*  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF   */
/*  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.            */
/******************************************************************************/

#ifndef _lru_cache_h_
#define _lru_cache_h_

// Bring all the necessary includes together,
// and define some type helpers.

#include <map>

#ifdef CXX11_COMPILER
#include <unordered_map>
#include "lru_cache_using_std.h"
#else
#include "lru_cache_using_std_c98.h"
#endif

// See http://www.gotw.ca/gotw/079.htm for why we can't
// just use a templated typedef.

#ifdef CXX11_COMPILER
template <typename K,typename V> struct lru_cache_using_std_map {
  typedef lru_cache_using_std<K,V,std::map> type;
};

template <typename K,typename V> struct lru_cache_using_std_unordered_map {
  typedef lru_cache_using_std<K,V,std::unordered_map> type;
};

#else
template <typename K,typename V> struct lru_cache_using_std_map {
  typedef lru_cache_using_std<K,V,std::map> type;
};

#endif

#endif
