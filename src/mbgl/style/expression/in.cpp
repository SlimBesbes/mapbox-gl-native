#include <mbgl/style/expression/in.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/style/expression/type.hpp>


namespace mbgl {
namespace style {
namespace expression {

EvaluationResult In::evaluate(const EvaluationContext& params) const {
    const EvaluationResult evaluatedNeedle = needle->evaluate(params);
    const EvaluationResult evaluatedHeystack = haystack->evaluate(params);
    if (!evaluatedNeedle) {
        return evaluatedNeedle.error();
    }
    if (!evaluatedHeystack) {
        return evaluatedHeystack.error();
    }
    
    type::Type evaluatedNeedleType = typeOf(*evaluatedNeedle);
    if (!isComparableRuntimeValue(evaluatedNeedleType)) {
        return EvaluationError{"Expected first argument to be of type boolean, string or number, but found " + toString(evaluatedNeedleType) + " instead."};
    }

    type::Type evaluatedHeystackType = typeOf(*evaluatedHeystack);
    if (!isSearchableRuntimeValue(evaluatedHeystackType)) {
        return EvaluationError{"Expected second argument to be of type array or string, but found " + toString(evaluatedHeystackType) + " instead."};
    }
    
    if(typeOf(*evaluatedHeystack) == type::String) {
        auto needleString = evaluatedNeedle->get<std::string>();
        auto heystackString = evaluatedHeystack->get<std::string>();
        return EvaluationResult(heystackString.find(needleString) != std::string::npos);
    } else {
        // todo: implement arry type.
        return EvaluationResult(false);
    }

}

void In::eachChild(const std::function<void(const Expression&)>& visit) const {
    visit(*needle);
    visit(*haystack);
}

using namespace mbgl::style::conversion;
ParseResult In::parse(const Convertible& value, ParsingContext& ctx) {
    assert(isArray(value));

    std::size_t length = arrayLength(value);
    if (length != 3) {
        ctx.error("Expected 2 arguments, but found " + util::toString(length - 1) + " instead.");
        return ParseResult();
    }

    type::Type needleType = ctx.getExpected() ? *ctx.getExpected() : type::Value;
    type::Type haystackType = type::Array(ctx.getExpected() ? *ctx.getExpected() : type::Value);

    ParseResult needle = ctx.parse(arrayMember(value, 1), 1, {needleType});
    
    ParseResult haystack = ctx.parse(arrayMember(value, 2), 2, {haystackType});

    if (!needle || !haystack) return ParseResult();

    if (!isComparableType((*needle)->getType())){
        ctx.error("Expected first argument to be of type boolean, string or number, but found" + toString((*needle)->getType()) + "instead.");
        return ParseResult();
    }

    return ParseResult(std::make_unique<In>(std::move(*needle), std::move(*haystack)));

}

bool In::isComparableType(type::Type type){
    return type == type::Boolean ||
        type == type::String ||
        type == type::Number ||
        type == type::Null ||
        type == type::Value;
}

bool In::isComparableRuntimeValue(type::Type type){
    return type == type::Boolean ||
        type == type::String ||
        type == type::Number;
}

bool In::isSearchableRuntimeValue(type::Type type){
    return type == type::String ||
        type.is<type::Array>();
}

} // namespace expression
} // namespace style
} // namespace mbgl
