#pragma once
#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>


namespace cload
{

/// A parser for very simple configuration files
struct ConfigurationLoader
{
public:
    /// Default constructor
    ConfigurationLoader() = default;

    /// Constructs a ConfigurationLoader and directly loads the provided file
    explicit
    ConfigurationLoader(std::string const& filename)
    {
        loadFromFile(filename);
    }

    /** Loads the provided file and extract key value pairs from it
     *
     * @param filename The file to open. If the file cannot be opened, the loader becomes invalid.
     */
    void loadFromFile(std::string const& filename)
    {
        // Reset valid state
        m_valid = false;
        std::ifstream infile(filename);
        // If the file could not be opened, stop here. The loader is invalid
        if (!infile) {
            return;
        }

        // The file has been successfully opened
        m_valid = true;

        // Read it line by line and extract key value pairs
        std::string line;
        while (std::getline(infile, line))
        {
            auto const pair = extractPair(extractPayload(line));
            if (pair.first.empty() || pair.second.empty()) {
                continue;
            } else {
                entries[pair.first] = pair.second;
            }
        }
    }

    /** Tries to parse the value into the specified type and put it in the provided destination
     *
     * @tparam TType The target type for the parse of the value
     * @param key The key associated with the value
     * @param destination Where to put the value if the parse is successful
     * @return An optional containing the parsed value if it was found and successfully converted
     */
    template<typename TType>
    bool tryReadValueInto(std::string const& key, TType* destination) const
    {
        std::optional<TType> const value = tryGetValueAs<TType>(key);
        if (value) {
            *destination = *value;
            return true;
        }
        return false;
    }

    bool tryReadBoolInto(std::string const& key, bool* const destination) const
    {
        std::optional<uint8_t> const value = tryGetValueAs<uint8_t>(key);
        if (value) {
            *destination = value != 0;
            return true;
        }
        return false;
    }

    /** Retrieves the raw string data associated with the provided key, if any
     *
     * @param key The key to search for
     * @param destination The destination string
     * @return An optional containing the raw data if found
     */
    template<>
    bool tryReadValueInto<std::string>(std::string const& key, std::string* const destination) const
    {
        std::optional<std::string> const value = tryGetValueAsString(key);
        if (value) {
            *destination = *value;
            return true;
        }
        return false;
    }

    /** Parses the value as a sequence of elements of the same type and put them in the provided array
     *
     * @tparam TType The expected type of the elements
     * @tparam COUNT The size of the provided array, deduced
     * @param key The key to search for
     * @param array The destination array
     * @return True if the parse was successful for all the elements of the array
     */
    template<typename TType, size_t COUNT>
    bool tryReadSequenceIntoArray(std::string const& key, TType (&array)[COUNT]) const
    {
        std::optional<std::string> const value = tryGetValueAsString(key);
        if (!value) {
            return false;
        }

        std::string const& data = *value;
        size_t start = 0;
        for (size_t i{0}; i < COUNT; ++i) {
            auto const val = getNext<TType>(data, &start);
            if (val) {
                array[i] = *val;
            } else {
                return false;
            }
        }

        return true;
    }

    /** Parses the value as a sequence of elements of the same type and put them in the provided array
     *
     * @tparam COUNT The size of the array
     * @tparam TType The expected type of the elements to read
     * @param key The key to search for
     * @param array The destination array
     * @return True if the parse was successful for all the elements of the array
     */
    template<size_t COUNT, typename TType>
    bool tryReadSequenceIntoArray(std::string const& key, TType array[]) const
    {
        std::optional<std::string> const value = tryGetValueAsString(key);
        if (!value) {
            return false;
        }

        std::string const& data = *value;
        size_t start = 0;
        for (size_t i{0}; i < COUNT; ++i) {
            auto const val = getNext<TType>(data, &start);
            if (val) {
                array[i] = *val;
            } else {
                return false;
            }
        }

        return true;
    }

    /** Retrieves all the elements of the sequence associated with the provided key
     *
     * @tparam TType The expected type of the elements
     * @param key The key to search for
     * @return An optional containing the vector of parsed elements if the key was found and the parse successful
     */
    template<typename TType>
    [[nodiscard]]
    std::optional<std::vector<TType>> tryGetSequence(std::string const& key) const
    {
        std::optional<std::string> const value = tryGetValueAsString(key);
        if (!value) {
            return std::nullopt;
        }

        std::string const& data = *value;
        std::vector<TType> result;
        size_t start = 0;
        while (auto const val = getNext<TType>(data, &start)) {
            result.push_back(*val);
        }

        return result;
    }

