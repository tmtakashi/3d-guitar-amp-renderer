#include "HRIRSofaReader.hpp"
#include <cmath>
#include <vector>
#define _USE_MATH_DEFINES
#define degreesToRadians(angleDegrees) (angleDegrees * M_PI / 180.0)

HRIRSofaReader::HRIRSofaReader(std::string_view path)
    : SofaReader<sofa::SimpleFreeFieldHRIR>(path) {
  extractIRs();
  extractPositionGrid();
}

HRIRSofaReader::~HRIRSofaReader() {}

void HRIRSofaReader::extractIRs() {
  extractDimentions();
  // check if the is for human ears
  assert(getR() == 2);

  // extract HRIR
  std::vector<double> tmp;
  irFile.GetDataIR(tmp);

  // resize HRIR containers
  hrirL.resize(getN(), getM());
  hrirR.resize(getN(), getM());
  for (std::size_t receiverIdx = 0; receiverIdx < getR(); receiverIdx++) {
    for (std::size_t measurementIdx = 0; measurementIdx < getM();
         measurementIdx++) {
      for (std::size_t numSampleIdx = 0; numSampleIdx < getN();
           numSampleIdx++) {
        std::size_t index = array3DIndex(measurementIdx, receiverIdx,
                                         numSampleIdx, getM(), getR(), getN());
        switch (receiverIdx) {
        case 0:
          hrirL(numSampleIdx, measurementIdx) = tmp[index];
          break;
        case 1:
          hrirR(numSampleIdx, measurementIdx) = tmp[index];
          break;
        default:
          break;
        }
      }
    }
  }
}

void HRIRSofaReader::extractPositionGrid() {
  sofa::Coordinates::Type coordinates;
  sofa::Units::Type units;
  const bool ok = sofaFile.GetSourcePosition(coordinates, units);
  assert(ok);
  // check if the coordinate is spherical
  assert(coordinates == sofa::Coordinates::Type::kSpherical);
  // check if the units are in degree, degree, metre
  assert(units == sofa::Units::Type::kSphericalUnits);

  std::vector<std::size_t> dims;
  sofaFile.GetVariableDimensions(dims, "SourcePosition");
  // check if the dimention is {measurements, coords}
  assert(dims.size() == 2);

  // temporary position container
  std::vector<double> pos(dims[0] * dims[1]);
  sofaFile.GetSourcePosition(&pos[0], dims[0], dims[1]);

  positionGrid.resize(dims[0], dims[1]);
  for (std::size_t i = 0; i < dims[0]; i++) {
    for (std::size_t j = 0; j < dims[1]; j++) {
      const std::size_t index = array2DIndex(i, j, dims[0], dims[1]);
      positionGrid(i, j) = pos[index];
    }
  }
  // convert degree to radian
  degToRadMat(positionGrid);
}

Eigen::MatrixXd HRIRSofaReader::getHRIRL() { return hrirL; }

Eigen::MatrixXd HRIRSofaReader::getHRIRR() { return hrirR; }
