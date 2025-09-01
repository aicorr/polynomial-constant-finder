#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

// Simple JSON parser for this specific format
class SimpleJSONParser {
public:
    std::map<std::string, std::string> data;
    
    void parse(const std::string& jsonStr) {
        std::string clean = jsonStr;
        // Remove whitespace, newlines, and braces
        clean.erase(std::remove_if(clean.begin(), clean.end(), ::isspace), clean.end());
        
        size_t pos = 0;
        while ((pos = clean.find('"', pos)) != std::string::npos) {
            size_t keyStart = pos + 1;
            size_t keyEnd = clean.find('"', keyStart);
            if (keyEnd == std::string::npos) break;
            
            std::string key = clean.substr(keyStart, keyEnd - keyStart);
            
            pos = clean.find(':', keyEnd);
            if (pos == std::string::npos) break;
            pos++;
            
            if (clean[pos] == '"') {
                // String value
                pos++;
                size_t valueEnd = clean.find('"', pos);
                if (valueEnd == std::string::npos) break;
                std::string value = clean.substr(pos, valueEnd - pos);
                data[key] = value;
                pos = valueEnd + 1;
            } else if (clean[pos] == '{') {
                // Object value - skip for now
                int braceCount = 1;
                pos++;
                while (braceCount > 0 && pos < clean.length()) {
                    if (clean[pos] == '{') braceCount++;
                    else if (clean[pos] == '}') braceCount--;
                    pos++;
                }
            } else {
                // Number value
                size_t valueEnd = clean.find_first_of(",}", pos);
                if (valueEnd == std::string::npos) valueEnd = clean.length();
                std::string value = clean.substr(pos, valueEnd - pos);
                data[key] = value;
                pos = valueEnd;
            }
        }
    }
    
    std::string getValue(const std::string& key) {
        return data.count(key) ? data[key] : "";
    }
};

// Advanced JSON parser for nested structures
class JSONParser {
private:
    std::string json;
    size_t pos;
    
    void skipWhitespace() {
        while (pos < json.length() && std::isspace(json[pos])) {
            pos++;
        }
    }
    
    std::string parseString() {
        if (json[pos] != '"') return "";
        pos++; // skip opening quote
        
        std::string result;
        while (pos < json.length() && json[pos] != '"') {
            result += json[pos];
            pos++;
        }
        pos++; // skip closing quote
        return result;
    }
    
    std::string parseValue() {
        skipWhitespace();
        if (json[pos] == '"') {
            return parseString();
        } else {
            std::string result;
            while (pos < json.length() && json[pos] != ',' && json[pos] != '}' && json[pos] != ']') {
                if (!std::isspace(json[pos])) {
                    result += json[pos];
                }
                pos++;
            }
            return result;
        }
    }
    
public:
    struct Point {
        int x;
        int base;
        std::string value;
    };
    
    struct TestCase {
        int n, k;
        std::vector<Point> points;
    };
    
