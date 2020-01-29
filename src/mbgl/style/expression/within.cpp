#include <mapbox/geojson.hpp>
#include <mapbox/geometry.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/expression/within.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/string.hpp>

namespace {

double isLeft(mbgl::Point<double> P0, mbgl::Point<double> P1, mbgl::Point<double> P2) {
    return ((P1.x - P0.x) * (P2.y - P0.y) - (P2.x - P0.x) * (P1.y - P0.y));
}

bool pointWithinPoly(mbgl::Point<double> point, const mapbox::geometry::polygon<double>& polys) {
    int wn = 0;
    for (auto poly : polys) {
        auto size = poly.size();
        auto i = size;
        for (i = 0; i < size - 1; ++i) {
            if (poly[i].y <= point.y) {
                if (poly[i + 1].y > point.y) {
                    if (isLeft(poly[i], poly[i + 1], point) > 0) {
                        ++wn;
                    }
                }
            } else {
                if (poly[i + 1].y <= point.y) {
                    if (isLeft(poly[i], poly[i + 1], point) < 0) {
                        --wn;
                    }
                }
            }
        }
        if (wn != 0) {
            return true;
        }
    }
    return wn != 0;
}

bool pointsWithinPoly(const mbgl::GeometryTileFeature& feature,
                      const mbgl::CanonicalTileID& canonical,
                      const mbgl::GeoJSON& geoJson) {
    const auto& geometrySet = geoJson.get<mapbox::geometry::geometry<double>>();
    // At first check type
    const auto& polygon = geometrySet.get<mapbox::geometry::polygon<double>>();

    const auto point = convertGeometry(feature, canonical).get<mapbox::geometry::point<double>>();

    bool isInside = pointWithinPoly(point, polygon);
    return isInside;
}
}

namespace mbgl {
namespace style {
namespace expression {

using namespace mbgl::style::conversion;

optional<GeoJSON> parseValue(const Convertible& value_) {
    if (isObject(value_)) {
        Error error;
        auto geojson = toGeoJSON(value_, error);
        if (geojson && error.message.empty()) {
            
        }
        return geojson;
    }
    return optional<GeoJSON>();
}

EvaluationResult Within::evaluate(const EvaluationContext& params) const {
    if (!params.feature || !params.canonical) {
        return false;
    }

    auto geometryType = params.feature->getType();
    if (geometryType == FeatureType::Polygon || geometryType == FeatureType::Unknown) {
        return false;
    }
    if (geometryType == FeatureType::Point) {
        return pointsWithinPoly(*params.feature, *params.canonical, geoJSONSource);
    } else if (geometryType == FeatureType::LineString) {
        return false;
    }
    return false;
}

ParseResult Within::parse(const Convertible& value, ParsingContext& ctx) {
    if (isArray(value)) {
        // object or array value, quoted with ["Within", value]
        if (arrayLength(value) != 2) {
            ctx.error("'Within' expression requires exactly one argument, but found " +
                      util::toString(arrayLength(value) - 1) + " instead.");
            return ParseResult();
        }

        auto parsedValue = parseValue(arrayMember(value, 1));
        if (!parsedValue) {
            return ParseResult();
        }
        return ParseResult(std::make_unique<Within>(*parsedValue));
    }
    return ParseResult();
}

mbgl::Value Within::serialize() const {
    auto ret = std::vector<mbgl::Value>{{getOperator()}};

    ret.push_back(mbgl::Value(mapbox::geojson::stringify(geoJSONSource)));
    return ret;
}

} // namespace expression
} // namespace style
} // namespace mbgl
