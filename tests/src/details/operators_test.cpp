#include <sstream>
#include <string>

#include <doctest.h>

#include "ureact/ureact.hpp"

namespace
{

enum class binary_operator_type
{
    signal_op_signal,
    temp_signal_op_signal,
    signal_op_temp_signal,
    temp_signal_op_temp_signal,
    value_op_signal,
    value_op_temp_signal,
    signal_op_value,
    temp_signal_op_value
};

#define MAKE_BINARY_OPERATOR_SIGNAL( OPERATOR, NAME )                                              \
    template <typename left_t,                                                                     \
        typename right_t,                                                                          \
        typename ret_t = decltype( std::declval<left_t>() OPERATOR std::declval<right_t>() )>      \
    auto make_binary_operator_signal_##NAME(                                                       \
        ureact::context& ctx, binary_operator_type type, const left_t& lhs, const right_t& rhs )   \
        ->ureact::signal<ret_t>                                                                    \
    {                                                                                              \
        switch( type )                                                                             \
        {                                                                                          \
            case binary_operator_type::signal_op_signal:                                           \
                return make_var( ctx, lhs ) OPERATOR make_var( ctx, rhs );                         \
            case binary_operator_type::temp_signal_op_signal:                                      \
                return (+make_var( ctx, lhs ))OPERATOR make_var( ctx, rhs );                       \
            case binary_operator_type::signal_op_temp_signal:                                      \
                return make_var( ctx, lhs ) OPERATOR( +make_var( ctx, rhs ) );                     \
            case binary_operator_type::temp_signal_op_temp_signal:                                 \
                return (+make_var( ctx, lhs ))OPERATOR( +make_var( ctx, rhs ) );                   \
            case binary_operator_type::value_op_signal: return lhs OPERATOR make_var( ctx, rhs );  \
            case binary_operator_type::value_op_temp_signal:                                       \
                return lhs OPERATOR( +make_var( ctx, rhs ) );                                      \
            case binary_operator_type::signal_op_value: return make_var( ctx, lhs ) OPERATOR rhs;  \
            case binary_operator_type::temp_signal_op_value:                                       \
                return (+make_var( ctx, lhs ))OPERATOR rhs;                                        \
        }                                                                                          \
        return {};                                                                                 \
    }

#define BINARY_OPERATOR( OPERATOR, NAME )                                                          \
    template <typename left_t,                                                                     \
        typename right_t,                                                                          \
        typename ret_t = decltype( std::declval<left_t>() OPERATOR std::declval<right_t>() )>      \
    auto binary_operator_##NAME( const left_t& lhs, const right_t& rhs )->ret_t                    \
    {                                                                                              \
        return lhs OPERATOR rhs;                                                                   \
    }

MAKE_BINARY_OPERATOR_SIGNAL( +, addition )
BINARY_OPERATOR( +, addition )

