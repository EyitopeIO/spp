#include <hash.h>
#include <gtest/gtest.h>

namespace
{


    TEST(HashTest, ConsistentHash)
    {
        std::string testString = "test";
        std::string testString2 = "test";
        size_t firstHash = get_string_hash(testString);
        size_t secondHash = get_string_hash(testString2);
        ASSERT_EQ(firstHash, secondHash);
    }

    /* Test to ensure get_string_hash produces different hash values for different inputs */
    TEST(HashTest, DifferentStringsDifferentHash)
    {
        std::string string1 = "test1";
        std::string string2 = "test2";
        ASSERT_NE(get_string_hash(string1), get_string_hash(string2));
    }

    /* Test to ensure get_string_index returns an index within expected range */
    TEST(HashTest, IndexInRange)
    {
        std::string testString = "test";
        size_t hash;
        int index = get_string_index(testString, &hash);
        ASSERT_GE(index, 0);
        ASSERT_LT(index, 256); // 256 is the size of the hash table

        testString = "Why, hello there! Let's test this hash function!";
        index = get_string_index(testString, &hash);
        ASSERT_GE(index, 0);
        ASSERT_LT(index, 256);

        testString = "This is a very long string that should produce a hash"
                     "index within the range of the hash table. Is it really"
                     "long enough? What if it was 256 * 256 in length? Let's"
                     "find out!";
        index = get_string_index(testString, &hash);
        ASSERT_GE(index, 0);
        ASSERT_LT(index, 256);
    }

    /* Test to ensure find_string_in_hash_table finds an existing string */
    TEST(HashTest, FindExistingString)
    {
        std::string testString = "Let's talk about Donlald Trump!";
        add_string_to_hash_table(testString);
        table_element* result = find_string_in_hash_table(testString);
        ASSERT_NE(result, nullptr);
        if (result)
            ASSERT_EQ(std::string(result->text), testString);
        remove_string_from_hash_table(testString);
    }

    /* Test to ensure find_string_in_hash_table returns nullptr for a non-existing string */
    TEST(HashTest, FindNonExistingString)
    {
        std::string testString = "nonExisting";
        table_element* result = find_string_in_hash_table(testString);
        ASSERT_EQ(result, nullptr);
    }
}