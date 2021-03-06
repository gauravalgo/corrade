/*
    This file is part of Corrade.

    Copyright © 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016,
                2017, 2018, 2019 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <sstream>

#include "Corrade/Containers/ArrayView.h"
#include "Corrade/TestSuite/Tester.h"

namespace {

struct IntView {
    IntView(int* data, std::size_t size): data{data}, size{size} {}

    int* data;
    std::size_t size;
};

struct ConstIntView {
    constexpr ConstIntView(const int* data, std::size_t size): data{data}, size{size} {}

    const int* data;
    std::size_t size;
};

}

namespace Corrade { namespace Containers {

namespace Implementation {

template<> struct ArrayViewConverter<int, IntView> {
    /* Needed only by convertVoidFromExternalView() */
    static ArrayView<int> from(IntView other) {
        return {other.data, other.size};
    }
};

template<> struct ErasedArrayViewConverter<const IntView>: ArrayViewConverter<int, IntView> {};

template<> struct ArrayViewConverter<const int, ConstIntView> {
    constexpr static ArrayView<const int> from(ConstIntView other) {
        return {other.data, other.size};
    }

    constexpr static ConstIntView to(ArrayView<const int> other) {
        return {other.data(), other.size()};
    }
};

template<> struct ErasedArrayViewConverter<ConstIntView>: ArrayViewConverter<const int, ConstIntView> {};
template<> struct ErasedArrayViewConverter<const ConstIntView>: ArrayViewConverter<const int, ConstIntView> {};

/* To keep the ArrayView API in reasonable bounds, the cost-adding variants
   have to be implemented explicitly */
template<> struct ArrayViewConverter<const int, IntView> {
    static ArrayView<const int> from(IntView other) {
        return {other.data, other.size};
    }
};
template<> struct ArrayViewConverter<int, ConstIntView> {
    constexpr static ConstIntView to(ArrayView<int> other) {
        return {other.data(), other.size()};
    }
};

}

namespace Test { namespace {

struct ArrayViewTest: TestSuite::Tester {
    explicit ArrayViewTest();

    void constructEmpty();
    void constructEmptyVoid();
    void constructNullptr();
    void constructNullptrVoid();
    void constructNullptrSize();
    void construct();
    void constructFixedSize();
    void constructFromStatic();
    void constructDerived();
    void constructVoid();

    void convertBool();
    void convertPointer();
    void convertConst();
    void convertVoid();
    void convertExternalView();
    void convertConstFromExternalView();
    void convertToConstExternalView();
    void convertVoidFromExternalView();
    void convertVoidFromConstExternalView();

    void emptyCheck();
    void access();
    void accessVoid();
    void accessConst();
    void accessInvalid();
    void rangeBasedFor();

    void sliceInvalid();
    void sliceNullptr();
    void slice();
    void slicePointer();
    void sliceToStatic();
    void sliceToStaticPointer();

