#include "gtest/gtest.h"
#include "DRIRSofaReader.hpp"

class DRIRSofaReaderTest : public ::testing::Test
{
    void SetUp() override
    {
        reader = new DRIRSofaReader("../../testdata/Marshall1960ADRIR.sofa");
    }

protected:
    DRIRSofaReader *reader;
};

TEST_F(DRIRSofaReaderTest, HasCorrectSampleRate)
{
    std::cout << reader->getSampleRate() << std::endl;
    EXPECT_EQ(reader->getSampleRate(), 48000.0);
}

TEST_F(DRIRSofaReaderTest, HasCorrectNumReceivers)
{
    EXPECT_EQ(reader->getR(), 80);
}

TEST_F(DRIRSofaReaderTest, HasCorrectNumMeasurements)
{
    EXPECT_EQ(reader->getM(), 1);
}

TEST_F(DRIRSofaReaderTest, HasCorrectNumSamples)
{
    EXPECT_EQ(reader->getN(), 100'000);
}

TEST_F(DRIRSofaReaderTest, HasCorrectDRIRMatrixSize)
{
    auto drirs = reader->getDRIRs();
    EXPECT_EQ(drirs.rows(), 80);
    EXPECT_EQ(drirs.cols(), 100'000);
}
