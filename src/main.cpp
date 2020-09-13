#include "AudioFile.h"
#include "BinauralIRRenderer.hpp"
#include "DRIRSofaReader.hpp"
#include "HRIRSofaReader.hpp"
#include "SphUtils.hpp"
#include <Eigen/Dense>
#include <iostream>
#include <string>

int main() {
  DRIRSofaReader drirReader("test/testdata/Marshall1960ADRIR.sofa");
  HRIRSofaReader hrirReader("data/HRIR_L2702.sofa");

  auto drirs = drirReader.getDRIRs();
  auto hrirsL = hrirReader.getHRIRL();
  auto hrirsR = hrirReader.getHRIRR();

  auto drirPositionGrid = drirReader.getPositionGrid();
  auto hrirPositionGrid = hrirReader.getPositionGrid();

  double fs = drirReader.getSampleRate();
  double radius = drirPositionGrid(0, 2);
  unsigned order = 7;
  unsigned radialFiltNfft = 1024;
  /* MicArrayConfig micArrayConfig{ArrayType::open, MicType::omni}; */
  /* Eigen::MatrixXd drirs = Eigen::MatrixXd::Random(32, 80); */
  /* Eigen::MatrixXd hrirsL = Eigen::MatrixXd::Random(32, 100); */
  /* Eigen::MatrixXd hrirsR = Eigen::MatrixXd::Random(32, 100); */

  /* Eigen::MatrixXd drirPositionGrid = Eigen::MatrixXd::Ones(80, 3); */
  /* Eigen::MatrixXd hrirPositionGrid = Eigen::MatrixXd::Ones(100, 3); */
  /* double radius = drirPositionGrid(0, 2); */
  /* unsigned order = 7; */
  /* unsigned radialFiltNfft = 32; */
  /* double fs = 48000.0; */
  MicArrayConfig micArrayConfig{ArrayType::open, MicType::omni};

  BinauralIRRenderer renderer(drirs, drirPositionGrid, hrirsL, hrirsR,
                              hrirPositionGrid, fs, radius, order,
                              radialFiltNfft, micArrayConfig);

  for (std::size_t i = 0; i < 360; i++) {
    std::vector<double> irL = renderer.getLeftIR(i);
    std::vector<double> irR = renderer.getRightIR(i);

    AudioFile<double> audioFile;
    AudioFile<double>::AudioBuffer buffer;
    buffer.resize(2);
    buffer[0].resize(irL.size());
    buffer[1].resize(irR.size());
    for (int i = 0; i < irL.size(); i++) {
      buffer[0][i] = irL[i];
      buffer[1][i] = irR[i];
    }
    bool ok = audioFile.setAudioBuffer(buffer);
    audioFile.setBitDepth(16);
    audioFile.setSampleRate(fs);
    audioFile.save(std::string("data/generated_irs/ir.") + std::to_string(i) +
                   std::string(".wav"));
  }
}