    void cast();
    void castInvalid();
    void size();
};

typedef Containers::ArrayView<int> ArrayView;
typedef Containers::ArrayView<const int> ConstArrayView;
typedef Containers::ArrayView<const void> VoidArrayView;

ArrayViewTest::ArrayViewTest() {
    addTests({&ArrayViewTest::constructEmpty,
              &ArrayViewTest::constructEmptyVoid,
              &ArrayViewTest::constructNullptr,
              &ArrayViewTest::constructNullptrVoid,
              &ArrayViewTest::constructNullptrSize,
              &ArrayViewTest::construct,
              &ArrayViewTest::constructFixedSize,
              &ArrayViewTest::constructFromStatic,
              &ArrayViewTest::constructDerived,
              &ArrayViewTest::constructVoid,

              &ArrayViewTest::convertBool,
              &ArrayViewTest::convertPointer,
              &ArrayViewTest::convertConst,
              &ArrayViewTest::convertVoid,
              &ArrayViewTest::convertExternalView,
              &ArrayViewTest::convertConstFromExternalView,
              &ArrayViewTest::convertToConstExternalView,
              &ArrayViewTest::convertVoidFromExternalView,
              &ArrayViewTest::convertVoidFromConstExternalView,

              &ArrayViewTest::emptyCheck,
              &ArrayViewTest::access,
              &ArrayViewTest::accessVoid,
              &ArrayViewTest::accessConst,
              &ArrayViewTest::accessInvalid,
              &ArrayViewTest::rangeBasedFor,

              &ArrayViewTest::sliceInvalid,
              &ArrayViewTest::sliceNullptr,
              &ArrayViewTest::slice,
              &ArrayViewTest::slicePointer,
              &ArrayViewTest::sliceToStatic,
              &ArrayViewTest::sliceToStaticPointer,

              &ArrayViewTest::cast,
              &ArrayViewTest::castInvalid,
              &ArrayViewTest::size});
}

void ArrayViewTest::constructEmpty() {
    ArrayView a;
    CORRADE_VERIFY(a == nullptr);
    CORRADE_VERIFY(a.empty());
    CORRADE_COMPARE(a.size(), 0);

    constexpr ArrayView ca;
    CORRADE_VERIFY(ca == nullptr);
    CORRADE_VERIFY(ca.empty());
    CORRADE_COMPARE(ca.size(), 0);
}

void ArrayViewTest::constructEmptyVoid() {
    VoidArrayView a;
    CORRADE_VERIFY(a == nullptr);
    CORRADE_VERIFY(a.empty());
    CORRADE_COMPARE(a.size(), 0);

    constexpr ArrayView ca;
    CORRADE_VERIFY(ca == nullptr);
    CORRADE_VERIFY(ca.empty());
    CORRADE_COMPARE(ca.size(), 0);
}

void ArrayViewTest::constructNullptr() {
    ArrayView a = nullptr;
    CORRADE_VERIFY(a == nullptr);
    CORRADE_VERIFY(a.empty());
    CORRADE_COMPARE(a.size(), 0);

    constexpr ArrayView ca = nullptr;
    CORRADE_VERIFY(ca == nullptr);
    CORRADE_VERIFY(ca.empty());
    CORRADE_COMPARE(ca.size(), 0);
}

void ArrayViewTest::constructNullptrVoid() {
    VoidArrayView a = nullptr;
    CORRADE_VERIFY(a == nullptr);
    CORRADE_VERIFY(a.empty());
    CORRADE_COMPARE(a.size(), 0);

    constexpr VoidArrayView ca = nullptr;
    CORRADE_VERIFY(ca == nullptr);
    CORRADE_VERIFY(ca.empty());
    CORRADE_COMPARE(ca.size(), 0);
}

void ArrayViewTest::constructNullptrSize() {
    /* This should be allowed for e.g. just allocating memory in
       Magnum::Buffer::setData() without passing any actual data */
    ArrayView a{nullptr, 5};
    CORRADE_VERIFY(a == nullptr);
    CORRADE_VERIFY(!a.empty());
    CORRADE_COMPARE(a.size(), 5);

    constexpr ArrayView ca{nullptr, 5};
    CORRADE_VERIFY(ca == nullptr);
    CORRADE_VERIFY(!a.empty());
    CORRADE_COMPARE(ca.size(), 5);
}

/* Needs to be here in order to use it in constexpr */
constexpr int Array30[30]{};

void ArrayViewTest::construct() {
    int a[30];

    {
        ArrayView b = {a, 20};
        CORRADE_VERIFY(b == a);
        CORRADE_COMPARE(b.size(), 20);
    } {
        auto b = arrayView(a, 20);
        CORRADE_VERIFY((std::is_same<decltype(b), ArrayView>::value));
        CORRADE_VERIFY(b == a);
        CORRADE_COMPARE(b.size(), 20);

        auto c = arrayView(b);
        CORRADE_VERIFY((std::is_same<decltype(c), ArrayView>::value));
        CORRADE_VERIFY(c == a);
        CORRADE_COMPARE(c.size(), 20);
    }

    {
        constexpr ConstArrayView b = {Array30, 20};
        CORRADE_VERIFY(b == Array30);
        CORRADE_COMPARE(b.size(), 20);
    } {
        constexpr auto b = arrayView(Array30, 20);
        CORRADE_VERIFY((std::is_same<decltype(b), const ConstArrayView>::value));
        CORRADE_VERIFY(b == Array30);
        CORRADE_COMPARE(b.size(), 20);

        constexpr auto c = arrayView(b);
        CORRADE_VERIFY((std::is_same<decltype(c), const ConstArrayView>::value));
        CORRADE_VERIFY(c == Array30);
        CORRADE_COMPARE(c.size(), 20);
    }
}

/* Needs to be here in order to use it in constexpr */
constexpr int Array13[13]{};

void ArrayViewTest::constructFixedSize() {
    int a[13];

    {
        ArrayView b = a;
        CORRADE_VERIFY(b == a);
        CORRADE_COMPARE(b.size(), 13);
    } {
        auto b = arrayView(a);
        CORRADE_VERIFY((std::is_same<decltype(b), ArrayView>::value));
        CORRADE_VERIFY(b == a);
        CORRADE_COMPARE(b.size(), 13);
    }

    {
        constexpr ConstArrayView b = Array13;
        CORRADE_VERIFY(b == Array13);
        CORRADE_COMPARE(b.size(), 13);
    } {
        constexpr auto b = arrayView(Array13);
        CORRADE_VERIFY((std::is_same<decltype(b), const ConstArrayView>::value));
        CORRADE_VERIFY(b == Array13);
        CORRADE_COMPARE(b.size(), 13);
    }

    /* Implicit construction from pointer should not be allowed */
    CORRADE_VERIFY(!(std::is_convertible<int*, ArrayView>::value));
}

void ArrayViewTest::constructFromStatic() {
    int a[13];
    StaticArrayView<13, int> av = a;
    constexpr StaticArrayView<13, const int> cav = Array13;

    {
        ArrayView b = av;
        CORRADE_VERIFY(b == a);
        CORRADE_COMPARE(b.size(), 13);
    } {
        auto b = arrayView(av);
        CORRADE_VERIFY((std::is_same<decltype(b), ArrayView>::value));
        CORRADE_VERIFY(b == a);
        CORRADE_COMPARE(b.size(), 13);
    }

    {
        constexpr ConstArrayView b = cav;
        CORRADE_VERIFY(b == cav);
        CORRADE_COMPARE(b.size(), 13);
    } {
        constexpr auto b = arrayView(cav);
        CORRADE_VERIFY((std::is_same<decltype(b), const ConstArrayView>::value));
        CORRADE_VERIFY(b == cav);
        CORRADE_COMPARE(b.size(), 13);
    }
}

/* Needs to be here in order to use it in constexpr */
struct Base {
    constexpr Base(): i{} {}
    int i;
};
struct Derived: Base {
    constexpr Derived() {}
};
constexpr Derived DerivedArray[5]
    /* This missing makes MSVC2015 complain it's not constexpr, but if present
       then GCC 4.8 fails to build. Eh. ¯\_(ツ)_/¯ */
    #ifdef CORRADE_MSVC2015_COMPATIBILITY
    {}
    #endif
    ;

void ArrayViewTest::constructDerived() {
    /* Valid use case: constructing Containers::ArrayView<Math::Vector<3, Float>>
       from Containers::ArrayView<Color3> because the data have the same size
       and data layout */

    Derived b[5];
    Containers::ArrayView<Derived> bv{b};
    Containers::ArrayView<Base> a{b};
    Containers::ArrayView<Base> av{bv};

    CORRADE_VERIFY(a == &b[0]);
    CORRADE_VERIFY(av == &b[0]);
    CORRADE_COMPARE(a.size(), 5);
    CORRADE_COMPARE(av.size(), 5);

    constexpr Containers::ArrayView<const Derived> cbv{DerivedArray};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY
    constexpr /* Implicit pointer downcast not constexpr on MSVC 2015 */
    #endif
    Containers::ArrayView<const Base> ca{DerivedArray};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY
    constexpr /* Implicit pointer downcast not constexpr on MSVC 2015 */
    #endif
    Containers::ArrayView<const Base> cav{cbv};

    CORRADE_VERIFY(ca == &DerivedArray[0]);
    CORRADE_VERIFY(cav == &DerivedArray[0]);
    CORRADE_COMPARE(ca.size(), 5);
    CORRADE_COMPARE(cav.size(), 5);
}

void ArrayViewTest::constructVoid() {
    void* a = reinterpret_cast<void*>(0xdeadbeef);
    VoidArrayView b(a, 25);
    CORRADE_VERIFY(b == a);
    CORRADE_VERIFY(!b.empty());
    CORRADE_COMPARE(b.size(), 25);

    int* c = reinterpret_cast<int*>(0xdeadbeef);
    VoidArrayView d(c, 25);
    CORRADE_VERIFY(d == c);
    CORRADE_VERIFY(!d.empty());
    CORRADE_COMPARE(d.size(), 100);

    constexpr VoidArrayView cd{Array30, 25};
    CORRADE_VERIFY(cd == Array30);
    CORRADE_VERIFY(!cd.empty());
    CORRADE_COMPARE(cd.size(), 100);
}

void ArrayViewTest::convertBool() {
    int a[7];
    CORRADE_VERIFY(ArrayView(a));
    CORRADE_VERIFY(!ArrayView());
    CORRADE_VERIFY(VoidArrayView(a));
    CORRADE_VERIFY(!VoidArrayView());

    constexpr ConstArrayView cb = Array30;
    constexpr bool boolCb = !!cb;
    CORRADE_VERIFY(boolCb);

    constexpr ConstArrayView cc;
    constexpr bool boolCc = !!cc;
    CORRADE_VERIFY(!boolCc);

    constexpr VoidArrayView cvb = Array30;
    constexpr bool boolCvb = !!cvb;
    CORRADE_VERIFY(boolCvb);

    constexpr VoidArrayView cvc;
    constexpr bool boolCvc = !!cvc;
    CORRADE_VERIFY(!boolCvc);

    CORRADE_VERIFY(!(std::is_convertible<ArrayView, int>::value));
    CORRADE_VERIFY(!(std::is_convertible<VoidArrayView, int>::value));
}

void ArrayViewTest::convertPointer() {
    int a[7];
    ArrayView b = a;
    int* bp = b;
    CORRADE_COMPARE(bp, static_cast<int*>(a));

    const ArrayView c = a;
    const int* cp = c;
    CORRADE_COMPARE(cp, static_cast<const int*>(a));

    constexpr ConstArrayView cc = Array13;
    constexpr const int* ccp = cc;
    CORRADE_COMPARE(ccp, static_cast<const int*>(Array13));

    const VoidArrayView d = a;
    const void* dp = d;
    CORRADE_COMPARE(dp, static_cast<const void*>(a));

    constexpr VoidArrayView cd = Array30;
    constexpr const void* cdp = cd;
    CORRADE_COMPARE(cdp, static_cast<const void*>(Array30));

    /* Pointer arithmetic */
    const ArrayView e = a;
    const int* ep = e + 2;
    CORRADE_COMPARE(ep, &e[2]);
}

void ArrayViewTest::convertConst() {
    int a[3];
    ArrayView b = a;
    ConstArrayView c = b;
    CORRADE_VERIFY(c == a);
    CORRADE_COMPARE(c.size(), 3);
}

void ArrayViewTest::convertVoid() {
    int a[] = {3, 4, 7, 12, 0, -15};

    /** @todo C++14: test that all the operations are really constexpr (C++11 doesn't allow void conversions IMHO) */

    /* void reference to compile-time array */
    VoidArrayView b = a;
    CORRADE_VERIFY(b == a);
    CORRADE_VERIFY(b.data() == a);
    CORRADE_COMPARE(b.size(), 6*sizeof(int));

    /* void reference to runtime array */
    VoidArrayView c = {a, 6};
    CORRADE_VERIFY(c == a);
    CORRADE_COMPARE(c.size(), 6*sizeof(int));

    /* void reference to ArrayView */
    ArrayView f = a;
    const ArrayView cf = a;
    VoidArrayView g = f;
    VoidArrayView cg = cf;
    CORRADE_VERIFY(g == f);
    CORRADE_VERIFY(cg == cf);
    CORRADE_COMPARE(g.size(), f.size()*sizeof(int));
    CORRADE_COMPARE(cg.size(), cf.size()*sizeof(int));

    /* void reference to compile-time array */
    constexpr VoidArrayView cb = Array13;
    CORRADE_VERIFY(cb == Array13);
    CORRADE_VERIFY(cb.data() == Array13);
    CORRADE_COMPARE(cb.size(), 13*sizeof(int));

    /* void reference to runtime array */
    constexpr VoidArrayView cc = {Array13, 6};
    CORRADE_VERIFY(cc == Array13);
    CORRADE_COMPARE(cc.size(), 6*sizeof(int));

    /* void reference to ArrayView */
    constexpr ConstArrayView ccf = Array13;
    constexpr VoidArrayView ccg = ccf;
    CORRADE_VERIFY(ccg == ccf);
    CORRADE_COMPARE(ccg.size(), ccf.size()*sizeof(int));
}

void ArrayViewTest::convertExternalView() {
    const int data[]{1, 2, 3, 4, 5};
    ConstIntView a{data, 5};
    CORRADE_COMPARE(a.data, data);
    CORRADE_COMPARE(a.size, 5);

    ConstArrayView b = a;
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.size(), 5);