#define DECLARE_BINARY_OPERATOR_TRAITS( OPERATOR, NAME )                                           \
    struct binary_operator_traits_##NAME                                                           \
    {                                                                                              \
        static const char* get_name()                                                              \
        {                                                                                          \
            return #NAME;                                                                          \
        }                                                                                          \
        static const char* get_operator()                                                          \
        {                                                                                          \
            return #OPERATOR;                                                                      \
        }                                                                                          \
                                                                                                   \
        template <typename left_t, typename right_t>                                               \
        static auto make_signal( ureact::context& ctx,                                             \
            const binary_operator_type type,                                                       \
            const left_t& lhs,                                                                     \
            const right_t& rhs )                                                                   \
            -> ureact::signal<typename decltype( std::declval<ureact::signal<left_t>>()            \
                    OPERATOR std::declval<ureact::signal<right_t>>() )::value_t>                   \
        {                                                                                          \
            switch( type )                                                                         \
            {                                                                                      \
                case binary_operator_type::signal_op_signal:                                       \
                    return make_var( ctx, lhs ) OPERATOR make_var( ctx, rhs );                     \
                case binary_operator_type::temp_signal_op_signal:                                  \
                    return (+make_var( ctx, lhs ))OPERATOR make_var( ctx, rhs );                   \
                case binary_operator_type::signal_op_temp_signal:                                  \
                    return make_var( ctx, lhs ) OPERATOR( +make_var( ctx, rhs ) );                 \
                case binary_operator_type::temp_signal_op_temp_signal:                             \
                    return (+make_var( ctx, lhs ))OPERATOR( +make_var( ctx, rhs ) );               \
                case binary_operator_type::value_op_signal:                                        \
                    return lhs OPERATOR make_var( ctx, rhs );                                      \
                case binary_operator_type::value_op_temp_signal:                                   \
                    return lhs OPERATOR( +make_var( ctx, rhs ) );                                  \
                case binary_operator_type::signal_op_value:                                        \
                    return make_var( ctx, lhs ) OPERATOR rhs;                                      \
                case binary_operator_type::temp_signal_op_value:                                   \
                    return (+make_var( ctx, lhs ))OPERATOR rhs;                                    \
            }                                                                                      \
            return {};                                                                             \
        }                                                                                          \
                                                                                                   \
        template <typename left_t, typename right_t>                                               \
        static auto execute_operator( const left_t& lhs, const right_t& rhs )                      \
            -> decltype( std::declval<left_t>() OPERATOR std::declval<right_t>() )                 \
        {                                                                                          \
            return lhs OPERATOR rhs;                                                               \
        }                                                                                          \
                                                                                                   \
        template <typename left_t, typename right_t>                                               \
        static std::string get_test_name(                                                          \
            const binary_operator_type type, const left_t& lhs, const right_t& rhs )               \
        {                                                                                          \
            std::ostringstream ss;                                                                 \
            switch( type )                                                                         \
            {                                                                                      \
                case binary_operator_type::signal_op_signal:                                       \
                    ss << "signal(" << lhs << ") " #OPERATOR " signal(" << rhs << ")";             \
                    break;                                                                         \
                case binary_operator_type::temp_signal_op_signal:                                  \
                    ss << "temp_signal(" << lhs << ") " #OPERATOR " signal(" << rhs << ")";        \
                    break;                                                                         \
                case binary_operator_type::signal_op_temp_signal:                                  \
                    ss << "signal(" << lhs << ") " #OPERATOR " temp_signal(" << rhs << ")";        \
                    break;                                                                         \
                case binary_operator_type::temp_signal_op_temp_signal:                             \
                    ss << "temp_signal(" << lhs << ") " #OPERATOR " temp_signal(" << rhs << ")";   \
                    break;                                                                         \
                case binary_operator_type::value_op_signal:                                        \
                    ss << "value(" << lhs << ") " #OPERATOR " signal(" << rhs << ")";              \
                    break;                                                                         \
                case binary_operator_type::value_op_temp_signal:                                   \
                    ss << "value(" << lhs << ") " #OPERATOR " temp_signal(" << rhs << ")";         \
                    break;                                                                         \
                case binary_operator_type::signal_op_value:                                        \
                    ss << "signal(" << lhs << ") " #OPERATOR " value(" << rhs << ")";              \
                    break;                                                                         \
                case binary_operator_type::temp_signal_op_value:                                   \
                    ss << "temp_signal(" << lhs << ") " #OPERATOR " value(" << rhs << ")";         \
                    break;                                                                         \
            }                                                                                      \
            return ss.str();                                                                       \
        }                                                                                          \
    };

#if defined( __clang__ ) && defined( __clang_minor__ )
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunknown-warning-option"
#    pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#endif

DECLARE_BINARY_OPERATOR_TRAITS( +, addition )
DECLARE_BINARY_OPERATOR_TRAITS( -, subtraction )
DECLARE_BINARY_OPERATOR_TRAITS( *, multiplication )
DECLARE_BINARY_OPERATOR_TRAITS( /, division )
DECLARE_BINARY_OPERATOR_TRAITS( %, modulo )

DECLARE_BINARY_OPERATOR_TRAITS( ==, equal )
DECLARE_BINARY_OPERATOR_TRAITS( !=, not_equal )
DECLARE_BINARY_OPERATOR_TRAITS( <, less )
DECLARE_BINARY_OPERATOR_TRAITS( <=, less_equal )
DECLARE_BINARY_OPERATOR_TRAITS( >, greater )
DECLARE_BINARY_OPERATOR_TRAITS( >=, greater_equal )

DECLARE_BINARY_OPERATOR_TRAITS( &&, logical_and )
DECLARE_BINARY_OPERATOR_TRAITS( ||, logical_or )

