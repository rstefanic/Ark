#include <Ark/VM/Value.hpp>

#include <Ark/VM/Frame.hpp>
#include <Ark/Utils.hpp>

#define init_const_type(is_const, type) ((is_const ? (1 << 8) : 0) | static_cast<uint8_t>(type))

namespace Ark::internal
{
    Value::Value() :
        m_constType(init_const_type(false, ValueType::Undefined))
    {}

    // --------------------------

    Value::Value(ValueType type) :
        m_constType(init_const_type(false, type))
    {
        if (type == ValueType::List)
            m_value = std::vector<Value>();
    }

    Value::Value(int value) :
        m_value(static_cast<double>(value)), m_constType(init_const_type(false, ValueType::Number))
    {}

    Value::Value(float value) :
        m_value(static_cast<double>(value)), m_constType(init_const_type(false, ValueType::Number))
    {}

    Value::Value(double value) :
        m_value(value), m_constType(init_const_type(false, ValueType::Number))
    {}

    Value::Value(const std::string& value) :
        m_value(value.c_str()), m_constType(init_const_type(false, ValueType::String))
    {}

    Value::Value(const String& value) :
        m_value(value), m_constType(init_const_type(false, ValueType::String))
    {}

    Value::Value(const char* value) :
        m_value(value), m_constType(init_const_type(false, ValueType::String))
    {}

    Value::Value(PageAddr_t value) :
        m_value(value), m_constType(init_const_type(false, ValueType::PageAddr))
    {}

    Value::Value(Value::ProcType value) :
        m_value(value), m_constType(init_const_type(false, ValueType::CProc))
    {}

    Value::Value(std::vector<Value>&& value) :
        m_value(value), m_constType(init_const_type(false, ValueType::List))
    {}

    Value::Value(Closure&& value) :
        m_value(value), m_constType(init_const_type(false, ValueType::Closure))
    {}

    Value::Value(UserType&& value) :
        m_value(value), m_constType(init_const_type(false, ValueType::User))
    {}

    // --------------------------

    std::vector<Value>& Value::list()
    {
        return std::get<std::vector<Value>>(m_value);
    }

    Closure& Value::closure_ref()
    {
        return std::get<Closure>(m_value);
    }

    String& Value::string_ref()
    {
        return std::get<String>(m_value);
    }

    UserType& Value::usertype_ref()
    {
        return std::get<UserType>(m_value);
    }

    // --------------------------

    void Value::push_back(const Value& value)
    {
        m_constType = init_const_type(isConst(), ValueType::List);
        list().push_back(value);
    }

    void Value::push_back(Value&& value)
    {
        m_constType = init_const_type(isConst(), ValueType::List);
        list().push_back(value);
    }

    // --------------------------

    int dec_places(double d)
    {
        constexpr double precision = 1e-7;
        double temp = 0.0;
        int decimal_places = 0;

        do
        {
            d *= 10;
            temp = d - static_cast<int>(d);
            decimal_places++;
        } while(temp > precision && decimal_places < std::numeric_limits<double>::digits10);
        
        return decimal_places;
    }

    int dig_places(double d)
    {
        int digit_places = 0;
        int i = static_cast<int>(d);
        while (i != 0)
        {
            digit_places++;
            i /= 10;
        }
        return digit_places;
    }

    std::ostream& operator<<(std::ostream& os, const Value& V)
    {
        switch (V.valueType())
        {
        case ValueType::Number:
        {
            double d = V.number();
            os.precision(dig_places(d) + dec_places(d));
            os << d;
            break;
        }

        case ValueType::String:
            os << V.string().c_str();
            break;

        case ValueType::PageAddr:
            os << "Function @ " << V.pageAddr();
            break;

        case ValueType::CProc:
            os << "CProcedure";
            break;

        case ValueType::List:
        {
            os << "[";
            for (auto it=V.const_list().begin(), it_end=V.const_list().end(); it != it_end; ++it)
            {
                if (it->valueType() == ValueType::String)
                    os << "\"" << (*it) << "\"";
                else
                    os << (*it);
                if (it + 1 != it_end)
                    os << " ";
            }
            os << "]";
            break;
        }

        case ValueType::Closure:
            os << V.closure();
            break;

        case ValueType::User:
            os << V.usertype();
            break;

        case ValueType::Nil:
            os << "nil";
            break;

        case ValueType::True:
            os << "true";
            break;

        case ValueType::False:
            os << "false";
            break;

        case ValueType::Undefined:
            os << "undefined";
            break;

        default:
            os << "~\\._./~";
            break;
        }

        return os;
    }
}