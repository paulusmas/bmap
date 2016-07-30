#pragma once
#include <type_traits>

namespace mpv{
  namespace bmap_types{    
    template<typename _Kty, typename _Ty>
    struct is_equal_types{
      enum : bool {
        value = std::is_same<_Kty, _Ty>::value
      };
    };
    struct select_key_type{
      template<typename _Kty, typename _Ty>
      struct result_type{
        typedef _Kty type;
      };
    };
    struct select_value_type{
      template<typename _Kty, typename _Ty>
      struct result_type{
        typedef _Ty type;
      };
    };
    template<bool flag>
    struct select_switch{
      typedef select_key_type result_type;
    };
    template<>
    struct select_switch<false>{    
      typedef select_value_type result_type;
    };
    template<bool flag, typename _Kty, typename _Ty>
    struct select_type{
      typedef typename select_switch<flag>::result_type _switch_type;
      typedef typename _switch_type::template result_type<_Kty, _Ty>::type result_type;
    };
  } 
}

