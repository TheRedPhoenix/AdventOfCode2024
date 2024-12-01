#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <vector>

static constexpr std::string_view kTestFile{"test.txt"};
static constexpr std::string_view kInputFile{"input.txt"};

using Locations = std::vector<int>;

std::vector<std::string> tokenize(const std::string &string,
                                  const std::string &separator) {
  if (string.empty()) {
    return {};
  }

  std::vector<std::string> tokenized;

  using re_iterator = std::sregex_token_iterator;

  static const auto sep = std::regex{separator};

  std::transform(re_iterator{string.begin(), string.end(), sep, -1},
                 re_iterator{}, std::back_insert_iterator(tokenized),
                 [&](const auto &item) { return item.str(); });
  return tokenized;
}

std::pair<Locations, Locations>
getLocationsFromFile(const std::filesystem::path &path) {

  if (!std::filesystem::exists(path)) {
    throw std::runtime_error(
        std::format("Error: file {} does not exist", path.string()));
  }

  std::ifstream locationsFile;
  locationsFile.open(path, std::ios_base::in);
  if (!locationsFile.is_open()) {
    throw std::runtime_error(
        std::format("Error: cannot open file {}", path.string()));
  }

  auto parseLine = [](const std::string &line) -> std::pair<int, int> {
    const auto numbers = tokenize(line, R"(\s+)");
    if (numbers.size() != 2) {
      throw std::runtime_error(std::format(
          "Error: expected two entries per line, found {}", numbers.size()));
    }

    try {
      return std::make_pair(std::stoi(numbers.at(0)), std::stoi(numbers.at(1)));
    } catch (const std::exception &e) {
      throw std::runtime_error(
          std::format("Error: Cannot convert either \'{}\' or \'{}\' string to "
                      "integer. Exception: {}",
                      numbers.at(0), numbers.at(1), e.what()));
    }
  };

  Locations leftLocations;
  Locations rightLocations;

  std::string line;
  while (std::getline(locationsFile, line)) {
    const auto &[left, right] = parseLine(line);
    leftLocations.push_back(left);
    rightLocations.push_back(right);
  }

  return std::make_pair(leftLocations, rightLocations);
}

int computeDistanceSum(Locations &left, Locations &right) {
  std::ranges::sort(left);
  std::ranges::sort(right);

  if (left.size() != right.size()) {
    throw std::runtime_error(std::format(
        "Error: left and right locations vector are expected to have the same "
        "size, but left has {} elements and right has {} elements",
        left.size(), right.size()));
  }

  std::vector<int> differences;
  differences.reserve(left.size());

  std::ranges::transform(left, right, std::back_inserter(differences),
                         [](int l, int r) { return std::abs(l - r); });

  return std::accumulate(differences.begin(), differences.end(), 0);
}

int computeSimilarityScore(const Locations& left, const Locations& right) {
  std::vector<int> similarities;
  std::ranges::transform(left, std::back_inserter(similarities),
          [&right](int locationId) {
            return locationId *
                    std::ranges::count(right, locationId);
          });

  return std::accumulate(similarities.begin(), similarities.end(), 0);
}

int main(int argc, char *argv[]) {

  {
    // Test Part 1
    static const auto expectedTestResultPart1 = 11;
    auto locations = getLocationsFromFile(kTestFile);
    if (const auto result =
            computeDistanceSum(locations.first, locations.second);
        expectedTestResultPart1 != result) {
      throw std::runtime_error(std::format("Error TEST 1! Expected {}, got {}",
                                           expectedTestResultPart1, result));
    }

    // Test Part 2
    static const auto expectedTestResultPart2 = 31;
    if (const auto result =
            computeSimilarityScore(locations.first, locations.second);
            expectedTestResultPart2 != result) {
      throw std::runtime_error(std::format("Error TEST 2! Expected {}, got {}",
              expectedTestResultPart2, result));
    }
  }

  // Part 1
  auto locations = getLocationsFromFile(kInputFile);
  const auto distanceSum =
      computeDistanceSum(locations.first, locations.second);
  std::cout << "The sum of distances is : " << distanceSum << std::endl;

  // Part 2
  const auto similarityScore = computeSimilarityScore(locations.first, locations.second);
  std::cout << "The similarity score is : " << similarityScore << std::endl;

  return 0;
}
