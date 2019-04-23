#ifndef ark_env
#define ark_env

#include <string>
#include <unordered_map>
#include <iostream>

#include <Ark/Lang/Node.hpp>

namespace Ark
{
    namespace Lang
    {
        class Environment
        {
        public:
            using Map = std::unordered_map<std::string, Node>;

            Environment(Environment* outer=nullptr);
            Environment(Nodes& params, Nodes& args, Environment* outer);

            Environment::Map& find(const std::string& var);
            Node& operator[](const std::string& var);

            friend std::ostream& operator<<(std::ostream& os, const Environment& E);

        private:
            Environment* m_outer;
            Environment::Map m_env;
        };
    }
}

#endif  // ark_env