    TestCase parseTestCase(const std::string& jsonStr) {
        json = jsonStr;
        pos = 0;
        TestCase testCase;
        
        // Find keys section
        size_t keysPos = json.find("\"keys\"");
        if (keysPos != std::string::npos) {
            pos = keysPos;
            // Find n value
            size_t nPos = json.find("\"n\"", pos);
            if (nPos != std::string::npos) {
                pos = json.find(':', nPos) + 1;
                testCase.n = std::stoi(parseValue());
            }
            
            // Find k value
            size_t kPos = json.find("\"k\"", pos);
            if (kPos != std::string::npos) {
                pos = json.find(':', kPos) + 1;
                testCase.k = std::stoi(parseValue());
            }
        }
        
        // Parse points
        pos = 0;
        while (pos < json.length()) {
            skipWhitespace();
            if (pos >= json.length()) break;
            
            // Look for numbered keys (not "keys")
            if (json[pos] == '"') {
                size_t keyStart = pos + 1;
                size_t keyEnd = json.find('"', keyStart);
                if (keyEnd == std::string::npos) break;
                
                std::string key = json.substr(keyStart, keyEnd - keyStart);
                
                // Check if it's a number (not "keys", "n", "k", "base", "value")
                if (key != "keys" && key != "n" && key != "k" && key != "base" && key != "value" && 
                    std::all_of(key.begin(), key.end(), ::isdigit)) {
                    
                    Point point;
                    point.x = std::stoi(key);
                    
                    // Find the object for this point
                    pos = json.find('{', keyEnd);
                    if (pos == std::string::npos) break;
                    
                    size_t objEnd = pos;
                    int braceCount = 1;
                    objEnd++;
                    while (braceCount > 0 && objEnd < json.length()) {
                        if (json[objEnd] == '{') braceCount++;
                        else if (json[objEnd] == '}') braceCount--;
                        objEnd++;
                    }
                    
                    std::string objStr = json.substr(pos, objEnd - pos);
                    
                    // Parse base
                    size_t basePos = objStr.find("\"base\"");
                    if (basePos != std::string::npos) {
                        size_t baseValuePos = objStr.find(':', basePos) + 1;
                        size_t baseStart = objStr.find('"', baseValuePos) + 1;
                        size_t baseEnd = objStr.find('"', baseStart);
                        point.base = std::stoi(objStr.substr(baseStart, baseEnd - baseStart));
                    }
                    
                    // Parse value
                    size_t valuePos = objStr.find("\"value\"");
                    if (valuePos != std::string::npos) {
                        size_t valueValuePos = objStr.find(':', valuePos) + 1;
                        size_t valueStart = objStr.find('"', valueValuePos) + 1;
                        size_t valueEnd = objStr.find('"', valueStart);
                        point.value = objStr.substr(valueStart, valueEnd - valueStart);
                    }
                    
                    testCase.points.push_back(point);
                    pos = objEnd;
                } else {
                    pos = keyEnd + 1;
                }
            } else {
                pos++;
            }
        }
        
        return testCase;
    }
};

// Convert from any base to decimal using long double for better precision
long double convertToDecimal(const std::string& value, int base) {
    long double result = 0.0L;
    long double power = 1.0L;
    
    for (int i = value.length() - 1; i >= 0; i--) {
        char digit = value[i];
        int digitValue;
        
        if (digit >= '0' && digit <= '9') {
            digitValue = digit - '0';
        } else if (digit >= 'a' && digit <= 'z') {
            digitValue = digit - 'a' + 10;
        } else if (digit >= 'A' && digit <= 'Z') {
            digitValue = digit - 'A' + 10;
        } else {
            throw std::invalid_argument("Invalid character in number");
        }
        
        if (digitValue >= base) {
            throw std::invalid_argument("Digit value exceeds base");
        }
        
        result += digitValue * power;
        power *= base;
    }
    
    return result;
}