    ConstIntView c = b;
    CORRADE_COMPARE(c.data, data);
    CORRADE_COMPARE(c.size, 5);

    auto d = arrayView(c);
    CORRADE_VERIFY((std::is_same<decltype(d), Containers::ArrayView<const int>>::value));
    CORRADE_COMPARE(d.data(), data);
    CORRADE_COMPARE(d.size(), 5);

    constexpr ConstIntView ca{Array13, 13};
    CORRADE_COMPARE(ca.data, Array13);
    CORRADE_COMPARE(ca.size, 13);

    /* Broken on Clang 3.8-svn on Apple. The same works with stock Clang 3.8
       (Travis ASan build). ¯\_(ツ)_/¯ */
    #if defined(CORRADE_TARGET_APPLE) && __clang_major__*100 + __clang_minor__ > 703
    constexpr
    #endif
    ConstArrayView cb = ca;
    CORRADE_COMPARE(cb.data(), Array13);
    CORRADE_COMPARE(cb.size(), 13);

    /* Broken on Clang 3.8-svn on Apple. The same works with stock Clang 3.8
       (Travis ASan build). ¯\_(ツ)_/¯ */
    #if defined(CORRADE_TARGET_APPLE) && __clang_major__*100 + __clang_minor__ > 703
    constexpr
    #endif
    ConstIntView cc = cb;
    CORRADE_COMPARE(cc.data, Array13);
    CORRADE_COMPARE(cc.size, 13);

