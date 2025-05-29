#include <fraction.h>
#include <gtest/gtest.h>


TEST(fraction_test, basic_arithmetic) {
    fraction a(big_int(1), big_int(2));
    fraction b(1, 3);
    EXPECT_EQ(a + b, fraction(5, 6));
    EXPECT_EQ(a - b, fraction(1, 6));
    EXPECT_EQ(a * b, fraction(1, 6));
    EXPECT_EQ(a / b, fraction(3, 2));
}

TEST(fraction_test, basic_arithmetic2) {
    fraction a(15, 67);
    fraction b(32, 97);
    EXPECT_EQ(a + b, fraction(3599, 6499));
    EXPECT_EQ(a - b, fraction(-689, 6499));
    EXPECT_EQ(a * b, fraction(480, 6499));
    EXPECT_EQ(a / b, fraction(1455, 2144));
}

TEST(fraction_test, basic_arithmetic3) {
    fraction a(255459636966, 95959595);
    fraction b(266669644, 926191);
    EXPECT_EQ(a + b, fraction(262193927658210686, 88876913252645));
    EXPECT_EQ(a - b, fraction(211014905584142326, 88876913252645));
}

TEST(fraction_test, big_arithmetic1) {
    big_int a("12");
    big_int b("34");
    fraction c(a, b);
    EXPECT_EQ(c, fraction(6, 17));
}

TEST(fraction_test, simplification1) {
    fraction a(2, 4);
    fraction b(4, 2);
    fraction c(-3, -6);
    EXPECT_EQ(a, fraction(1, 2));
    EXPECT_EQ(b, fraction(2, 1));
    EXPECT_EQ(c, fraction(1, 2));
}

TEST(fraction_test, simplification2) {
    fraction a(121212, 5454);
    fraction b(45595, 85);
    fraction c(123456789, 67545);
    EXPECT_EQ(a, fraction(6734, 303));
    EXPECT_EQ(b, fraction(9119, 17));
    EXPECT_EQ(c, fraction(13717421, 7505));
}

TEST(fraction_test, simplification3) {
    fraction a(big_int("10000000"), big_int("100"));
    EXPECT_EQ(a, fraction(100000, 1));
}

TEST(fraction_test, simplification4) {
    fraction h(big_int("98754321"), big_int("3423234"));
    EXPECT_EQ(h, fraction(32918107, 1141078));
}

TEST(fraction_test, negative_fractions) {
    fraction a(-1, 2);
    fraction b(1, -3);
    EXPECT_EQ(a.to_string(), "-1/2");
    EXPECT_EQ(b.to_string(), "-1/3");
    EXPECT_EQ(a + b, fraction(-5, 6));
    EXPECT_EQ(a * b, fraction(1, 6));
}

TEST(fraction_test, comparison) {
    fraction a(3, 4);
    fraction b(2, 3);
    fraction c(6, 8);
    EXPECT_TRUE(a > b);
    EXPECT_TRUE(b < a);
    EXPECT_EQ(a, c);
}

TEST(fraction_test, funny_test) {
    fraction zero(0, 5);
    EXPECT_EQ(zero, fraction(0, 1));

    fraction a(123456, 654321);
    fraction b(654321, 123456);
    EXPECT_EQ(a * b, fraction(1, 1)); // (123456/654321) * (654321/123456) = 1
}

TEST(fraction_test, exceptions) {
    EXPECT_THROW(fraction(5, 0), std::logic_error);
    fraction a(1, 2);
    fraction zero(0, 1);
    EXPECT_THROW(a / zero, std::logic_error);
}


TEST(fraction_test, trigonometric1) {
    fraction zero(0, 1);
    EXPECT_EQ(zero.sin(fraction(1, 1000)), fraction(0, 1));
    EXPECT_EQ(zero.cos(fraction(1, 1000)), fraction(1, 1));
}

TEST(fraction_test, root_test) {
    fraction temp(169, 1);
    std::cout << "ROOT OF 4: " << temp.root(2, {1, 100}) << "\n";
}

TEST(fraction_test, pow_function) {
    fraction a(23, 15);
    fraction b = a.pow(4);
    EXPECT_EQ(b, fraction(279841, 50625));
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}