// Gaussian elimination with partial pivoting for better numerical stability
long double gaussianElimination(const std::vector<std::pair<int, long double>>& points) {
    int n = points.size();
    
    // Create augmented matrix [A|B] where Ax = B
    // For polynomial of degree (n-1): x₀^i * a₀ + x₁^i * a₁ + ... + xₙ₋₁^i * aₙ₋₁ = y₀
    std::vector<std::vector<long double>> matrix(n, std::vector<long double>(n + 1));
    
    // Fill the matrix
    for (int i = 0; i < n; i++) {
        long double x = points[i].first;
        long double y = points[i].second;
        
        // Fill coefficients: 1, x, x², x³, ..., x^(n-1)
        long double power = 1.0L;
        for (int j = 0; j < n; j++) {
            matrix[i][j] = power;
            power *= x;
        }
        matrix[i][n] = y; // RHS
    }
    
    // Forward elimination with partial pivoting
    for (int i = 0; i < n; i++) {
        // Find pivot
        int maxRow = i;
        for (int k = i + 1; k < n; k++) {
            if (std::abs(matrix[k][i]) > std::abs(matrix[maxRow][i])) {
                maxRow = k;
            }
        }
        
        // Swap rows
        if (maxRow != i) {
            std::swap(matrix[i], matrix[maxRow]);
        }
        
        // Check for zero pivot
        if (std::abs(matrix[i][i]) < 1e-15L) {
            throw std::runtime_error("Matrix is singular");
        }
        
        // Eliminate
        for (int k = i + 1; k < n; k++) {
            long double factor = matrix[k][i] / matrix[i][i];
            for (int j = i; j <= n; j++) {
                matrix[k][j] -= factor * matrix[i][j];
            }
        }
    }
    
    // Back substitution
    std::vector<long double> solution(n);
    for (int i = n - 1; i >= 0; i--) {
        solution[i] = matrix[i][n];
        for (int j = i + 1; j < n; j++) {
            solution[i] -= matrix[i][j] * solution[j];
        }
        solution[i] /= matrix[i][i];
    }
    
    // Return constant term (coefficient of x⁰)
    return solution[0];
}

// Alternative method: Using Newton's divided differences
long double newtonDividedDifferences(const std::vector<std::pair<int, long double>>& points) {
    int n = points.size();
    std::vector<std::vector<long double>> table(n, std::vector<long double>(n));
    
    // Initialize first column with y values
    for (int i = 0; i < n; i++) {
        table[i][0] = points[i].second;
    }
    
    // Fill the divided difference table
    for (int j = 1; j < n; j++) {
        for (int i = 0; i < n - j; i++) {
            table[i][j] = (table[i + 1][j - 1] - table[i][j - 1]) / 
                         (points[i + j].first - points[i].first);
        }
    }
    
    // Calculate f(0) using Newton's interpolation formula
    long double result = table[0][0]; // f[x₀]
    long double product = 1.0L;
    
    for (int i = 1; i < n; i++) {
        product *= (0 - points[i - 1].first); // (0 - x₀)(0 - x₁)...(0 - xᵢ₋₁)
        result += table[0][i] * product;
    }
    
    return result;
}

// Method using modular arithmetic for extremely large numbers (if needed)
class ModularArithmetic {
private:
    static const long long MOD = 1000000007LL; // Large prime
    
    long long modPow(long long base, long long exp, long long mod) {
        long long result = 1;
        while (exp > 0) {
            if (exp % 2 == 1) {
                result = (result * base) % mod;
            }
            base = (base * base) % mod;
            exp /= 2;
        }
        return result;
    }
    
    long long modInverse(long long a, long long mod) {
        return modPow(a, mod - 2, mod);
    }
    
public:
    long long lagrangeModular(const std::vector<std::pair<int, long long>>& points) {
        int n = points.size();
        long long result = 0;
        
        for (int i = 0; i < n; i++) {
            long long numerator = points[i].second % MOD;
            long long denominator = 1;
            
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    numerator = (numerator * ((-points[j].first) % MOD + MOD)) % MOD;
                    denominator = (denominator * ((points[i].first - points[j].first) % MOD + MOD)) % MOD;
                }
            }
            
            result = (result + numerator * modInverse(denominator, MOD)) % MOD;
        }
        
        return result;
    }
};

