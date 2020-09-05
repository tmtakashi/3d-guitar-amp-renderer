#include "gtest/gtest.h"
#include "HRIRSofaReader.hpp"

class HRIRSofaReaderTest : public ::testing::Test
{
    void SetUp() override
    {
        reader = new HRIRSofaReader("../../testdata/HRIR_L2702.sofa");
    }

protected:
    HRIRSofaReader *reader;
};

TEST_F(HRIRSofaReaderTest, HasCorrectSampleRate)
{
    EXPECT_EQ(reader->getSampleRate(), 48000.0);
}

TEST_F(HRIRSofaReaderTest, HasCorrectNumReceivers)
{
    EXPECT_EQ(reader->getR(), 2);
}

TEST_F(HRIRSofaReaderTest, HasCorrectNumMeasurements)
{
    EXPECT_EQ(reader->getM(), 2702);
}

TEST_F(HRIRSofaReaderTest, HasCorrectNumSamples)
{
    EXPECT_EQ(reader->getN(), 128);
}

TEST_F(HRIRSofaReaderTest, HasCorrectLeftHRIRMatrixSize)
{
    EXPECT_EQ(reader->getHRIRL().rows(), 2702);
    EXPECT_EQ(reader->getHRIRL().cols(), 128);
}

TEST_F(HRIRSofaReaderTest, HasCorrectRightHRIRMatrixSize)
{
    EXPECT_EQ(reader->getHRIRR().rows(), 2702);
    EXPECT_EQ(reader->getHRIRR().cols(), 128);
}
