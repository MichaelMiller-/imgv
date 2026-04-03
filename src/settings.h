#pragma once

#include <filesystem>
#include <fstream>
#include <ostream>
#include <string>
#include <unordered_map>

namespace imgv
{
   class settings
   {
      using value_t = std::unordered_map<std::string, std::string>;
      value_t m_value;

   public:
      settings() = default;
      explicit settings(auto const& filename) { std::ignore = read(filename); }

      class value
      {
         std::string m_value;

      public:
         explicit value(std::string v) : m_value{std::move(v)} {}

         [[nodiscard]] auto to_string() const noexcept { return m_value; };

         [[nodiscard]] auto to_int() const
         {
#if 0
            int number;
            auto [ptr, ec] = std::from_chars(m_value.data(), m_value.data() + m_value.size(), number);
            assert(ec == std::errc{});
            return number;
#else
            return std::stoi(m_value);
#endif
         };
      };

      [[nodiscard]] auto operator[](std::string const& key) { return value{m_value.at(key)}; }

      [[nodiscard]] auto read(auto const& filename) -> bool
      try {
         std::ifstream ifs{filename};
         std::string line;
         while (std::getline(ifs, line)) {
            // Skip comments
            if (line.empty() || line[0] == '#') {
               continue;
            }

            // Split line into key and value based on '='
            auto delimiter = line.find('=');
            if (delimiter != std::string::npos) {
               auto key = line.substr(0, delimiter);
               auto value = line.substr(delimiter + 1);

               // Trim whitespace
               key.erase(key.find_last_not_of(" \t\n\r\f\v") + 1);
               value.erase(value.find_last_not_of(" \t\n\r\f\v") + 1);

               m_value[key] = value;
            }
         }
         return true;
      } catch (std::exception const& ex) {
         std::puts(ex.what());
         return false;
      }

      [[nodiscard]] auto write(auto const& filename) const -> bool
      try {
         std::ofstream ofs{filename};
         for (auto const& [key, value] : m_value) {
            ofs << key << " = " << value << '\n';
         }
      } catch (std::exception const& ex) {
         std::puts(ex.what());
         return false;
      }
   };

} // namespace imgv