    /* Broken on Clang 3.8-svn on Apple. The same works with stock Clang 3.8
       (Travis ASan build). ¯\_(ツ)_/¯ */
    #if defined(CORRADE_TARGET_APPLE) && __clang_major__*100 + __clang_minor__ > 703
    constexpr
    #else
    const
    #endif
    auto cd = arrayView(cc);
    CORRADE_VERIFY((std::is_same<decltype(cd), const Containers::ArrayView<const int>>::value));
    CORRADE_COMPARE(cd.data(), Array13);
    CORRADE_COMPARE(cd.size(), 13);

    /* Conversion from/to a different type is not allowed */
    CORRADE_VERIFY((std::is_convertible<ConstIntView, Containers::ArrayView<const int>>::value));
    CORRADE_VERIFY(!(std::is_convertible<ConstIntView, Containers::ArrayView<const float>>::value));
    CORRADE_VERIFY((std::is_convertible<Containers::ArrayView<const int>, ConstIntView>::value));
    CORRADE_VERIFY(!(std::is_convertible<Containers::ArrayView<const float>, ConstIntView>::value));
}

void ArrayViewTest::convertConstFromExternalView() {
    int data[]{1, 2, 3, 4, 5};
    IntView a{data, 5};
    CORRADE_COMPARE(a.data, &data[0]);
    CORRADE_COMPARE(a.size, 5);

    ConstArrayView b = a;
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.size(), 5);