DECLARE_BINARY_OPERATOR_TRAITS( &, bitwise_and )
DECLARE_BINARY_OPERATOR_TRAITS( |, bitwise_or )
DECLARE_BINARY_OPERATOR_TRAITS( ^, bitwise_xor )
DECLARE_BINARY_OPERATOR_TRAITS( <<, bitwise_left_shift )
DECLARE_BINARY_OPERATOR_TRAITS( >>, bitwise_right_shift )

#if defined( __clang__ ) && defined( __clang_minor__ )
#    pragma clang diagnostic pop
#endif

template <typename traits, typename left_t, typename right_t>
void test_binary_operator_impl( binary_operator_type type, const left_t& lhs, const right_t& rhs )
{
    ureact::context ctx;
    auto signal_to_test = traits::make_signal( ctx, type, lhs, rhs );
    auto value_from_signal = signal_to_test.value();
    auto value_from_operator = traits::execute_operator( lhs, rhs );
    static_assert(
        std::is_same<decltype( value_from_signal ), decltype( value_from_operator )>::value,
        "Value type from signal should match return type of operator" );
    CHECK( value_from_signal == value_from_operator );
}

template <typename traits, typename left_t, typename right_t>
void test_binary_operator( const left_t& lhs, const right_t& rhs )
{
    for( auto type : { binary_operator_type::signal_op_signal,
             binary_operator_type::temp_signal_op_signal,
             binary_operator_type::signal_op_temp_signal,
             binary_operator_type::temp_signal_op_temp_signal,
             binary_operator_type::value_op_signal,
             binary_operator_type::value_op_temp_signal,
             binary_operator_type::signal_op_value,
             binary_operator_type::temp_signal_op_value } )
    {
        SUBCASE( traits::get_test_name( type, lhs, rhs ).c_str() )
        {
            test_binary_operator_impl<traits>( type, lhs, rhs );
        }
    }
}

} // namespace

