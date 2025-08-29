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
#include <inttypes.h>

using namespace cali;

namespace
{

class ArielBinding : public cali::AnnotationBinding
{

    bool enabled = false;
    int depth = 0;

    /* To be used in a possible future mode where we
     * turn off ariel for excluded regions nested in
     * traced regions
     */

    /*
    enum class RegionType {
        INCLUDE,
        EXCLUDE,
        INVALID
    };

    std::stack<RegionType> stack;
    */


public:

    const char* service_tag() const { return "ariel"; };

    void on_begin(Caliper* c, const Attribute& attr, const Variant& value) override
    {
        if (!enabled) {
            ariel_enable();
            enabled=true;
        }
        depth++;
        enabled = true;
        if (attr.type() == CALI_TYPE_STRING) {
            const char* str = static_cast<const char*>(value.data());
            ariel_output_stats_begin_region(str);
        } else {
            printf("ARIEL-CALI-WARNING: Unknown attr.type()\n");
        }
    }

    void on_begin_excluded(Caliper* c, const Attribute& attr, const Variant& value)
    {
        /* To be used in a possible future mode where we
         * turn off ariel for excluded regions nested in
         * traced regions
         */

        /*
        printf("on_begin_excluded\n");
        if (enabled) {
            ariel_disable();
        }
        */
    }

    void on_end(Caliper* c, const Attribute& attr, const Variant& value) override
    {
        depth--;
        if (depth == 0) {
            ariel_disable();
            enabled = false;
        }
        if (attr.type() == CALI_TYPE_STRING) {
            const char* str = static_cast<const char*>(value.data());
            ariel_output_stats_end_region(str);
        } else {
            printf("ARIEL-CALI-WARNING: Unknown attr.type()\n");
        }
    }

    void on_end_excluded(Caliper* c, const Attribute& attr, const Variant& value)
    {
        /* To be used in a possible future mode where we
         * turn off ariel for excluded regions nested in
         * traced regions
         */

        /*
        if (enabled && depth>0) {
            ariel_disable();
        }
        */
    }
};

} // namespace

namespace cali
{

CaliperService ariel_service { "ariel", &AnnotationBinding::make_binding<::ArielBinding> };

}
