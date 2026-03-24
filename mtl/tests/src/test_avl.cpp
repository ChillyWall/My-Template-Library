#include <gtest/gtest.h>

#include <mtl/avl_tree.h>

using namespace mtl;

// 基础功能测试
TEST(AVLTreeTest, BasicOperations) {
    avl_tree<int> tree;

    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.size(), 0);

    // 测试插入
    tree.insert(10);
    tree.insert(20);
    tree.insert(5);

    EXPECT_FALSE(tree.empty());
    EXPECT_EQ(tree.size(), 3);
    EXPECT_TRUE(tree.contain(10));
    EXPECT_TRUE(tree.contain(20));
    EXPECT_TRUE(tree.contain(5));
    EXPECT_FALSE(tree.contain(100));

    // 测试删除
    EXPECT_EQ(tree.remove(20), 1);
    EXPECT_EQ(tree.size(), 2);
    EXPECT_FALSE(tree.contain(20));

    // 删除不存在的元素
    EXPECT_EQ(tree.remove(100), 0);
    EXPECT_EQ(tree.size(), 2);
}

// 测试 AVL 平衡性（触发旋转）
TEST(AVLTreeTest, BalanceAndRotations) {
    avl_tree<int> tree;

    // 触发单旋转 (Right Rotation)
    // 插入顺序: 30, 20, 10
    tree.insert(30);
    tree.insert(20);
    tree.insert(10);

    // 验证顺序是否正确 (AVL 内部应该已经通过旋转平衡)
    auto it = tree.begin();
    EXPECT_EQ(*it++, 10);
    EXPECT_EQ(*it++, 20);
    EXPECT_EQ(*it++, 30);
    EXPECT_EQ(it, tree.end());

    // 触发双旋转 (Left-Right Rotation)
    avl_tree<int> tree2;
    tree2.insert(30);
    tree2.insert(10);
    tree2.insert(20);  // 10 的右子节点，触发双旋

    EXPECT_EQ(tree2.size(), 3);
    EXPECT_TRUE(tree2.contain(10));
    EXPECT_TRUE(tree2.contain(20));
    EXPECT_TRUE(tree2.contain(30));
}

// 测试迭代器遍历
TEST(AVLTreeTest, IteratorTraversal) {
    avl_tree<int> tree;
    std::vector<int> values = {50, 30, 70, 20, 40, 60, 80};
    for (int v : values)
        tree.insert(v);

    std::vector<int> expected = {20, 30, 40, 50, 60, 70, 80};
    std::vector<int> actual;

    for (auto it = tree.begin(); it != tree.end(); ++it) {
        actual.push_back(*it);
    }

    EXPECT_EQ(actual, expected);
}

// 测试拷贝与移动语义
TEST(AVLTreeTest, CopyAndMove) {
    avl_tree<int> tree1;
    tree1.insert(10);
    tree1.insert(20);

    // 拷贝构造
    avl_tree<int> tree2 = tree1;
    EXPECT_EQ(tree2.size(), 2);
    EXPECT_TRUE(tree2.contain(10));

    // 修改 tree2 不应影响 tree1
    tree2.insert(30);
    EXPECT_FALSE(tree1.contain(30));

    // 移动构造
    avl_tree<int> tree3 = std::move(tree1);
    EXPECT_EQ(tree3.size(), 2);
    EXPECT_TRUE(tree1.empty());  // 原树应被置空
}

// 测试大量数据的稳定性
TEST(AVLTreeTest, LargeScaleData) {
    avl_tree<int> tree;
    const int count = 1000;

    for (int i = 0; i < count; ++i) {
        tree.insert(i);
    }

    EXPECT_EQ(tree.size(), count);

    for (int i = 0; i < count; ++i) {
        if (!tree.contain(i)) {
            FAIL() << "Tree should contain " << i;
        }
    }

    for (int i = 0; i < count; ++i) {
        tree.remove(i);
    }
    EXPECT_TRUE(tree.empty());
}

// 测试边界情况：删除根节点
TEST(AVLTreeTest, RemoveRoot) {
    avl_tree<int> tree;
    tree.insert(10);
    tree.insert(5);
    tree.insert(15);

    EXPECT_EQ(tree.remove(10), 1);  // 删除根
    EXPECT_EQ(tree.size(), 2);
    EXPECT_FALSE(tree.contain(10));
    EXPECT_TRUE(tree.contain(5));
    EXPECT_TRUE(tree.contain(15));
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
