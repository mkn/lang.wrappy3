
#include <vector>

TEST(stdlib, random_number)
{
    std::vector<wrappy::PythonObject> args;
    args.push_back(wrappy::construct(0));

    wrappy::callWithArgs("random.seed", args);
    auto v1 = wrappy::callWithArgs("random.random");

    wrappy::callWithArgs("random.seed", args);
    auto v2 = wrappy::callWithArgs("random.random");

    EXPECT_EQ(v1.num(), v2.num());
}

// TEST(stdlib, builtins) // issue calling builtin function
// {
//     std::vector<wrappy::PythonObject> args(1);

//     args[0] = wrappy::construct(255);
//     auto intval = wrappy::callWithArgs("hex", args);

//     args[0] = wrappy::construct(255ll);
//     auto longval = wrappy::callWithArgs("hex", args);

//     EXPECT_EQ(intval.str(), "0xff");
//     EXPECT_EQ(longval.str(), "0xffL");
// }

TEST(stdlib, error)
{
    ASSERT_THROW(wrappy::callWithArgs("asdf"),
        wrappy::WrappyError);
}

TEST(stdlib, destruction)
{

    {
        auto v1 = wrappy::callWithArgs("random.random");
    }

    {
        auto v2 = wrappy::callWithArgs("random.random");
    }

    // test successful if python didn't crash
}
