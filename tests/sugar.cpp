

// TEST(sugar, stdlib) // python3 is now unicode so I dunno
// {
//     auto datetime = wrappy::call("datetime.datetime", 2003, 8, 4, 12, 30, 45);
//     auto formatted = wrappy::call(datetime, "isoformat");

//     EXPECT_EQ(formatted.str(), "2003-08-04T12:30:45");
// }

TEST(sugar, kwargs)
{
    auto delta = wrappy::call("datetime.timedelta", std::make_pair("hours", 1));
    auto seconds = delta.attr("seconds").num();

    EXPECT_EQ(seconds, 3600);
}
