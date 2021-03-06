// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the OpenColorIO Project.


#include <cmath>

#include <OpenColorIO/OpenColorIO.h>

#include "ops/log/LogOp.h"
#include "ops/matrix/MatrixOp.h"
#include "transforms/builtins/ACES.h"
#include "transforms/builtins/BuiltinTransformRegistry.h"
#include "transforms/builtins/OpHelpers.h"
#include "transforms/builtins/SonyCameras.h"


namespace OCIO_NAMESPACE
{

namespace SONY_SGAMUT3
{
static const Chromaticities red_xy(0.730,  0.280);
static const Chromaticities grn_xy(0.140,  0.855);
static const Chromaticities blu_xy(0.100, -0.050);
static const Chromaticities wht_xy(0.3127, 0.3290);

const Primaries primaries(red_xy, grn_xy, blu_xy, wht_xy);
}

namespace SONY_SGAMUT3_CINE
{
static const Chromaticities red_xy(0.766,  0.275);
static const Chromaticities grn_xy(0.225,  0.800);
static const Chromaticities blu_xy(0.089, -0.087);
static const Chromaticities wht_xy(0.3127, 0.3290);

const Primaries primaries(red_xy, grn_xy, blu_xy, wht_xy);
}


namespace SONY_SLOG3_to_LINEAR
{
static constexpr double linSideSlope  = 1. / (0.18 + 0.01);
static constexpr double linSideOffset = 0.01 / (0.18 + 0.01);
static constexpr double logSideSlope  = 261.5 / 1023.;
static constexpr double logSideOffset = 420. / 1023.;
static constexpr double linSideBreak  = 0.01125000;
static constexpr double linearSlope   = ( (171.2102946929 - 95.) / 0.01125000) / 1023.;
static constexpr double base          = 10.;

static const LogOpData::Params 
    params { logSideSlope, logSideOffset, linSideSlope, linSideOffset, linSideBreak, linearSlope };

static const LogOpData log(base, params, params, params, TRANSFORM_DIR_INVERSE);
}


namespace CAMERA
{

namespace SONY
{

void RegisterAll(BuiltinTransformRegistryImpl & registry) noexcept
{
    {
        auto SONY_SLOG3_SGAMUT3_to_ACES2065_1_Functor = [](OpRcPtrVec & ops)
        {
            LogOpDataRcPtr log = SONY_SLOG3_to_LINEAR::log.clone();
            CreateLogOp(ops, log, TRANSFORM_DIR_FORWARD);

            MatrixOpData::MatrixArrayPtr matrix
                = build_conversion_matrix(SONY_SGAMUT3::primaries, ACES_AP0::primaries, ADAPTATION_CAT02);
            CreateMatrixOp(ops, matrix, TRANSFORM_DIR_FORWARD);
        };

        registry.addBuiltin("SONY_SLOG3-SGAMUT3_to_ACES2065-1",
                            "Convert Sony S-Log3 S-Gamut3 to ACES2065-1",
                            SONY_SLOG3_SGAMUT3_to_ACES2065_1_Functor);
    }

    {
        auto SONY_SLOG3_SGAMUT3_CINE_to_ACES2065_1_Functor = [](OpRcPtrVec & ops)
        {
            LogOpDataRcPtr log = SONY_SLOG3_to_LINEAR::log.clone();
            CreateLogOp(ops, log, TRANSFORM_DIR_FORWARD);

            MatrixOpData::MatrixArrayPtr matrix
                = build_conversion_matrix(SONY_SGAMUT3_CINE::primaries, ACES_AP0::primaries, ADAPTATION_CAT02);
            CreateMatrixOp(ops, matrix, TRANSFORM_DIR_FORWARD);
        };

        registry.addBuiltin("SONY_SLOG3-SGAMUT3.CINE_to_ACES2065-1", 
                            "Convert Sony S-Log3 S-Gamut3.Cine to ACES2065-1",
                            SONY_SLOG3_SGAMUT3_CINE_to_ACES2065_1_Functor);
    }
}

} // namespace SONY

} // namespace CAMERA

} // namespace OCIO_NAMESPACE
