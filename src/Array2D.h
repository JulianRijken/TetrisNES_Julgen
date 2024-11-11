// Credits to Learn++ for providing Array2D implementation
// https://www.learncpp.com/cpp-tutorial/multidimensional-stdarray/


#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <functional>

template<typename T, std::size_t Row, std::size_t Col>
class Array2D
{

public:
    Array2D(std::array<T, Row * Col>&& arr) :
        m_Array{ std::move(arr) }
    {
    }

    Array2D(const std::array<std::array<T, Col>, Row>& arr2D) :
        m_Array(
            [&arr2D]()
            {
                std::array<T, Row * Col> temp{};
                std::size_t index = 0;
                for(const auto& row : arr2D)
                    for(const auto& elem : row)
                        temp[index++] = elem;

                return temp;
            }())
    {
    }

    // Get element via single subscript (using operator[])
    T& operator[](int i) { return m_Array.get()[static_cast<std::size_t>(i)]; }

    const T& operator[](int i) const { return m_Array.get()[static_cast<std::size_t>(i)]; }

    // Get element via 2d subscript (using operator(), since operator[] doesn't support multiple dimensions prior to
    // C++23)
    T& operator()(int row, int col) { return m_Array.get()[static_cast<std::size_t>(row * Cols() + col)]; }

    const T& operator()(int row, int col) const { return m_Array.get()[static_cast<std::size_t>(row * Cols() + col)]; }

    // in C++23, you can uncomment these since multidimensional operator[] is supported
    //    T& operator[](int row, int col) { return m_arr.get()[static_cast<std::size_t>(row * cols() + col)]; }
    //    const T& operator[](int row, int col) const { return m_arr.get()[static_cast<std::size_t>(row * cols() +
    //    col)]; }


    [[nodiscard]] int Rows() const { return static_cast<int>(Row); }

    [[nodiscard]] int Cols() const { return static_cast<int>(Col); }

    [[nodiscard]] int Length() const { return static_cast<int>(Row * Col); }

private:
    // You might be tempted to make m_Array a reference to an ArrayFlat2D,
    // but this makes the view non-copy-assignable since references can't be reseated.
    // Using std::reference_wrapper gives us reference semantics and copy assignability.
    std::array<T, Row * Col> m_Array{};
};

#endif  // ARRAY2D_H
