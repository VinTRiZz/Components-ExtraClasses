#include <gtest/gtest.h>

#include <memory>

#include <Components/ExtraClasses/Containers/HandlerBase.h>

using namespace ExtraClasses;

using TestHandler_t = HandlerBase<std::string>;

TEST(HandlerBase, RegularWork) {
    const auto sampleText = "Test sample string";
    auto pSource = std::make_shared<std::string>(sampleText);

    const auto otherSampleText = "Not a sample string";
    auto pOtherSource = std::make_shared<std::string>(otherSampleText);

    // Basic checks
    TestHandler_t hdlSource(pSource.get());
    ASSERT_TRUE(hdlSource.isValid());
    ASSERT_EQ(bool(hdlSource), hdlSource.isValid());
    ASSERT_EQ(sampleText, *hdlSource);
    ASSERT_EQ(sampleText, *hdlSource.get());

    // Removing checks
    hdlSource.invalidate();
    ASSERT_FALSE(hdlSource.isValid());
    ASSERT_EQ(bool(hdlSource), hdlSource.isValid());
    try {
        auto pInvalidSource = hdlSource.get();
        ASSERT_TRUE(pInvalidSource && false) << "Source must not be returned after invalidating";
    } catch (const std::runtime_error& ex) {
        // All's valid
    }

    // Assign checks - value
    hdlSource = pOtherSource.get();
    ASSERT_TRUE(hdlSource.isValid());
    ASSERT_EQ(bool(hdlSource), hdlSource.isValid());
    ASSERT_EQ(otherSampleText, *hdlSource);
    ASSERT_EQ(otherSampleText, *hdlSource.get());

    // Assign checks - copy
    auto hdlCopy = hdlSource;
    ASSERT_TRUE(hdlCopy.isValid());
    ASSERT_EQ(bool(hdlCopy), hdlCopy.isValid());
    ASSERT_EQ(otherSampleText, *hdlCopy);
    ASSERT_EQ(otherSampleText, *hdlCopy.get());
}