    /** Retrieves the specified number of elements from the sequence associated with the provided key
     *
     * @tparam TType The expected type of the elements
     * @tparam COUNT The number of elements to load
     * @param key The key to search for
     * @return An optional containing the array of parsed elements if the key was found and the parse successful
     */
    template<typename TType, size_t COUNT>
    [[nodiscard]]
    std::optional<std::array<TType, COUNT>> tryGetSequenceAsArray(std::string const& key) const
    {
        std::optional<std::string> const value = tryGetValueAsString(key);
        if (!value) {
            return std::nullopt;
        }

        std::string const& data = *value;
        std::array<TType, COUNT> result;
        size_t start = 0;
        for (size_t i{0}; i < COUNT; ++i) {
            auto const val = getNext<TType>(data, &start);
            if (val) {
                result[i] = *val;
            } else {
                // If the value cannot be parsed, then the sequence is considered invalid
                return std::nullopt;
            }
        }

        return result;
    }

    /** Retrieves the value associated with the provided key
     *
     * @param key The key to search for
     * @return An optional containing the result if the key was found
     */
    [[nodiscard]]
    std::optional<std::string> tryGetValueAsString(std::string const& key) const
    {
        auto const it = entries.find(key);
        if (it == entries.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    /** Retrieves and parses the value associated with the provided key
     *
     * @tparam TType The expected type of the data
     * @param key The key to search for
     * @return An optional containing the parsed value if the key was found and the parse successful
     */
    template<typename TType>
    [[nodiscard]]
    std::optional<TType> tryGetValueAs(std::string const& key) const
    {
        auto const it = entries.find(key);
        if (it == entries.end()) {
            return std::nullopt;
        }
        return tryParse<TType>(it->second);
    }

    [[nodiscard]]
    bool isValid() const
    {
        return m_valid;
    }

    operator bool() const // NOLINT(*-explicit-constructor)
    {
        return m_valid;
    }

private:
    /// Returns the non commented portion of the line
    static std::string extractPayload(std::string const& s)
    {
        size_t const hashtag = s.find('#');
        if (hashtag == std::string::npos) {
            return s;
        } else {
            std::string payload = s.substr(0, hashtag);
            return payload;
        }
    }

    /// Extracts key = value pair
    static std::pair<std::string, std::string> extractPair(std::string const& s)
    {
        size_t const equal = s.find('=');
        if (equal == std::string::npos) {
            return {"", ""};
        }

        return {strip(s.substr(0, equal)), strip(s.substr(equal+1))};
    }

    /// Removes all spaces from the provided string
    static std::string strip(std::string const& s)
    {
        return std::regex_replace(s, std::regex(" "), "");
    }

    /// Tries to interpret the provided string as the provided float type
    template<typename TType>
    [[nodiscard]]
    std::enable_if_t<std::is_floating_point_v<TType>, std::optional<TType>> tryParse(std::string const& s) const
    {
        try {
            double const result = std::stod(s);
            // Ensure the value is within target range
            if (result >= std::numeric_limits<TType>::min() && result <= std::numeric_limits<TType>::max()) {
                return static_cast<TType>(result);
            }
        }
        catch (std::invalid_argument const&) {}
        // If parse failed or value is not within range
        return std::nullopt;
    }

    template<typename TType>
    [[nodiscard]]
    std::enable_if_t<std::is_same_v<TType, bool>, std::optional<bool>> tryParse(std::string const& s) const
    {
        using LoadType = long long;
        try {
            LoadType const result = std::stoll(s);
            return result != 0;
        }
        catch (std::invalid_argument const&) {}
        // If parse failed or value is not within range
        return std::nullopt;
    }

    /// Tries to interpret the provided string as the provided integer type
    template<typename TType>
    [[nodiscard]]
    std::enable_if_t<std::is_integral_v<TType> && !std::is_same_v<TType, bool>, std::optional<TType>> tryParse(std::string const& s) const
    {
        using LoadType = long long;
        try {
            LoadType const result = std::stoll(s);
            /* Ensure the value is within target range
             * The cast avoids warnings when trying to parse bool.
             * It should not be a problem because stoll reads into this type anyway.
             */
            TType constexpr min_value = std::numeric_limits<TType>::min();
            TType constexpr max_value = std::numeric_limits<TType>::max();
            if ((result >= min_value) && (result <= max_value)) {
                return static_cast<TType>(result);
            }
        }
        catch (std::invalid_argument const&) {}
        // If parse failed or value is not within range
        return std::nullopt;
    }

    /** Retrieves the next element in a coma separated sequence of elements
     *
     * @tparam TType The expected type of the element
     * @param sequence The string containing the coma separated sequence
     * @param pos The current position in the data string
     * @return An optional containing the element if the parse was successful
     */
    template<typename TType>
    [[nodiscard]]
    std::optional<TType> getNext(std::string const& sequence, size_t* pos) const
    {
        size_t const start = *pos;
        // Stop if we are done
        if (start == std::string::npos) {
            return std::nullopt;
        }

        // Else search for the next part to parse
        size_t const coma  = sequence.find(',', start);
        if (coma != std::string::npos) {
            *pos = coma + 1;
            return tryParse<TType>(sequence.substr(start, coma - start));
        }
        *pos = std::string::npos;
        return tryParse<TType>(sequence.substr(start));
    }

private:
    /// The map providing the key/value association
    std::map<std::string, std::string> entries;

    /// Tells if a valid file has been loaded
    bool m_valid = false;
};
}