    /* Conversion from a different type is not allowed */
    CORRADE_VERIFY((std::is_convertible<IntView, Containers::ArrayView<const int>>::value));
    CORRADE_VERIFY(!(std::is_convertible<IntView, Containers::ArrayView<const float>>::value));
}

void ArrayViewTest::convertToConstExternalView() {
    int data[]{1, 2, 3, 4, 5};
    ArrayView a = data;
    CORRADE_COMPARE(a.data(), &data[0]);
    CORRADE_COMPARE(a.size(), 5);

    ConstIntView b = a;
    CORRADE_COMPARE(b.data, data);
    CORRADE_COMPARE(b.size, 5);

    /* Conversion to a different type is not allowed */
    CORRADE_VERIFY((std::is_convertible<Containers::ArrayView<int>, ConstIntView>::value));
    CORRADE_VERIFY(!(std::is_convertible<Containers::ArrayView<float>, ConstIntView>::value));
}

void ArrayViewTest::convertVoidFromExternalView() {
    int data[]{1, 2, 3, 4, 5};
    IntView a{data, 5};
    CORRADE_COMPARE(a.data, &data[0]);
    CORRADE_COMPARE(a.size, 5);

    VoidArrayView b = a;
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.size(), 5*4);
}

void ArrayViewTest::convertVoidFromConstExternalView() {
    const int data[]{1, 2, 3, 4, 5};
    ConstIntView a{data, 5};
    CORRADE_COMPARE(a.data, &data[0]);
    CORRADE_COMPARE(a.size, 5);

    VoidArrayView b = a;
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.size(), 5*4);
}

void ArrayViewTest::emptyCheck() {
    ArrayView a;
    CORRADE_VERIFY(!a);
    CORRADE_VERIFY(a.empty());

    constexpr ConstArrayView ca;
    CORRADE_VERIFY(!ca);
    constexpr bool caEmpty = ca.empty();
    CORRADE_VERIFY(caEmpty);

    int b[5];
    ArrayView c = {b, 5};
    CORRADE_VERIFY(c);
    CORRADE_VERIFY(!c.empty());

    constexpr ConstArrayView cc = {Array13, 5};
    CORRADE_VERIFY(cc);
    constexpr bool ccEmpty = cc.empty();
    CORRADE_VERIFY(!ccEmpty);
}

/* Needs to be here in order to use it in constexpr */
constexpr int OneToSeven[]{0, 1, 2, 3, 4, 5, 6};

