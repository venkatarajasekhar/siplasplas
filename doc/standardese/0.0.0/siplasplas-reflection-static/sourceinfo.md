---
layout: standardese-doc
---

# Header file `sourceinfo.hpp`

``` cpp
#define SIPLASPLAS_REFLECTION_STATIC_SOURCEINFO_HPP 

#include <siplasplas/utility/meta.hpp>

#include <array>

#include <string>

namespace cpp
{
    namespace static_reflection
    {
        enum class Kind;
        
        namespace meta
        {
            template <static_reflection::Kind Kind, typename FullName, typename Spelling, typename DisplayName, typename File, std::size_t Line>
            class SourceInfo;
        }
    }
}
```