TEST_SUITE( "operators" )
{
    TEST_CASE( "unary operators" )
    {
        ureact::context ctx;

        auto v1 = make_var( ctx, 1 );

        // clang-format off
        auto unary_plus           = +v1;
        auto unary_minus          = -v1;
        auto logical_negation     = !v1;
        auto bitwise_complement   = ~v1;
        auto unary_plus_2         = +(+v1);
        auto unary_minus_2        = -(+v1);
        auto logical_negation_2   = !(+v1);
        auto bitwise_complement_2 = ~(+v1);
        // clang-format on

        auto checkValues = [&]( std::initializer_list<int> valuesToTest ) {
            for( const int& value : valuesToTest )
            {
                v1 <<= value;

                // clang-format off
                CHECK( unary_plus.value()           == (+value) );
                CHECK( unary_minus.value()          == (-value) );
                CHECK( logical_negation.value()     == (!value) );
                CHECK( bitwise_complement.value()   == (~value) );
                CHECK( unary_plus_2.value()         == (+value) );
                CHECK( unary_minus_2.value()        == (-value) );
                CHECK( logical_negation_2.value()   == (!value) );
                CHECK( bitwise_complement_2.value() == (~value) );
                // clang-format on
            }
        };

        checkValues( { 0, 1, -4, 654 } );
    }

    TEST_CASE( "binary operators (normal)" )
    {
        ureact::context ctx;

        auto lhs = make_var( ctx, 0 );
        auto rhs = make_var( ctx, 1 );

        // clang-format off
        auto addition            = lhs +  rhs;
        auto subtraction         = lhs -  rhs;
        auto multiplication      = lhs *  rhs;
        auto bitwise_and         = lhs &  rhs;
        auto bitwise_or          = lhs |  rhs;
        auto bitwise_xor         = lhs ^  rhs;
        auto bitwise_left_shift  = lhs << rhs;
        auto bitwise_right_shift = lhs >> rhs;
        auto equal               = lhs == rhs;
        auto not_equal           = lhs != rhs;
        auto less                = lhs <  rhs;
        auto less_equal          = lhs <= rhs;
        auto greater             = lhs >  rhs;
        auto greater_equal       = lhs >= rhs;
        auto logical_and         = lhs && rhs;
        auto logical_or          = lhs || rhs;
        // clang-format on

        std::initializer_list<std::pair<int, int>> values_to_test = {
            { 2, 2 },
            { 3, -3 },
            { 0, 0 },
            { -4, 7 },
            { -8, -2 },
            { 0, -2 },
            { 5, 0 },
        };

        for( const auto& values : values_to_test )
        {
            int left, right;
            std::tie( left, right ) = values;

            SUBCASE( ( std::to_string( left ) + std::string( " op " ) + std::to_string( right ) )
                         .c_str() )
            {
                ctx.do_transaction( [&]() {
                    lhs <<= left;
                    rhs <<= right;
                } );

                // clang-format off
                CHECK( addition.value()            == (left +  right) );
                CHECK( subtraction.value()         == (left -  right) );
                CHECK( multiplication.value()      == (left *  right) );
                CHECK( equal.value()               == (left == right) );
                CHECK( not_equal.value()           == (left != right) );
                CHECK( less.value()                == (left <  right) );
                CHECK( less_equal.value()          == (left <= right) );
                CHECK( greater.value()             == (left >  right) );
                CHECK( greater_equal.value()       == (left >= right) );
                CHECK( logical_and.value()         == (left && right) );
                CHECK( logical_or.value()          == (left || right) );
                CHECK( bitwise_and.value()         == (left &  right) );
                CHECK( bitwise_or.value()          == (left |  right) );
                CHECK( bitwise_xor.value()         == (left ^  right) );
                CHECK( bitwise_left_shift.value()  == (left << right) );
                CHECK( bitwise_right_shift.value() == (left >> right) );
                // clang-format on
            }
        }
    }

    TEST_CASE( "binary operators (divisible)" )
    {
        ureact::context ctx;

        auto lhs = make_var( ctx, 0 );
        auto rhs = make_var( ctx, 1 );

        // clang-format off
        auto division = lhs / rhs;
        auto modulo   = lhs % rhs;
        // clang-format on

        std::initializer_list<std::pair<int, int>> values_to_test = {
            { 2, 2 },
            { 3, -3 },
            { 8, 3 },
        };

        for( const auto& values : values_to_test )
        {
            int left, right;
            std::tie( left, right ) = values;
            SUBCASE( ( std::to_string( left ) + std::string( " op " ) + std::to_string( right ) )
                         .c_str() )
            {
                ctx.do_transaction( [&]() {
                    lhs <<= left;
                    rhs <<= right;
                } );
                // clang-format off
                CHECK( division.value() == (left / right) );
                CHECK( modulo.value()   == (left % right) );
                // clang-format on
            }
        }
    }

    TEST_CASE( "binary operators (full test)" )
    {
        test_binary_operator<binary_operator_traits_addition>( 2, 2 );
        test_binary_operator<binary_operator_traits_addition>( 3, -3 );
        test_binary_operator<binary_operator_traits_addition>( 1, 0.1f );
        test_binary_operator<binary_operator_traits_addition>( true, 0 );

        test_binary_operator<binary_operator_traits_division>( 2, 2 );
        test_binary_operator<binary_operator_traits_division>( 3, -3 );
        test_binary_operator<binary_operator_traits_division>( 8, 3 );
        test_binary_operator<binary_operator_traits_division>( 0, 1 );
        test_binary_operator<binary_operator_traits_division>( 10, 3.0 );

        test_binary_operator<binary_operator_traits_modulo>( 2, 2 );
        test_binary_operator<binary_operator_traits_modulo>( 3, -3 );
        test_binary_operator<binary_operator_traits_modulo>( 8, 3 );
        test_binary_operator<binary_operator_traits_modulo>( 0, 1 );
    }

    TEST_CASE( "priority" )
    {
        // https://en.cppreference.com/w/cpp/language/operator_precedence
        // 3.  +a -a          Unary plus and minus
        //     ! ~            Logical NOT and bitwise NOT
        // 5.  a*b  a/b  a%b  Multiplication, division, and remainder
        // 6.  a+b  a-b       Addition and subtraction
        // 7.  <<  >>         Bitwise left shift and right shift
        // 8.  <=>            Three-way comparison operator (since C++20)
        // 9.  <  <=          For relational operators < and ≤ respectively
        //     >  >=          For relational operators > and ≥ respectively
        // 10. ==  !=         For equality operators = and ≠ respectively
        // 11. &              Bitwise AND
        // 12. ^              Bitwise XOR (exclusive or)
        // 13. |              Bitwise OR (inclusive or)
        // 14. &&             Logical AND
        // 15. ||             Logical OR

        ureact::context ctx;

        auto _2 = make_var( ctx, 2 );

        auto result = _2 + _2 * _2;
        CHECK( result.value() == 6 );

        auto result2 = ( _2 + _2 ) * _2;
        CHECK( result2.value() == 8 );
    }

} // TEST_SUITE_END