int main() {
    // Test Case 1
    std::string testCase1 = R"({
        "keys": {
            "n": 4,
            "k": 3
        },
        "1": {
            "base": "10",
            "value": "4"
        },
        "2": {
            "base": "2",
            "value": "111"
        },
        "3": {
            "base": "10",
            "value": "12"
        },
        "6": {
            "base": "4",
            "value": "213"
        }
    })";
    
    // Test Case 2
    std::string testCase2 = R"({
        "keys": {
            "n": 10,
            "k": 7
        },
        "1": {
            "base": "6",
            "value": "13444211440455345511"
        },
        "2": {
            "base": "15",
            "value": "aed7015a346d635"
        },
        "3": {
            "base": "15",
            "value": "6aeeb69631c227c"
        },
        "4": {
            "base": "16",
            "value": "e1b5e05623d881f"
        },
        "5": {
            "base": "8",
            "value": "316034514573652620673"
        },
        "6": {
            "base": "3",
            "value": "2122212201122002221120200210011020220200"
        },
        "7": {
            "base": "3",
            "value": "20120221122211000100210021102001201112121"
        },
        "8": {
            "base": "6",
            "value": "20220554335330240002224253"
        },
        "9": {
            "base": "12",
            "value": "45153788322a1255483"
        },
        "10": {
            "base": "7",
            "value": "1101613130313526312514143"
        }
    })";
    
    JSONParser parser;
    
    // Process Test Case 1
    std::cout << "=== Test Case 1 ===" << std::endl;
    auto tc1 = parser.parseTestCase(testCase1);
    
    std::vector<std::pair<int, long double>> points1;
    std::cout << "Points (x, y):" << std::endl;
    
    for (int i = 0; i < std::min(tc1.k, (int)tc1.points.size()); i++) {
        auto& point = tc1.points[i];
        long double y = convertToDecimal(point.value, point.base);
        points1.push_back({point.x, y});
        std::cout << "(" << point.x << ", " << std::fixed << std::setprecision(0) << y 
                  << ") [base " << point.base << ": " << point.value << "]" << std::endl;
    }
    
    std::cout << "\nUsing different methods:" << std::endl;
    
    try {
        long double constant1_gauss = gaussianElimination(points1);
        std::cout << "Gaussian elimination: c = " << std::fixed << std::setprecision(0) 
                  << std::round(constant1_gauss) << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Gaussian elimination failed: " << e.what() << std::endl;
    }
    
    try {
        long double constant1_newton = newtonDividedDifferences(points1);
        std::cout << "Newton's method: c = " << std::fixed << std::setprecision(0) 
                  << std::round(constant1_newton) << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Newton's method failed: " << e.what() << std::endl;
    }
    
    std::cout << std::endl;
    
    // Process Test Case 2
    std::cout << "=== Test Case 2 ===" << std::endl;
    auto tc2 = parser.parseTestCase(testCase2);
    
    std::vector<std::pair<int, long double>> points2;
    std::cout << "Points (x, y) - using first " << tc2.k << " points:" << std::endl;
    
    for (int i = 0; i < std::min(tc2.k, (int)tc2.points.size()); i++) {
        auto& point = tc2.points[i];
        long double y = convertToDecimal(point.value, point.base);
        points2.push_back({point.x, y});
        std::cout << "(" << point.x << ", " << std::scientific << std::setprecision(6) << y 
                  << ") [base " << point.base << "]" << std::endl;
    }
    
    std::cout << "\nUsing different methods:" << std::endl;
    
    try {
        long double constant2_gauss = gaussianElimination(points2);
        std::cout << "Gaussian elimination: c = " << std::scientific << std::setprecision(15) 
                  << constant2_gauss << std::endl;
        std::cout << "Gaussian elimination (rounded): c = " << std::fixed << std::setprecision(0) 
                  << std::round(constant2_gauss) << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Gaussian elimination failed: " << e.what() << std::endl;
    }
    
    try {
        long double constant2_newton = newtonDividedDifferences(points2);
        std::cout << "Newton's method: c = " << std::scientific << std::setprecision(15) 
                  << constant2_newton << std::endl;
        std::cout << "Newton's method (rounded): c = " << std::fixed << std::setprecision(0) 
                  << std::round(constant2_newton) << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Newton's method failed: " << e.what() << std::endl;
    }

#ifdef _WIN32
    system("pause");
#endif
    
    return 0;
}