void ArrayViewTest::access() {
    int a[7];
    ArrayView b = a;
    for(std::size_t i = 0; i != 7; ++i)
        b[i] = i;

    CORRADE_VERIFY(b.data() == a);
    CORRADE_COMPARE(b.size(), 7);
    CORRADE_COMPARE(b.front(), 0);
    CORRADE_COMPARE(b.back(), 6);
    CORRADE_COMPARE(*(b.begin()+2), 2);
    CORRADE_COMPARE(b[4], 4);
    CORRADE_COMPARE(b.end()-b.begin(), b.size());
    CORRADE_COMPARE(b.cbegin(), b.begin());
    CORRADE_COMPARE(b.cend(), b.end());

    Containers::ArrayView<const int> c = a;
    CORRADE_COMPARE(c.data(), a);

    constexpr ConstArrayView cb = OneToSeven;

    constexpr const int* data = cb.data();
    CORRADE_VERIFY(data == OneToSeven);

    constexpr std::size_t size = cb.size();
    CORRADE_COMPARE(size, 7);

    constexpr const int* begin = cb.begin();
    constexpr const int* cbegin = cb.cbegin();
    CORRADE_COMPARE(begin, OneToSeven);
    CORRADE_COMPARE(cbegin, OneToSeven);

    constexpr const int* end = cb.end();
    constexpr const int* cend = cb.cend();
    CORRADE_COMPARE(end, OneToSeven + 7);
    CORRADE_COMPARE(cend, OneToSeven + 7);
}

void ArrayViewTest::accessVoid() {
    int a[7]{};

    VoidArrayView b = a;
    CORRADE_VERIFY(b.data() == a);
    CORRADE_COMPARE(b.size(), 7*sizeof(int));

    constexpr VoidArrayView cb = OneToSeven;

    constexpr const void* data = cb.data();
    CORRADE_VERIFY(data == OneToSeven);

    constexpr std::size_t size = cb.size();
    CORRADE_COMPARE(size, 7*sizeof(int));
}

void ArrayViewTest::accessConst() {
    /* The array is non-owning, so it should provide write access to the data */

    int a[7];
    const ArrayView b = a;
    b.front() = 0;
    *(b.begin()+1) = 1;
    *(b.cbegin()+2) = 2;
    b[3] = 3;
    *(b.end()-3) = 4;
    *(b.cend()-2) = 5;
    b.back() = 6;

    CORRADE_COMPARE(a[0], 0);
    CORRADE_COMPARE(a[1], 1);
    CORRADE_COMPARE(a[2], 2);
    CORRADE_COMPARE(a[3], 3);
    CORRADE_COMPARE(a[4], 4);
    CORRADE_COMPARE(a[5], 5);
    CORRADE_COMPARE(a[6], 6);
}

void ArrayViewTest::accessInvalid() {
    std::stringstream out;
    Error redirectError{&out};

    ArrayView a;
    a.front();
    a.back();
    CORRADE_COMPARE(out.str(),
        "Containers::ArrayView::front(): view is empty\n"
        "Containers::ArrayView::back(): view is empty\n");
}

void ArrayViewTest::rangeBasedFor() {
    int a[5];
    ArrayView b = a;
    for(auto& i: b)
        i = 3;

    CORRADE_COMPARE(b[0], 3);
    CORRADE_COMPARE(b[1], 3);
    CORRADE_COMPARE(b[2], 3);
    CORRADE_COMPARE(b[3], 3);
    CORRADE_COMPARE(b[4], 3);
}

void ArrayViewTest::sliceInvalid() {
    /* Do it this way to avoid (reasonable) warnings about out-of-bounds array
       access with `a - 1` */
    int data[6] = {0, 1, 2, 3, 4, 5};
    ArrayView a{data + 1, 5};

    CORRADE_COMPARE(a.size(), 5);

    std::ostringstream out;
    Error redirectError{&out};

    a.slice(a - 1, a);
    a.slice(a + 5, a + 6);
    a.slice(a + 2, a + 1);
    a.slice<5>(1);

    CORRADE_COMPARE(out.str(),
        "Containers::ArrayView::slice(): slice [-1:0] out of range for 5 elements\n"
        "Containers::ArrayView::slice(): slice [5:6] out of range for 5 elements\n"
        "Containers::ArrayView::slice(): slice [2:1] out of range for 5 elements\n"
        "Containers::ArrayView::slice(): slice [1:6] out of range for 5 elements\n");
}

void ArrayViewTest::sliceNullptr() {
    ArrayView a{nullptr, 5};

    ArrayView b = a.prefix(nullptr);
    CORRADE_VERIFY(!b);
    CORRADE_COMPARE(b.size(), 0);

    ArrayView c = a.suffix(nullptr);
    CORRADE_VERIFY(!c);
    CORRADE_COMPARE(c.size(), 5);

    constexpr ArrayView ca{nullptr, 5};

    constexpr ArrayView cb = ca.prefix(nullptr);
    CORRADE_VERIFY(!cb);
    CORRADE_COMPARE(cb.size(), 0);

    /* constexpr ArrayView cc = ca.suffix(nullptr) won't compile because
       arithmetic on nullptr is not allowed */

    int data[5];
    ArrayView d{data};

    ArrayView e = d.prefix(nullptr);
    CORRADE_VERIFY(!e);
    CORRADE_COMPARE(e.size(), 0);

    ArrayView f = d.suffix(nullptr);
    CORRADE_VERIFY(!f);
    CORRADE_COMPARE(f.size(), 0);

    constexpr ConstArrayView cd = Array13;
    constexpr ConstArrayView ce = cd.prefix(nullptr);
    CORRADE_VERIFY(!ce);
    CORRADE_COMPARE(ce.size(), 0);

    constexpr ConstArrayView cf = cd.suffix(nullptr);
    CORRADE_VERIFY(!cf);
    CORRADE_COMPARE(cf.size(), 0);
}

