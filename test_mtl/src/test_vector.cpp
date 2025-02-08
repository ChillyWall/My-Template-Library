#define GLOG_USE_GLOG_EXPORT

#include <gtest/gtest.h>
#include <mtl/vector.h>

using mtl::vector;

TEST(TestConstructor, TestDefaultConstructor) {
    vector<int> v;
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
    EXPECT_EQ(v.data(), nullptr);
}

TEST(TestConstructor, TestInitListConstructor) {
    vector<int> v({0, 1, 2, 3, 4});
    EXPECT_EQ(v.size(), 5);
    EXPECT_EQ(v.capacity(), 5);
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(v.data()[i], i);
    }
}

TEST(TestConstructor, TestCopyConstructor) {
    vector<int> v1({0, 1, 2, 3, 4});
    vector<int> v2(v1);
    EXPECT_EQ(v2.size(), v1.size());
    EXPECT_EQ(v2.capacity(), v1.capacity());
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(v1.data()[i], v2.data()[i]);
    }
}

TEST(TestConstructor, TestMoveConstrucor) {
    vector<int> v1({0, 1, 2, 3, 4});
    vector<int> v2(std::move(v1));
    EXPECT_EQ(v2.size(), 5);
    EXPECT_EQ(v2.capacity(), 5);
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(v2.data()[i], i);
    }
    EXPECT_EQ(v1.size(), 0);
    EXPECT_EQ(v1.capacity(), 0);
    EXPECT_EQ(v1.data(), nullptr);
}

class TestVector : public testing::Test {
public:
    TestVector() : v({0, 1, 2, 3, 4}), vc({0, 1, 2, 3, 4}) {}
    vector<int> v;
    const vector<int> vc;
};

TEST_F(TestVector, TestClear) {
    v.clear();
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
    EXPECT_EQ(v.data(), nullptr);

    EXPECT_TRUE(v.empty());
}

TEST_F(TestVector, TestVectorIndex) {
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(v[i], i);
        v[i] = i + 1;
    }

    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(v[i], i + 1);
    }
}

TEST_F(TestVector, TestVectorIndexConst) {
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(vc[i], i);
    }
}

TEST_F(TestVector, TestVectorAt) {
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(v.at(i), i);
        v.at(i) = i + 1;
    }

    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(v.at(i), i + 1);
    }
}

TEST_F(TestVector, TestVectorAtConst) {
    const vector<int> v({0, 1, 2, 3, 4});
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(vc.at(i), i);
    }
}

TEST_F(TestVector, TestVectorFront) {
    EXPECT_EQ(v.front(), 0);
    v.front() = 1;
    EXPECT_EQ(v.front(), 1);
}

TEST_F(TestVector, TestVectorFrontConst) {
    EXPECT_EQ(vc.front(), 0);
}

TEST_F(TestVector, TestVectorBackConst) {
    EXPECT_EQ(vc.back(), 4);
}

TEST_F(TestVector, TestVectorBack) {
    EXPECT_EQ(v.back(), 4);
    v.back() = 5;
    EXPECT_EQ(v.back(), 5);
}

TEST_F(TestVector, TestCopyAssignment) {
    vector<int> v1 = v;
    EXPECT_EQ(v.size(), v1.size());
    EXPECT_EQ(v.capacity(), v1.capacity());
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(v1.data()[i], v.data()[i]);
    }
}

TEST_F(TestVector, TestMoveAssignment) {
    vector<int> v1 = std::move(v);
    EXPECT_EQ(v1.size(), 5);
    EXPECT_EQ(v1.capacity(), 5);
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(v1.data()[i], i);
    }
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
    EXPECT_EQ(v.data(), nullptr);
}

TEST_F(TestVector, TestPushBack) {
    v.push_back(5);
    EXPECT_EQ(v.size(), 6);
    EXPECT_EQ(v.capacity(), 10);
    EXPECT_EQ(v.back(), 5);
}

TEST_F(TestVector, TestPopBack) {
    v.pop_back();
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v.capacity(), 5);
    EXPECT_EQ(v.back(), 3);
    for (int i = 0; i < 4; ++i) {
        v.pop_back();
    }
    EXPECT_THROW(v.pop_back(), std::out_of_range);
}

TEST_F(TestVector, TestPushFront) {
    v.push_front(-1);
    EXPECT_EQ(v.size(), 6);
    EXPECT_EQ(v.capacity(), 10);
    EXPECT_EQ(v.front(), -1);
}

TEST_F(TestVector, TestPopFront) {
    v.pop_front();
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v.capacity(), 5);
    EXPECT_EQ(v.front(), 1);

    for (int i = 0; i < 4; ++i) {
        v.pop_back();
    }
    EXPECT_THROW(v.pop_front(), std::out_of_range);
}

TEST_F(TestVector, TestIteratorRandomAccess) {
    auto itr = v.begin();
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(*itr, i);
        ++itr;
    }
}

TEST_F(TestVector, TestIteratorRandomAccessConst) {
    auto itr = v.begin();
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(*itr, i);
        ++itr;
    }
}

TEST_F(TestVector, TestIteratorSubtraction) {
    auto itr1 = vc.begin();
    auto itr2 = vc.cend();
    EXPECT_EQ(itr2 - itr1, 5);
}

TEST_F(TestVector, TestIteratorPrefixIncrement) {
    auto itr1 = v.begin();
    auto itr2 = ++itr1;
    EXPECT_EQ(itr2, v.begin() + 1);
    EXPECT_EQ(itr1, v.begin() + 1);
}

TEST_F(TestVector, TestIteratorPostfixIncrement) {
    auto itr1 = v.begin();
    auto itr2 = itr1++;
    EXPECT_EQ(itr2, v.begin());
    EXPECT_EQ(itr1, v.begin() + 1);
}

TEST_F(TestVector, TestInsert) {
    v.insert(v.begin() + 2, 5);
    EXPECT_EQ(v.size(), 6);
    EXPECT_EQ(v.capacity(), 10);
    EXPECT_EQ(v[2], 5);
    for (int i = 3; i < 6; ++i) {
        EXPECT_EQ(v[i], i - 1);
    }
}

TEST_F(TestVector, TestInsertRange) {
    vector<int> v1({5, 6, 7});
    v.insert(v.begin() + 2, v1.begin(), v1.end());
    EXPECT_EQ(v.size(), 8);
    EXPECT_EQ(v.capacity(), 10);
    for (int i = 0; i < 2; ++i) {
        EXPECT_EQ(v[i], i);
    }
    for (int i = 2; i < 5; ++i) {
        EXPECT_EQ(v[i], i + 3);
    }
    for (int i = 5; i < 8; ++i) {
        EXPECT_EQ(v[i], i - 3);
    }
}

TEST_F(TestVector, TestRemove) {
    v.remove(v.begin() + 2);
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v.capacity(), 5);
    for (int i = 0; i < 2; ++i) {
        EXPECT_EQ(v[i], i);
    }
    for (int i = 2; i < 4; ++i) {
        EXPECT_EQ(v[i], i + 1);
    }
}

TEST_F(TestVector, TestRemoveRange) {
    v.remove(v.begin() + 2, v.begin() + 4);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v.capacity(), 5);
    for (int i = 0; i < 2; ++i) {
        EXPECT_EQ(v[i], i);
    }
    for (int i = 2; i < 3; ++i) {
        EXPECT_EQ(v[i], i + 2);
    }
}

TEST_F(TestVector, TestSplice) {
    auto v1 = v.splice(1, 4);

    EXPECT_EQ(v1.size(), 3);
    EXPECT_EQ(v1.capacity(), 3);
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(v1[i], i + 1);
    }
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
