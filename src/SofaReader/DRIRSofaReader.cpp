#include "DRIRSofaReader.hpp"

DRIRSofaReader::DRIRSofaReader(std::string_view path)
    : SofaReader<sofa::GeneralFIR>(path), firFile(std::string(path)) {
  extractIRs();
  extractPositionGrid();
}

void DRIRSofaReader::extractIRs() {
  extractDimentions();
  // ensure there is only one measurement set
  assert(getM() == 1);

  std::vector<double> tmp;
  irFile.GetDataIR(tmp);
  // resize DRIR container
  drirs.resize(getR(), getN());
  for (std::size_t measurementIdx = 0; measurementIdx < getM();
       measurementIdx++) {
    for (std::size_t receiverIdx = 0; receiverIdx < getR(); receiverIdx++) {
      for (std::size_t numSampleIdx = 0; numSampleIdx < getN();
           numSampleIdx++) {
        std::size_t index = array3DIndex(measurementIdx, receiverIdx,
                                         numSampleIdx, getM(), getR(), getN());
        drirs(receiverIdx, numSampleIdx) = tmp[index];
      }
    }
  }
}

void DRIRSofaReader::extractPositionGrid() {
  sofa::Coordinates::Type coordinates;
  sofa::Units::Type units;
  const bool ok = sofaFile.GetReceiverPosition(coordinates, units);
  assert(ok);
  // check if the coordinate is spherical
  assert(coordinates == sofa::Coordinates::Type::kSpherical);
  // check if the units are in degree, degree, metre
  assert(units == sofa::Units::Type::kSphericalUnits);

  std::vector<std::size_t> dims;
  sofaFile.GetVariableDimensions(dims, "ReceiverPosition");
  // check if the dimention is {R, C, I}
  assert(dims.size() == 3);
  // check if I == 1
  assert(dims[2] == 1);
  // temporary position container
  std::vector<double> pos(dims[0] * dims[1] * dims[2]);
  sofaFile.GetReceiverPosition(&pos[0], dims[0], dims[1], dims[2]);

  positionGrid.resize(dims[0], dims[1]);
  for (std::size_t i = 0; i < dims[0]; i++) {
    for (std::size_t j = 0; j < dims[1]; j++) {
      for (std::size_t k = 0; k < dims[2]; k++) {
        const std::size_t index =
            array3DIndex(i, j, k, dims[0], dims[1], dims[2]);
        positionGrid(i, j) = pos[index];
      }
    }
  }
  // convert degree to radian
  degToRadMat(positionGrid);
}

Eigen::MatrixXd DRIRSofaReader::getDRIRs() { return drirs; }
