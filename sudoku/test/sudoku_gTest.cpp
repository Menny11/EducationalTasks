// Copyright 2017, GL.
// All rights reserved.
//

#include "gtest/gtest.h"

#include "sudoku.h"

TEST(Sudoku, solve1)
{
    std::vector<Row> t = {
        {6, 0, 0,   2, 0, 4,   0, 0, 0},
        {4, 0, 0,   5, 0, 0,   7, 0, 0},
        {0, 0, 0,   0, 0, 0,   0, 8, 0},

        {0, 0, 0,   0, 8, 0,   0, 1, 0},
        {0, 0, 5,   0, 3, 0,   0, 0, 0},
        {2, 0, 0,   0, 0, 0,   0, 0, 0},

        {0, 8, 0,   0, 7, 0,   0, 0, 0},
        {0, 0, 0,   6, 0, 0,   2, 0, 0},
        {0, 1, 0,   0, 0, 0,   0, 0, 0},
    };

    std::vector<Row> e = {
        {6, 7, 8,   2, 9, 4,   1, 5, 3},
        {4, 2, 3,   5, 1, 8,   7, 6, 9},
        {1, 5, 9,   3, 6, 7,   4, 8, 2},

        {3, 6, 7,   9, 8, 2,   5, 1, 4},
        {8, 4, 5,   1, 3, 6,   9, 2, 7},
        {2, 9, 1,   7, 4, 5,   8, 3, 6},

        {5, 8, 2,   4, 7, 3,   6, 9, 1},
        {9, 3, 4,   6, 5, 1,   2, 7, 8},
        {7, 1, 6,   8, 2, 9,   3, 4, 5},
    };

    Table table(t);

    ASSERT_EQ(e, table.sudoku());
}

std::vector< std::vector<Row> > solve2_solutions =
{
    {
        {1, 4, 8,   5, 6, 2,   7, 3, 9 },
        {9, 2, 3,   7, 9, 4,   8, 1, 5 },
        {7, 5, 9,   1, 8, 3,   6, 2, 4 },

        {5, 3, 4,   8, 7, 6,   2, 9, 1 },
        {2, 8, 1,   4, 5, 9,   3, 6, 7 },
        {9, 6, 7,   2, 3, 1,   4, 5, 8 },

        {8, 9, 5,   3, 2, 7,   1, 4, 6 },
        {3, 1, 6,   9, 4, 8,   5, 7, 2 },
        {4, 7, 2,   6, 1, 5,   9, 8, 3 },
    },
    {
        {1, 4, 8,   5, 6, 2,   7, 9, 3 },
        {9, 6, 5,   7, 3, 4,   8, 2, 1 },
        {7, 2, 3,   1, 8, 9,   6, 5, 4 },

        {5, 3, 4,   8, 7, 6,   2, 1, 9 },
        {2, 8, 1,   4, 9, 5,   3, 6, 7 },
        {6, 9, 7,   2, 1, 3,   4, 8, 5 },

        {8, 5, 9,   3, 2, 7,   1, 4, 6 },
        {3, 1, 6,   9, 4, 8,   5, 7, 2 },
        {4, 7, 2,   6, 5, 1,   9, 3, 8 },
    },
};

TEST(Sudoku, solve2)
{
    std::vector<Row> t = {
        {0, 0, 0,   0, 6, 2,   0, 0, 0},
        {9, 0, 0,   0, 0, 0,   8, 0, 0},
        {0, 0, 0,   0, 0, 0,   0, 0, 0},

        {5, 3, 0,   8, 0, 0,   0, 0, 0},
        {0, 0, 0,   0, 0, 0,   0, 6, 7},
        {0, 0, 0,   0, 0, 0,   0, 0, 0},

        {0, 0, 0,   3, 0, 0,   1, 4, 0},
        {0, 0, 6,   0, 4, 0,   5, 0, 0},
        {0, 7, 2,   0, 0, 0,   0, 0, 0},
    };

    Table table(t);
    std::vector<Row> s = table.sudoku();

    bool res = false;
    for(auto e : solve2_solutions)
    {
        if ( s == e )
        {
            res = true;
            break;
        }
    }
    ASSERT_TRUE(res);
}

TEST(Sudoku, solve3)
{
    std::vector<Row> t = {
        {0, 0, 0,   7, 2, 0,   0, 0, 0},
        {0, 0, 0,   0, 0, 0,   8, 3, 0},
        {0, 9, 0,   0, 0, 0,   0, 0, 0},

        {7, 0, 0,   4, 0, 0,   1, 0, 0},
        {3, 0, 1,   0, 0, 0,   0, 0, 0},
        {0, 0, 0,   0, 0, 6,   4, 0, 0},

        {0, 6, 0,   0, 0, 0,   0, 5, 2},
        {0, 0, 0,   1, 0, 0,   0, 0, 0},
        {0, 0, 0,   0, 0, 0,   0, 7, 0},
    };

    std::vector<Row> e = {
        {6, 3, 4,   7, 2, 8,   5, 1, 9 },
        {2, 1, 7,   5, 4, 9,   8, 3, 6 },
        {8, 9, 5,   6, 3, 1,   7, 2, 4 },

        {7, 8, 6,   4, 5, 2,   1, 9, 3 },
        {3, 4, 1,   9, 8, 7,   2, 6, 5 },
        {5, 2, 9,   3, 1, 6,   4, 8, 7 },

        {1, 6, 3,   8, 7, 4,   9, 5, 2 },
        {9, 7, 2,   1, 6, 5,   3, 4, 8 },
        {4, 5, 8,   2, 9, 3,   6, 7, 1 },
    };

    Table table(t);

    ASSERT_EQ(e, table.sudoku());
}

// TEST(Sudoku, solve4)
// {
//     std::vector<Row> t = {
//         {0, 0, 0,   0, 0, 0,   0, 0, 0},
//         {0, 0, 0,   0, 0, 0,   0, 0, 0},
//         {0, 0, 0,   0, 0, 0,   0, 0, 0},

//         {0, 0, 0,   0, 0, 0,   0, 0, 0},
//         {0, 0, 0,   0, 0, 0,   0, 0, 0},
//         {0, 0, 0,   0, 0, 0,   0, 0, 0},

//         {0, 0, 0,   0, 0, 0,   0, 0, 0},
//         {0, 0, 0,   0, 0, 0,   0, 0, 0},
//         {0, 0, 0,   0, 0, 0,   0, 0, 0},
//     };

//     Table table(t);
//     table.sudoku();
// }

int main(int argc, char **argv)
{
    printf("Running main() from sudoku_gTest.cpp\n");

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
