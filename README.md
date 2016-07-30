# Compatibility
c++11

# mpv::bmap
<pre>
template<
    class _Kty, 
    class _Ty,
    class _KeyPr = std::less<_Kty>,
    class _ValuePr = std::less<_Ty>,
    class _Alloc = std::allocator<std::pair<std::shared_ptr<_Kty>, std::shared_ptr<_Ty>> >
    >
  class bmap 
</pre>

mpv::bmap is a sorted associative container that contains key-value pairs with unique keys and values. Keys are sorted by using the comparison function _KeyPr, values are sorted by using _ValuePr.
Search, removal, and insertion operations are thread-safety.

[Wiki. mpv::bmap](https://github.com/paulusmas/bmap/wiki)
