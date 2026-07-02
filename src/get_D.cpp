#include <iostream>
#include <vector>
#include <stdexcept>
#include <boost/multiprecision/cpp_int.hpp>

using boost::multiprecision::cpp_int;
using Series = std::vector<cpp_int>;

cpp_int binom_nonneg(int n, int k) {
    if (k < 0 || k > n) return 0;
    if (k == 0 || k == n) return 1;
    if (k > n - k) k = n - k;

    cpp_int res = 1;
    for (int i = 1; i <= k; ++i) {
        res *= (n - k + i);
        res /= i;
    }
    return res;
}

// Truncated multiplication of two series up to degree maxDeg
Series multiply(const Series& a, const Series& b, int maxDeg) {
    Series c(maxDeg + 1, 0);
    for (int i = 0; i < (int)a.size(); ++i) {
        for (int j = 0; j < (int)b.size() && i + j <= maxDeg; ++j) {
            c[i + j] += a[i] * b[j];
        }
    }
    return c;
}

// (1 + X)^n
Series pow1plusX(int n, int maxDeg) {
    Series s(maxDeg + 1, 0);
    for (int i = 0; i <= n && i <= maxDeg; ++i) {
        s[i] = binom_nonneg(n, i);
    }
    return s;
}

// (1 - X)^r, for integer r
// If r >= 0: polynomial
// If r < 0: 1 / (1 - X)^(-r), infinite series
Series pow1minusX(int r, int maxDeg) {
    Series s(maxDeg + 1, 0);

    if (r >= 0) {
        for (int i = 0; i <= r && i <= maxDeg; ++i) {
            cpp_int coeff = binom_nonneg(r, i);
            s[i] = (i % 2 == 0 ? coeff : -coeff);
        }
    } else {
        int t = -r; // 1 / (1 - X)^t
        for (int i = 0; i <= maxDeg; ++i) {
            s[i] = binom_nonneg(t + i - 1, i);
        }
    }

    return s;
}

// 1 / (1 - X) = sum_{k>=0} X^k
Series inv1minusX(int maxDeg) {
    return Series(maxDeg + 1, 1);
}

// 1 / (1 + X^2)^m = sum_{k>=0} (-1)^k * C(m+k-1, k) * X^(2k)
Series inv1plusX2_pow_m(int m, int maxDeg) {
    Series s(maxDeg + 1, 0);
    for (int k = 0; 2 * k <= maxDeg; ++k) {
        cpp_int coeff = binom_nonneg(m + k - 1, k);
        s[2 * k] = (k % 2 == 0 ? coeff : -coeff);
    }
    return s;
}

// Truncated version of ps(q, m, n)
Series ps(int q, int m, int n, int maxDeg) {
    if (q == 2) {
        Series a = pow1plusX(n, maxDeg);
        Series b = inv1minusX(maxDeg);
        Series c = inv1plusX2_pow_m(m, maxDeg);
        return multiply(multiply(a, b, maxDeg), c, maxDeg);
    } else {
        int r = m - n - 1;
        Series a = pow1minusX(r, maxDeg);
        Series b = pow1plusX(m, maxDeg);
        return multiply(a, b, maxDeg);
    }
}

// Equivalent to Sage's deg_info(q, m, n), but with a search limit
int deg_info(int q, int m, int n, int maxDeg = 1000) {
    Series f = ps(q, m, n, maxDeg);

    for (int i = 0; i <= maxDeg; ++i) {
        if (f[i] <= 0) {
            return i;
        }
    }
    return -1; // no nonpositive coefficient found up to maxDeg
}

// int main() {
//     int q = 31, m = 160, n = 80;
//     int d = deg_info(q, m, n, 100);
// 
//     std::cout << "deg_info(" << q << ", " << m << ", " << n << ") = " << d << "\n";
//     return 0;
// }