constexpr int Array5[]{1, 2, 3, 4, 5};

void ArrayViewTest::slice() {
    int data[5] = {1, 2, 3, 4, 5};
    ArrayView a = data;

    ArrayView b = a.slice(1, 4);
    CORRADE_COMPARE(b.size(), 3);
    CORRADE_COMPARE(b[0], 2);
    CORRADE_COMPARE(b[1], 3);
    CORRADE_COMPARE(b[2], 4);

    ArrayView c = a.prefix(3);
    CORRADE_COMPARE(c.size(), 3);
    CORRADE_COMPARE(c[0], 1);
    CORRADE_COMPARE(c[1], 2);
    CORRADE_COMPARE(c[2], 3);

    ArrayView d = a.suffix(2);
    CORRADE_COMPARE(d.size(), 3);
    CORRADE_COMPARE(d[0], 3);
    CORRADE_COMPARE(d[1], 4);
    CORRADE_COMPARE(d[2], 5);

    constexpr ConstArrayView ca = Array5;
    constexpr ConstArrayView cb = ca.slice(1, 4);
    CORRADE_COMPARE(cb.size(), 3);
    CORRADE_COMPARE(cb[0], 2);
    CORRADE_COMPARE(cb[1], 3);
    CORRADE_COMPARE(cb[2], 4);

    constexpr ConstArrayView cc = ca.prefix(3);
    CORRADE_COMPARE(cc.size(), 3);
    CORRADE_COMPARE(cc[0], 1);
    CORRADE_COMPARE(cc[1], 2);
    CORRADE_COMPARE(cc[2], 3);

    constexpr ConstArrayView cd = ca.suffix(2);
    CORRADE_COMPARE(cd.size(), 3);
    CORRADE_COMPARE(cd[0], 3);
    CORRADE_COMPARE(cd[1], 4);
    CORRADE_COMPARE(cd[2], 5);
}

void ArrayViewTest::slicePointer() {
    int data[5] = {1, 2, 3, 4, 5};
    ArrayView a = data;

    ArrayView b = a.slice(data + 1, data + 4);
    CORRADE_COMPARE(b.size(), 3);
    CORRADE_COMPARE(b[0], 2);
    CORRADE_COMPARE(b[1], 3);
    CORRADE_COMPARE(b[2], 4);

    ArrayView c = a.prefix(data + 3);
    CORRADE_COMPARE(c.size(), 3);
    CORRADE_COMPARE(c[0], 1);
    CORRADE_COMPARE(c[1], 2);
    CORRADE_COMPARE(c[2], 3);

    ArrayView d = a.suffix(data + 2);
    CORRADE_COMPARE(d.size(), 3);
    CORRADE_COMPARE(d[0], 3);
    CORRADE_COMPARE(d[1], 4);
    CORRADE_COMPARE(d[2], 5);

    /* MSVC 2015 chokes on all these due to (I assume) the assertion doing
       pointer arithmetic on the _data member. */
    #ifndef CORRADE_MSVC2015_COMPATIBILITY
    constexpr ConstArrayView ca = Array5;
    constexpr ConstArrayView cb = ca.slice(Array5 + 1, Array5 + 4);
    CORRADE_COMPARE(cb.size(), 3);
    CORRADE_COMPARE(cb[0], 2);
    CORRADE_COMPARE(cb[1], 3);
    CORRADE_COMPARE(cb[2], 4);

    /* The slice function checks for validity of the pointers, taking one
       pointer from _data and the second pointer from the prefix() argument.
       GCC <= 5 chokes on that, because for it doing pointer arithmetic on
       _data is apparently not constexpr. Note that the above slice() call
       worked correctly on it (both pointers treated as constexpr?). */
    #if !defined(__GNUC__) || defined(__clang__) || __GNUC__ > 5
    constexpr ConstArrayView cc = ca.prefix(Array5 + 3);
    CORRADE_COMPARE(cc.size(), 3);
    CORRADE_COMPARE(cc[0], 1);
    CORRADE_COMPARE(cc[1], 2);
    CORRADE_COMPARE(cc[2], 3);

    constexpr ConstArrayView cd = ca.suffix(Array5 + 2);
    CORRADE_COMPARE(cd.size(), 3);
    CORRADE_COMPARE(cd[0], 3);
    CORRADE_COMPARE(cd[1], 4);
    CORRADE_COMPARE(cd[2], 5);
    #endif
    #endif
}

