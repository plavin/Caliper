// Copyright (c) 2015-2022, Lawrence Livermore National Security, LLC.
// See top-level LICENSE file for details.

// Caliper annotation bindings for the Ariel API, a simulation
// component in SST-Elements.

#include "../Services.h"

#include "../../caliper/AnnotationBinding.h"

#include "caliper/common/Attribute.h"
#include "caliper/common/Variant.h"

#include <arielapi.h>

#include <stack>

using namespace cali;

namespace
{

class ArielBinding : public cali::AnnotationBinding
{

    bool enabled = false;
    int depth = 0;

    enum class RegionType {
        INCLUDE,
        EXCLUDE,
        INVALID
    };

    RegionType last = RegionType::INVALID;

    std::stack<RegionType> stack;
    //void print_timestamp
    //TODO: print out current cycle count in ariel and name of region

public:

    const char* service_tag() const { return "ariel"; };

    void on_begin(Caliper* c, const Attribute& attr, const Variant& value)
    {
        //TODO: Always : ariel_output_stats();
        if (!enabled) {
            ariel_enable();
        }
        depth++;
        enabled = true;
        if (attr.type() == CALI_TYPE_STRING) {
            const char* str = static_cast<const char*>(value.data());
            printf("Beginning region: %s\n", str);
        }
        //stack.push(RegionType::INCLUDE);
    }

    void on_begin_excluded(Caliper* c, const Attribute& attr, const Variant& value)
    {
        printf("on_begin_excluded\n");
        if (enabled) {
            ariel_disable();
        }
    }

    void on_end(Caliper* c, const Attribute& attr, const Variant& value)
    {
        depth--;
        if (depth == 0) {
            ariel_disable();
            enabled = false;
        }
        printf("end\n");
        //arielapi_output_stats();
        //enabled = false;
    }

    void on_end_excluded(Caliper* c, const Attribute& attr, const Variant& value)
    {
        if (enabled && depth>0) {
            ariel_disable();
        }
    }
};

} // namespace

namespace cali
{

CaliperService ariel_service { "ariel", &AnnotationBinding::make_binding<::ArielBinding> };

}
