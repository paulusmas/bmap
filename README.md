# Compatibility
c++11

# mpv::bmap
```c++
template<
    class _Kty, 
    class _Ty,
    class _KeyPr = std::less<_Kty>,
    class _ValuePr = std::less<_Ty>,
    class _Alloc = std::allocator<std::pair<std::shared_ptr<_Kty>, std::shared_ptr<_Ty>> >
    >
  class bmap 
```

**mpv::bmap** is a sorted associative container that contains key-value pairs with unique keys and values. Keys are sorted by using the comparison function _**_KeyPr**_, values are sorted by using _**_ValuePr**_.
Search, removal, and insertion operations are thread-safety.

### Iterators
[begin](https://github.com/paulusmas/bmap/wiki/mpv::map::begin#mpvmapbegin) -- _returns an iterator to the beginning_

[end](https://github.com/paulusmas/bmap/wiki/mpv::end) -- _returns an iterator to the end_

### Capacity
[empty](https://github.com/paulusmas/bmap/wiki/mpv::bmap::empty) -- _checks whether the container is empty_

[size](https://github.com/paulusmas/bmap/wiki/mpv::bmap::size) -- _returns the number of elements_

### Modifiers
[clear](https://github.com/paulusmas/bmap/wiki/mpv::bmap::clear) -- _clears the contents_

[insert](https://github.com/paulusmas/bmap/wiki/mpv::bmap::insert) -- _inserts elements_

[erase](https://github.com/paulusmas/bmap/wiki/mpv::bmap::erase) -- _erases elements_

### Lookup
[find](https://github.com/paulusmas/bmap/wiki/mpv::bmap::find) -- _finds element with specific key/value_

***
# [Wiki. mpv::bmap](https://github.com/paulusmas/bmap/wiki)