void ArrayViewTest::sliceToStatic() {
    int data[5] = {1, 2, 3, 4, 5};
    ArrayView a = data;

    StaticArrayView<3, int> b = a.slice<3>(1);
    CORRADE_COMPARE(b[0], 2);
    CORRADE_COMPARE(b[1], 3);
    CORRADE_COMPARE(b[2], 4);

    StaticArrayView<3, int> c = a.prefix<3>();
    CORRADE_COMPARE(c[0], 1);
    CORRADE_COMPARE(c[1], 2);
    CORRADE_COMPARE(c[2], 3);

    /* Similarly to above, MSVC 2015 chokes on this due to (I assume) doing
       pointer arithmetic on _data inside the assert. */
    #ifndef CORRADE_MSVC2015_COMPATIBILITY
    constexpr ConstArrayView ca = Array5;
    constexpr StaticArrayView<3, const int> cb = ca.slice<3>(1);
    CORRADE_COMPARE(cb[0], 2);
    CORRADE_COMPARE(cb[1], 3);
    CORRADE_COMPARE(cb[2], 4);

    constexpr StaticArrayView<3, const int> cc = ca.prefix<3>();
    CORRADE_COMPARE(cc[0], 1);
    CORRADE_COMPARE(cc[1], 2);
    CORRADE_COMPARE(cc[2], 3);
    #endif
}

void ArrayViewTest::sliceToStaticPointer() {
    int data[5] = {1, 2, 3, 4, 5};
    ArrayView a = data;

    StaticArrayView<3, int> b = a.slice<3>(a + 1);
    CORRADE_COMPARE(b[0], 2);
    CORRADE_COMPARE(b[1], 3);
    CORRADE_COMPARE(b[2], 4);

    /* Similarly to above, MSVC 2015 chokes on this due to (I assume) doing
       pointer arithmetic on _data inside the assert. */
    #ifndef CORRADE_MSVC2015_COMPATIBILITY
    constexpr ConstArrayView ca = Array5;
    constexpr StaticArrayView<3, const int> cb = ca.slice<3>(ca + 1);
    CORRADE_COMPARE(cb[0], 2);
    CORRADE_COMPARE(cb[1], 3);
    CORRADE_COMPARE(cb[2], 4);
    #endif
}

void ArrayViewTest::cast() {
    std::uint32_t data[6]{};
    Containers::ArrayView<std::uint32_t> a = data;
    auto b = Containers::arrayCast<std::uint64_t>(a);
    auto c = Containers::arrayCast<std::uint16_t>(a);

    CORRADE_VERIFY((std::is_same<decltype(b), Containers::ArrayView<std::uint64_t>>::value));
    CORRADE_VERIFY((std::is_same<decltype(c), Containers::ArrayView<std::uint16_t>>::value));
    CORRADE_COMPARE(reinterpret_cast<void*>(b.begin()), reinterpret_cast<void*>(a.begin()));
    CORRADE_COMPARE(reinterpret_cast<void*>(c.begin()), reinterpret_cast<void*>(a.begin()));
    CORRADE_COMPARE(a.size(), 6);
    CORRADE_COMPARE(b.size(), 3);
    CORRADE_COMPARE(c.size(), 12);
}

void ArrayViewTest::castInvalid() {
    char data[10]{};
    Containers::ArrayView<char> a = data;

    auto b = Containers::arrayCast<std::uint16_t>(a);
    CORRADE_COMPARE(b.size(), 5);

    {
        std::ostringstream out;
        Error redirectError{&out};
        Containers::arrayCast<std::uint32_t>(a);
        CORRADE_COMPARE(out.str(), "Containers::arrayCast(): can't reinterpret 10 1-byte items into a 4-byte type\n");
    }
}

void ArrayViewTest::size() {
    int a[6]{};
    ArrayView b{a, 3};
    VoidArrayView c{a};

    CORRADE_COMPARE(Containers::arraySize(a), 6);
    CORRADE_COMPARE(Containers::arraySize(b), 3);
    CORRADE_COMPARE(Containers::arraySize(c), 24);

    constexpr ConstArrayView cb{Array13, 3};
    constexpr VoidArrayView cc{Array13};
    constexpr std::size_t sizeA = Containers::arraySize(Array13);
    constexpr std::size_t sizeB = Containers::arraySize(cb);
    constexpr std::size_t sizeC = Containers::arraySize(cc);
    CORRADE_COMPARE(sizeA, 13);
    CORRADE_COMPARE(sizeB, 3);
    CORRADE_COMPARE(sizeC, 52);
}

}}}}

CORRADE_TEST_MAIN(Corrade::Containers::Test::ArrayViewTest)
